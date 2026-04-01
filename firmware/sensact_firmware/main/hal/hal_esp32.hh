#pragma once
#include "hal.hh"
#include <sensact_logger.hh>
#include <driver/gpio.h>
#include <esp_twai.h>
#include <esp_twai_onchip.h>
#include <driver/uart.h>
#include <driver/mcpwm_prelude.h>
#include <freertos/queue.h>
#include <vector>
#include <driver/temperature_sensor.h>
#include <logger.hh>
#include <messagecodes.hh>
#include <esp_timer.h>

#define TAG "HAL"

using namespace sensact::hal;
using namespace sensact;
namespace sensact::hal
{
	struct CanRxFrame
	{
		uint32_t id;
		uint8_t len;
		uint8_t data[8];
	};

	constexpr ledc_timer_bit_t DUTY_RESOLUTION = LEDC_TIMER_10_BIT;
	
	class cESP32 : public iHAL
	{
	private:
		static bool OnCanRxDone(twai_node_handle_t node, const twai_rx_done_event_data_t *edata, void *user_ctx)
		{
			(void)edata;
			auto *self = static_cast<cESP32 *>(user_ctx);
			if (!self || !self->can_rx_queue)
			{
				return false;
			}

			CanRxFrame out = {};
			uint8_t payload[8] = {0};
			twai_frame_t rx_frame = {};
			rx_frame.buffer = payload;
			rx_frame.buffer_len = sizeof(payload);
			if (twai_node_receive_from_isr(node, &rx_frame) != ESP_OK)
			{
				return false;
			}

			out.id = rx_frame.header.id;
			out.len = static_cast<uint8_t>(twaifd_dlc2len(rx_frame.header.dlc));
			if (out.len > sizeof(out.data))
			{
				out.len = sizeof(out.data);
			}
			for (size_t i = 0; i < out.len; i++)
			{
				out.data[i] = payload[i];
			}

			BaseType_t hp_task_woken = pdFALSE;
			xQueueSendFromISR(self->can_rx_queue, &out, &hp_task_woken);
			return hp_task_woken == pdTRUE;
		}

	protected:
		void *modbus_master_handler{nullptr};
		temperature_sensor_handle_t temp_handle{nullptr};
		twai_node_handle_t twai_node{nullptr};
		QueueHandle_t can_rx_queue{nullptr};

		void SetupCAN(gpio_num_t tx, gpio_num_t rx)
		{
			if (!can_rx_queue)
			{
				can_rx_queue = xQueueCreate(32, sizeof(CanRxFrame));
				assert(can_rx_queue);
			}

			twai_onchip_node_config_t node_config = {};
			node_config.io_cfg.tx = tx;
			node_config.io_cfg.rx = rx;
			node_config.io_cfg.quanta_clk_out = GPIO_NUM_NC;
			node_config.io_cfg.bus_off_indicator = GPIO_NUM_NC;
			node_config.bit_timing.bitrate = 125000;
			node_config.tx_queue_depth = 16;
			node_config.fail_retry_cnt = -1;

			ESP_ERROR_CHECK(twai_new_node_onchip(&node_config, &twai_node));

			twai_event_callbacks_t cbs = {};
			cbs.on_rx_done = OnCanRxDone;
			ESP_ERROR_CHECK(twai_node_register_event_callbacks(twai_node, &cbs, this));
			ESP_ERROR_CHECK(twai_node_enable(twai_node));
		}

		void SetupMCPWMForLEDs(gpio_num_t gpio1, gpio_num_t gpio2)
		{
			const uint32_t RESOLUTION_HZ = 1'000'000; // 1MHz resolution
			const uint32_t PERIOD_TICKS = 1024; // 1024 ticks per period --> circa 1kHz PWM frequency
			const uint32_t CARRIER_FREQ_HZ = 1000'000; // muss nach ersten Experimenten genau so groß sein, wie Resolution_Hz, sonst gibt es "E (1134) mcpwm: mcpwm_set_prescale(279): group prescale conflict, already is 2 but attempt to 32"
			//configuration objects
			gpio_num_t gpio_nums[2]{gpio1, gpio2};
			mcpwm_timer_config_t timer_config = {};
			timer_config.group_id = 0;
			timer_config.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
			timer_config.resolution_hz = RESOLUTION_HZ;
			timer_config.count_mode = MCPWM_TIMER_COUNT_MODE_UP;
			timer_config.period_ticks = PERIOD_TICKS;

			mcpwm_operator_config_t operator_config = {};
			operator_config.group_id = 0;

			mcpwm_comparator_config_t comparator_config = {};
			comparator_config.flags.update_cmp_on_tez = true;

			mcpwm_generator_config_t generator_config = {};
			//PWM hat etwa 1kHz
			mcpwm_carrier_config_t carrier_config = {};
			carrier_config.clk_src = MCPWM_CARRIER_CLK_SRC_DEFAULT;
			carrier_config.frequency_hz = CARRIER_FREQ_HZ;
			carrier_config.first_pulse_duration_us = 1'000'000 / CARRIER_FREQ_HZ;
			carrier_config.duty_cycle = 50;
		
			//Handles
			mcpwm_timer_handle_t timer{nullptr};
			mcpwm_oper_handle_t operators[2];
			mcpwm_cmpr_handle_t comparators[2];
			mcpwm_gen_handle_t generators[2];

			//Allocation and initialization of the handles
			ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));
			
