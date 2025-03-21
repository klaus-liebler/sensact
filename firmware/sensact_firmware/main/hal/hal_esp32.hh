#pragma once
#include "hal.hh"
#include <sensact_logger.hh>
#include <driver/gpio.h>
#include <driver/twai.h>
#include <driver/uart.h>
#include <i2c.hh>
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
	class cESP32 : public iHAL
	{

	protected:
		void *modbus_master_handler{nullptr};
		temperature_sensor_handle_t temp_handle{nullptr};
		void SetupCAN(gpio_num_t tx, gpio_num_t rx, int intr_alloc_flags=0)
		{
			twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(tx, rx, TWAI_MODE_NORMAL);
			g_config.alerts_enabled = TWAI_ALERT_ABOVE_ERR_WARN | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_OFF;
			g_config.intr_flags = intr_alloc_flags;
			g_config.tx_queue_len=16;
			twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
			twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
			ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
			ESP_ERROR_CHECK(twai_start());
		}

	

		void SetupLedcCommonTimer(ledc_timer_t timer){
			
			// Prepare and then apply the LEDC PWM timer configuration
			ledc_timer_config_t ledc_timer = {};
			ledc_timer.speed_mode= LEDC_LOW_SPEED_MODE;//obligatory according to https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32s3/api-reference/peripherals/ledc.html
			ledc_timer.duty_resolution  = LEDC_TIMER_14_BIT;
			ledc_timer.timer_num        = timer;
			ledc_timer.freq_hz          = 1000;
			ledc_timer.clk_cfg          = LEDC_AUTO_CLK;
			ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
		}
		void SetupLedcChannel(ledc_timer_t timer, ledc_channel_t channel, gpio_num_t gpio, uint32_t initialDuty=0){

			// Prepare and then apply the LEDC PWM channel configuration
			ledc_channel_config_t ledc_channel = {};
			ledc_channel.speed_mode     = LEDC_LOW_SPEED_MODE;
			ledc_channel.channel        = channel;
			ledc_channel.timer_sel      = timer;
			ledc_channel.intr_type      = LEDC_INTR_DISABLE;
			ledc_channel.gpio_num       = (int)gpio;
			ledc_channel.duty           = initialDuty;
			ledc_channel.hpoint         = 0;
			ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
		}

		void SetDuty(ledc_channel_t channel, uint16_t value_0_65635){
			ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, value_0_65635>>2);//>>2 because of 14 bit Resolution
			ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
		}
		
		
 		
		void SetupInternalTemperatureSensor_deprecated_Do_IT_LIKE_SENSACT_UP_CONTROL()
		{
			temperature_sensor_config_t temp_sensor_config={};
			temp_sensor_config.range_min=-10;
			temp_sensor_config.range_max=+80;
			temp_sensor_config.clk_src=TEMPERATURE_SENSOR_CLK_SRC_DEFAULT;
			temp_sensor_config.flags.allow_pd=0;
			ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &this->temp_handle));
			ESP_ERROR_CHECK(temperature_sensor_enable(this->temp_handle));
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

			return gpio_get_level((gpio_num_t)id) == ESP_OK ? ErrorCode::OK : ErrorCode::GENERIC_ERROR;
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
			twai_message_t rx_msg;
			if (twai_receive(&rx_msg, 0) != ESP_OK)
			{
				return ErrorCode::NONE_AVAILABLE;
			}
			LOGD(TAG, "Received id %lu", rx_msg.identifier);
			m.Id = rx_msg.identifier;
			m.DataLen = rx_msg.data_length_code;
			for (int i = 0; i < rx_msg.data_length_code; i++)
			{
				m.Data[i] = rx_msg.data[i];
			}
			return ErrorCode::OK;
		}
		ErrorCode TrySendCanMessage(CANMessage &m) override
		{
			twai_message_t tx_msg;
			tx_msg.identifier = m.Id;
			tx_msg.data_length_code = m.DataLen;
			for (int i = 0; i < tx_msg.data_length_code; i++)
			{
				tx_msg.data[i] = m.Data[i];
			}
			if (twai_transmit(&tx_msg, pdMS_TO_TICKS(200)) != ESP_OK)
			{
				return ErrorCode::QUEUE_OVERLOAD;
			}
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