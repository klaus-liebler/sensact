#pragma once
#include "hal.hh"
#include <sensact_logger.hh>
#include <driver/gpio.h>
#include <driver/twai.h>
#include <driver/uart.h>
#include "modbusmaster.hh"
#include <i2c.hh>
#include <vector>
#include <wifimanager.hh>

#define TAG "HAL"

#define MASTER_CHECK(a, ret_val, str, ...) \
    if (!(a)) { \
        ESP_LOGE(TAG, "%s(%u): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

using namespace sensact::hal;
using namespace sensact;
namespace sensact::hal
{
	class I2CBus:public iI2CBus{
		private:
		i2c_port_t i2c_port;
		public:
		I2CBus(i2c_port_t i2c_port):i2c_port(i2c_port){

		}
		ErrorCode ReadReg(uint8_t address7bit, uint8_t reg_addr, uint8_t *reg_data, size_t len)override{
			return I2C::ReadReg(this->i2c_port, address7bit, reg_addr, reg_data, len)==ESP_OK?ErrorCode::OK:ErrorCode::DEVICE_NOT_RESPONDING;
		}
    	ErrorCode ReadReg16(uint8_t address7bit, uint16_t reg_addr16, uint8_t *reg_data, size_t len)override{
			return I2C::ReadReg16(this->i2c_port, address7bit, reg_addr16, reg_data, len)==ESP_OK?ErrorCode::OK:ErrorCode::DEVICE_NOT_RESPONDING;
		}
    	ErrorCode Read(uint8_t address7bit, uint8_t *data, size_t len)override{
			return I2C::Read(this->i2c_port, address7bit, data, len)==ESP_OK?ErrorCode::OK:ErrorCode::DEVICE_NOT_RESPONDING;
		}
    	ErrorCode WriteReg(uint8_t address7bit, uint8_t reg_addr, uint8_t *reg_data, size_t len)override{
			return I2C::WriteReg(this->i2c_port, address7bit, reg_addr, reg_data, len)==ESP_OK?ErrorCode::OK:ErrorCode::DEVICE_NOT_RESPONDING;
		}
    	ErrorCode WriteSingleReg(uint8_t address7bit, uint8_t reg_addr, uint8_t reg_data){
			return I2C::WriteSingleReg(this->i2c_port, address7bit, reg_addr, reg_data)==ESP_OK?ErrorCode::OK:ErrorCode::DEVICE_NOT_RESPONDING;
		}
    	ErrorCode Write(uint8_t address7bit, uint8_t *data, size_t len)override{
			return I2C::Write(this->i2c_port, address7bit, data, len)==ESP_OK?ErrorCode::OK:ErrorCode::DEVICE_NOT_RESPONDING;
		}
    	ErrorCode IsAvailable(uint8_t address7bit)override{
			return I2C::IsAvailable(this->i2c_port, address7bit)==ESP_OK?ErrorCode::OK:ErrorCode::DEVICE_NOT_RESPONDING;
		}
	};
	
	
	class cESP32 : public iHAL
	{
	
	protected:
		void* modbus_master_handler = NULL;
		ErrorCode SetupCAN(gpio_num_t tx, gpio_num_t rx)
		{
			twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(tx, rx, TWAI_MODE_NORMAL);
			twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
			twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
			ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
			return ErrorCode::OK;
		}

		ErrorCode SetupNetworkCommon(){
			//TODO initNetIfandEventLoop();
			return ErrorCode::OK;
		}

		ErrorCode SetupWIFI(){
			//TODO connect(CONFIG_NETWORK_WIFI_SSID, CONFIG_NETWORK_WIFI_PASSWORD, "sensact-%02X-%02X-%02X");
			return ErrorCode::OK;
		}

		ErrorCode SetupETH(gpio_num_t mdc, gpio_num_t mdio, gpio_num_t reset){
			//TODO W5500!!! initEth(mdc, mdio, reset);
			return ErrorCode::OK;
		}

		ErrorCode SetupModbusMaster(uart_port_t uartPort, int baudrate, gpio_num_t tx, gpio_num_t rx, gpio_num_t rts){
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
		tms_t GetMillisS64() override{
			return esp_timer_get_time()/1000;
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
			return gpio_set_level((gpio_num_t )id, value) == ESP_OK ? ErrorCode::OK : ErrorCode::PIN_NOT_AVAILABLE;
		}





		ErrorCode TryReceiveCANMessage(CANMessage &m) override
		{
			twai_message_t rx_msg;
			if (twai_receive(&rx_msg, 0) != ESP_OK)
			{
				return ErrorCode::NONE_AVAILABLE;
			}
			LOGI(TAG, "Received id %lu", rx_msg.identifier);
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
			if (twai_transmit(&tx_msg, 1) != ESP_OK)
			{
				return ErrorCode::QUEUE_OVERLOAD;
			}
			LOGI(TAG, "Enqueued id %lu", tx_msg.identifier);
			return ErrorCode::OK;
		}

		ErrorCode GetModbusHoldingRegister(uint16_t regIndexZeroBased, uint16_t &value){
			return ErrorCode::OK;
		}
		ErrorCode SetModbusHoldingRegister(uint16_t regIndexZeroBased, uint16_t &value){
			return ErrorCode::OK;
		}
	};
}
#undef TAG