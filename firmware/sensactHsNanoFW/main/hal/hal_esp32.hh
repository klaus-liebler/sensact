#pragma once
#include "hal.hh"
#include <sensact_logger.hh>
#include <driver/gpio.h>
#include <driver/twai.h>
#include <driver/uart.h>
#include "modbusmaster.hh"
#include <i2c.hh>
#include <vector>
#include <driver/temperature_sensor.h>
#include <webmanager.hh>

#define TAG "HAL"

using namespace sensact::hal;
using namespace sensact;
namespace sensact::hal
{
	class MyI2CPort : public iI2CPort
	{
	private:
		i2c_port_t i2c_port;

	public:
		MyI2CPort(i2c_port_t i2c_port) : i2c_port(i2c_port)
		{
		}
		ErrorCode ReadReg(uint8_t address7bit, uint8_t reg_addr, uint8_t *reg_data, size_t len) override
		{
			return I2C::ReadReg(this->i2c_port, address7bit, reg_addr, reg_data, len) == ESP_OK ? ErrorCode::OK : ErrorCode::DEVICE_NOT_RESPONDING;
		}
		ErrorCode ReadReg16(uint8_t address7bit, uint16_t reg_addr16, uint8_t *reg_data, size_t len) override
		{
			return I2C::ReadReg16(this->i2c_port, address7bit, reg_addr16, reg_data, len) == ESP_OK ? ErrorCode::OK : ErrorCode::DEVICE_NOT_RESPONDING;
		}
		ErrorCode Read(uint8_t address7bit, uint8_t *data, size_t len) override
		{
			return I2C::Read(this->i2c_port, address7bit, data, len) == ESP_OK ? ErrorCode::OK : ErrorCode::DEVICE_NOT_RESPONDING;
		}
		ErrorCode WriteReg(uint8_t address7bit, uint8_t reg_addr, const uint8_t *reg_data, size_t len) override
		{
			return I2C::WriteReg(this->i2c_port, address7bit, reg_addr, reg_data, len) == ESP_OK ? ErrorCode::OK : ErrorCode::DEVICE_NOT_RESPONDING;
		}
		ErrorCode WriteSingleReg(uint8_t address7bit, uint8_t reg_addr, uint8_t reg_data)
		{
			return I2C::WriteSingleReg(this->i2c_port, address7bit, reg_addr, reg_data) == ESP_OK ? ErrorCode::OK : ErrorCode::DEVICE_NOT_RESPONDING;
		}
		ErrorCode Write(uint8_t address7bit, const uint8_t *data, size_t len) override
		{
			return I2C::Write(this->i2c_port, address7bit, data, len) == ESP_OK ? ErrorCode::OK : ErrorCode::DEVICE_NOT_RESPONDING;
		}
		ErrorCode IsAvailable(uint8_t address7bit) override
		{
			return I2C::IsAvailable(this->i2c_port, address7bit) == ESP_OK ? ErrorCode::OK : ErrorCode::DEVICE_NOT_RESPONDING;
		}
		ErrorCode Discover() override
		{
			return I2C::Discover(this->i2c_port) == ESP_OK ? ErrorCode::OK : ErrorCode::DEVICE_NOT_RESPONDING;
		}
	};

	class cESP32 : public iHAL
	{

	protected:
		void *modbus_master_handler = NULL;
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

 		
		void SetupInternalTemperatureSensor()
		{
			temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
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
            return temperature_sensor_get_celsius(this->temp_handle, &temperatureCelcius)==ESP_OK?ErrorCode::OK:ErrorCode::GENERIC_ERROR;
        }

		
		ErrorCode CheckAndLogHealth() override{

			bool atLeastHealthWarning{false};
			bool healthError{false};
				
			webmanager::M* wm = webmanager::M::GetSingleton();

			float temperatureCelcius;
			GetBoardTemperature(temperatureCelcius);
			if(temperatureCelcius>60){
				wm->Log(messagecodes::C::BOARD_HOT, temperatureCelcius);
				healthError=true;				
			}
			else if(temperatureCelcius>50){
				wm->Log(messagecodes::C::BOARD_WARM, temperatureCelcius);
				atLeastHealthWarning=true;
			}
			twai_status_info_t status_info={};
			twai_get_status_info(&status_info);
			if(status_info.tx_error_counter>0){
				wm->Log(messagecodes::C::CAN_TX_ERROR_COUNTER, status_info.tx_error_counter);
				atLeastHealthWarning=true;
			}
			if(status_info.rx_error_counter>0){
				wm->Log(messagecodes::C::CAN_RX_ERROR_COUNTER, status_info.rx_error_counter);
				atLeastHealthWarning=true;
			}
			if(status_info.tx_failed_count>0){
				wm->Log(messagecodes::C::CAN_TX_FAILED_COUNTER, status_info.tx_failed_count);
				healthError=true;
			}
			if(status_info.rx_missed_count>0){
				wm->Log(messagecodes::C::CAN_RX_MISSED_COUNTER, status_info.rx_missed_count);
				healthError=true;
			}
			if(status_info.rx_overrun_count>0){
				wm->Log(messagecodes::C::CAN_RX_OVERRUN_COUNTER, status_info.rx_overrun_count);
				healthError=true;
			}
			if(status_info.arb_lost_count>0){
				wm->Log(messagecodes::C::CAN_ARBITRATION_LOST, status_info.arb_lost_count);
				healthError=true;
			}
			if(status_info.bus_error_count>0){
				wm->Log(messagecodes::C::CAN_BUS_ERROR, status_info.bus_error_count);
				healthError=true;
			}

			return healthError?ErrorCode::HEALTH_ERROR:atLeastHealthWarning?ErrorCode::HEALTH_WARNING:ErrorCode::OK;
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

		ErrorCode TryReceiveCANMessage(CANMessage &m) override
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