			for(int i=0; i<2; i++){
				ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operators[i]));
				ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operators[i], timer));	
				ESP_ERROR_CHECK(mcpwm_new_comparator(operators[i], &comparator_config, &comparators[i]));
				generator_config.gen_gpio_num = gpio_nums[i];
				ESP_ERROR_CHECK(mcpwm_new_generator(operators[i], &generator_config, &generators[i]));
				ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparators[i], 512));
				ESP_ERROR_CHECK(mcpwm_operator_apply_carrier(operators[i], &carrier_config));
			}
			ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
			ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
		}

	

		void SetupLedcCommonTimer(ledc_timer_t timer, uint32_t freq_hz){
			
			// Prepare and then apply the LEDC PWM timer configuration
			ledc_timer_config_t ledc_timer = {};
			ledc_timer.speed_mode= LEDC_LOW_SPEED_MODE;//obligatory according to https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/api-reference/peripherals/ledc.html
			ledc_timer.duty_resolution  = DUTY_RESOLUTION;
			ledc_timer.timer_num        = timer;
			ledc_timer.freq_hz          = freq_hz;
			ledc_timer.clk_cfg          = LEDC_AUTO_CLK;
			ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
		}
		void SetupLedcChannel(ledc_timer_t timer, ledc_channel_t channel, gpio_num_t gpio, uint32_t initialDuty=0){

			// Prepare and then apply the LEDC PWM channel configuration
			ledc_channel_config_t ledc_channel = {};
			ledc_channel.speed_mode     = LEDC_LOW_SPEED_MODE;
			ledc_channel.channel        = channel;
			ledc_channel.timer_sel      = timer;
			ledc_channel.gpio_num       = (int)gpio;
			ledc_channel.duty           = initialDuty;
			ledc_channel.hpoint         = 0;
			ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
		}

		void SetDuty(ledc_channel_t channel, uint16_t value_0_65635){
			uint16_t val = value_0_65635>>(16-DUTY_RESOLUTION);
			ESP_LOGI(TAG, "SetDuty %d/1024 = %6.2f", val, val/1024.0f);
			ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, val);
			ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
		}

		ErrorCode SetupModbusMaster(uart_port_t uartPort, int baudrate, gpio_num_t tx, gpio_num_t rx, gpio_num_t rts)
		{
			/*
			TODO
			mb_communication_info_t comm = {};
			comm.port=uartPort;
			comm.mode = MB_MODE_RTU;
			comm.baudrate = baudrate;
			comm.parity = MB_PARITY_NONE;

			esp_err_t err = mbc_master_init(MB_PORT_SERIAL_MASTER, &modbus_master_handler);
			MASTER_CHECK((modbus_master_handler != NULL), ErrorCode::GENERIC_ERROR, "mb controller initialization fail.");
			MASTER_CHECK((err == ESP_OK), ErrorCode::GENERIC_ERROR, "mb controller initialization fail, returns(0x%x).", (uint32_t)err);
			err = mbc_master_setup((void*)&comm);
			MASTER_CHECK((err == ESP_OK), ErrorCode::GENERIC_ERROR, "mb controller setup fail, returns(0x%x).", (uint32_t)err);

			// Set UART pin numbers
			err = uart_set_pin(uartPort, (int)tx, (int)rx, (int)rts, UART_PIN_NO_CHANGE);
			MASTER_CHECK((err == ESP_OK), ErrorCode::GENERIC_ERROR, "mb serial set pin failure, uart_set_pin() returned (0x%x).", (uint32_t)err);

			//Start Master
			err = mbc_master_start();
			MASTER_CHECK((err == ESP_OK), ErrorCode::GENERIC_ERROR, "mb controller start fail, returns(0x%x).", (uint32_t)err);

			// Set driver mode to Half Duplex
			err = uart_set_mode(uartPort, UART_MODE_RS485_HALF_DUPLEX);
			MASTER_CHECK((err == ESP_OK), ErrorCode::GENERIC_ERROR, "mb serial set mode failure, uart_set_mode() returned (0x%x).", (uint32_t)err);

			vTaskDelay(5);
			*/
			return ErrorCode::OK;
		}

	public:
		
		tms_t GetMillisS64() override
		{
			return esp_timer_get_time() / 1000;
		}

		ErrorCode GetBoardTemperature(float &temperatureCelcius) override{
			if(!this->temp_handle){
				temperatureCelcius=std::numeric_limits<float>::quiet_NaN();
				return ErrorCode::FUNCTION_NOT_AVAILABLE;
			}
            return temperature_sensor_get_celsius(this->temp_handle, &temperatureCelcius)==ESP_OK?ErrorCode::OK:ErrorCode::GENERIC_ERROR;
        }
		
		
		ErrorCode GetU16Input(InOutId id, uint16_t &inputState) override
		{
			if (id == sensact::magic::INPUT0)
			{
				inputState = 0;
				return ErrorCode::OK;
			}
			if (id == sensact::magic::INPUT1)
			{
				inputState = UINT16_MAX;
				return ErrorCode::OK;
			}
			if (id >= GPIO_NUM_MAX)
			{
				return ErrorCode::PIN_NOT_AVAILABLE;
			}

			inputState = gpio_get_level((gpio_num_t)id) ? UINT16_MAX : 0;
			return ErrorCode::OK;
		}

		ErrorCode SetU16Output(InOutId id, uint16_t value) override
		{
			if (id == sensact::magic::OUTPUT_NULL)
			{
				return ErrorCode::OK;
			}
			if (id >= GPIO_NUM_MAX)
			{
				return ErrorCode::PIN_NOT_AVAILABLE;
			}
			return gpio_set_level((gpio_num_t)id, value) == ESP_OK ? ErrorCode::OK : ErrorCode::PIN_NOT_AVAILABLE;
		}

		ErrorCode TryReceiveCanMessage(CANMessage &m) override
		{
			if (!can_rx_queue)
			{
				return ErrorCode::FUNCTION_NOT_AVAILABLE;
			}

			CanRxFrame rx = {};
			if (xQueueReceive(can_rx_queue, &rx, 0) != pdTRUE)
			{
				return ErrorCode::NONE_AVAILABLE;
			}
			LOGD(TAG, "Received id %lu", rx.id);
			m.Id = rx.id;
			m.DataLen = rx.len;
			for (size_t i = 0; i < rx.len; i++)
			{
				m.Data[i] = rx.data[i];
			}
			return ErrorCode::OK;
		}
		ErrorCode TrySendCanMessage(CANMessage &m) override
		{
			if (!twai_node)
			{
				return ErrorCode::FUNCTION_NOT_AVAILABLE;
			}

			twai_frame_t tx_frame = {};
			tx_frame.header.id = m.Id;
			tx_frame.header.ide = (m.Id > TWAI_STD_ID_MASK) ? 1 : 0;
			tx_frame.header.dlc = static_cast<uint16_t>(m.DataLen);
			tx_frame.buffer = m.Data;
			tx_frame.buffer_len = m.DataLen;

			if (tx_frame.header.dlc > TWAI_FRAME_MAX_DLC)
			{
				return ErrorCode::DATA_FORMAT_ERROR;
			}

			if (twai_node_transmit(twai_node, &tx_frame, 200) != ESP_OK)
			{
				return ErrorCode::QUEUE_OVERLOAD;
			}
			return ErrorCode::OK;
		}

		ErrorCode GetCanDiagnostics(uint16_t &txErrorCount, uint16_t &rxErrorCount, uint32_t &busErrorCount)
		{
			if (!twai_node)
			{
				txErrorCount = 0;
				rxErrorCount = 0;
				busErrorCount = 0;
				return ErrorCode::FUNCTION_NOT_AVAILABLE;
			}

			twai_node_status_t status = {};
			twai_node_record_t record = {};
			if (twai_node_get_info(twai_node, &status, &record) != ESP_OK)
			{
				return ErrorCode::GENERIC_ERROR;
			}

			txErrorCount = status.tx_error_count;
			rxErrorCount = status.rx_error_count;
			busErrorCount = record.bus_err_num;
			return ErrorCode::OK;
		}

		ErrorCode GetModbusHoldingRegister(uint16_t regIndexZeroBased, uint16_t &value)
		{
			return ErrorCode::OK;
		}
		ErrorCode SetModbusHoldingRegister(uint16_t regIndexZeroBased, uint16_t &value)
		{
			return ErrorCode::OK;
		}
	};
}
#undef TAG