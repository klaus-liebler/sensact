#pragma once
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <esp_types.h>
#include <esp_attr.h>
#include <esp_intr_alloc.h>
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_pm.h"
#include "soc/soc_memory_layout.h"
extern "C" {
	#include "hal/i2c_hal.h"
}
#include "soc/i2c_periph.h"
#include "driver/periph_ctrl.h"
#include "esp_log.h"

#include <array>

#define TAG "I2C_MEM"


constexpr size_t I2C_FIFO_FULL_THRESH_VAL_DATA{16};        /* Threshold value while waiting for data */
constexpr size_t I2C_FIFO_FULL_THRESH_VAL_ADDR{1};        /* Threshold value while waiting for address */
constexpr size_t I2C_FIFO_EMPTY_THRESH_VAL{2};
constexpr size_t I2C_FIFO_BYTES_PER_TX{4};

constexpr uint32_t I2C_SLAVE_TIMEOUT_DEFAULT{32000}; /* I2C slave timeout value, APB clock cycle number */
constexpr uint32_t I2C_SLAVE_SDA_SAMPLE_DEFAULT{10}; /* I2C slave sample time after scl positive edge default value */
constexpr uint32_t I2C_SLAVE_SDA_HOLD_DEFAULT{10};	  /* I2C slave hold time after scl negative edge default value */
constexpr uint32_t I2C_MASTER_TOUT_CNUM_DEFAULT{8};  /* I2C master timeout cycle number of I2C clock, after which the timeout interrupt will be triggered */
constexpr uint32_t I2C_ACKERR_CNT_MAX{10};
constexpr uint32_t I2C_FILTER_CYC_NUM_DEF{7}; /* The number of apb cycles filtered by default*/
constexpr uint32_t I2C_CLR_BUS_SCL_NUM{9};
constexpr uint32_t I2C_CLR_BUS_HALF_PERIOD_US{5};
constexpr size_t I2C_MEM_SIZE{UINT8_MAX};

extern "C" void i2c_isr_handler_mem(void *arg);

class I2C_MemoryEmulationHandler{
	public:
	virtual void HandleWriteTransactionFromISR(std::array<uint8_t, I2C_MEM_SIZE> *mem, uint8_t startMem, uint8_t len)=0;
	virtual void ProvideDataForReadTransactionFromISR(uint8_t idx, uint8_t *data)=0;
};

class I2C_MemoryEmulation
{
public:
	I2C_MemoryEmulation(const int I2C_NUM_0or1, I2C_MemoryEmulationHandler* const handler):handler(handler)
	{
		i2c_num = I2C_NUM_0or1,
		hal.dev = I2C_LL_GET_HW(I2C_NUM_0or1);
		spinlock.count = 0;
		spinlock.owner = SPINLOCK_FREE;
	}

	esp_err_t Get(uint8_t addr, uint8_t *value){
		*value=mem[addr];
		return ESP_OK;
	}

	esp_err_t Set(uint8_t addr, uint8_t value){
		mem[addr]=value;
		return ESP_OK;
	}

	esp_err_t Setup(const gpio_num_t scl_io_num, const gpio_num_t sda_io_num, const uint16_t slaveAddr, bool addr_10bit_en, int intr_alloc_flags)
	{

		int sda_out_sig = i2c_periph_signal[i2c_num].sda_out_sig;
		int sda_in_sig = i2c_periph_signal[i2c_num].sda_in_sig;
		int scl_out_sig = i2c_periph_signal[i2c_num].scl_out_sig;
		int scl_in_sig = i2c_periph_signal[i2c_num].scl_in_sig;

		gpio_set_level(sda_io_num, 1);
		PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[sda_io_num], PIN_FUNC_GPIO);
		gpio_set_direction(sda_io_num, GPIO_MODE_INPUT_OUTPUT_OD);
		gpio_set_pull_mode(sda_io_num, GPIO_FLOATING);
       	gpio_matrix_out(sda_io_num, sda_out_sig, 0, 0);
        gpio_matrix_in(sda_io_num, sda_in_sig, 0);

		gpio_set_level(scl_io_num, 1);
		PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[scl_io_num], PIN_FUNC_GPIO);
		gpio_set_direction(scl_io_num, GPIO_MODE_INPUT_OUTPUT_OD);
        gpio_matrix_out(scl_io_num, scl_out_sig, 0, 0);
        gpio_matrix_in(scl_io_num, scl_in_sig, 0);
		gpio_set_pull_mode(scl_io_num, GPIO_FLOATING);

		portENTER_CRITICAL(&spinlock);
		periph_module_enable(i2c_periph_signal[i2c_num].module);
		i2c_hal_disable_intr_mask(&hal, I2C_LL_INTR_MASK);
		i2c_hal_clr_intsts_mask(&hal, I2C_LL_INTR_MASK);
		i2c_hal_slave_init(&hal, i2c_num);
		i2c_hal_set_slave_addr(&hal, slaveAddr, addr_10bit_en);
		i2c_hal_set_rxfifo_full_thr(&hal, I2C_FIFO_FULL_THRESH_VAL_ADDR);
		i2c_hal_set_txfifo_empty_thr(&hal, I2C_FIFO_EMPTY_THRESH_VAL);
		//set timing for data
		i2c_hal_set_sda_timing(&hal, I2C_SLAVE_SDA_SAMPLE_DEFAULT, I2C_SLAVE_SDA_HOLD_DEFAULT);
		i2c_hal_set_tout(&hal, I2C_SLAVE_TIMEOUT_DEFAULT);
    	//hook isr handler
		esp_intr_alloc(i2c_periph_signal[i2c_num].irq, intr_alloc_flags, i2c_isr_handler_mem, this, &this->intr_handle);
        i2c_hal_enable_slave_rx_it(&this->hal);
		//i2c_hal_update_config(&hal);
		portEXIT_CRITICAL(&spinlock);
		return ESP_OK;
	}

	void LogStatus(){
		ESP_LOGI(TAG, "Status = {statError:%d, lastMemAddr:%d, statTransCompleteCnt:%d, statRxFifoFullCnt:%d, statTxFifoEmptyCnt:%d}", statError, memAddr,  statTransCompleteCnt, statRxFifoFullCnt, statTxFifoEmptyCnt);
	}

	void isr_handler()
	{
		portBASE_TYPE HPTaskAwoken = pdFALSE;
        if (hal.dev->int_status.trans_complete){
			this->statTransCompleteCnt++;
			doReceive();
			doFinish();
			HPTaskAwoken=pdTRUE;
			i2c_hal_clr_intsts_mask(&(this->hal), I2C_TRANS_COMPLETE_INT_CLR_M);
		}else if(hal.dev->int_status.rx_fifo_full) {
			this->statRxFifoFullCnt++;
			doReceive();
            i2c_hal_clr_intsts_mask(&(this->hal), I2C_RXFIFO_FULL_INT_CLR_M);
        } else if (hal.dev->int_status.tx_fifo_empty) {
			this->statTxFifoEmptyCnt++;
			doProvide();
			i2c_hal_clr_intsts_mask(&(this->hal), I2C_TXFIFO_EMPTY_INT_CLR_M);
		}
		//We only need to check here if there is a high-priority task needs to be switched.
		if (HPTaskAwoken == pdTRUE){portYIELD_FROM_ISR();}
	}

private:
	intr_handle_t intr_handle; /*!< I2C interrupt handle*/
	std::array<uint8_t, I2C_MEM_SIZE> mem{0};

	i2c_port_t i2c_num;
	i2c_hal_context_t hal;
	portMUX_TYPE spinlock;
	I2C_MemoryEmulationHandler* const handler;
	uint32_t statTxFifoEmptyCnt=0;
	uint32_t statRxFifoFullCnt=0;
	uint32_t statTransCompleteCnt=0;
	uint8_t statError=0;

	void IRAM_ATTR doReceive()
	{
		// get number of bytes in rx fifo
		uint32_t rx_fifo_cnt;
		i2c_hal_get_rxfifo_cnt(&(this->hal), &rx_fifo_cnt);
		if(nextRxByteIsAddress && rx_fifo_cnt>0){ //first byte in an RX situation is always a new address
			//wenn der Request schnell bearbeitet wird, sollte rx_fifo_cnt immer nur 1 byte groß sein. Wenn aber die Bearbeitung länger dauert, könnten schon mehr bytes angekommen sein. Deshalb darf nicht zwingend davon ausgegangen werden, dass nur ein byte im FIFO liegt...
			i2c_hal_read_rxfifo(&hal, &memAddr, 1);
			rxoffset=0;
			txoffset=0;
			rx_fifo_cnt--;
			
			/*Prepare for forthcoming TX request*/
			i2c_hal_txfifo_rst(&hal); // empty tx fifo
			doProvide();//write some bytes into TX FIFO
			i2c_hal_clr_intsts_mask(&(this->hal), I2C_TXFIFO_EMPTY_INT_CLR_M);//clear IRQ
			i2c_hal_enable_slave_tx_it(&this->hal);// enable tx fifo tx fifo empty interrupt,
			
			/*Prepare for forthcoming RX request*/
			i2c_hal_set_rxfifo_full_thr(&hal, I2C_FIFO_FULL_THRESH_VAL_DATA);// use larger rx fifo threshold for data bytes
			nextRxByteIsAddress=false;
		}
		while(rx_fifo_cnt>0) //runs in a loop byte by byte to avoind problems with circular writes (254, 255, 0, 1)
		{
			i2c_hal_read_rxfifo(&(this->hal),&(this->mem[memAddr+rxoffset]), 1);
			rxoffset++;
			rx_fifo_cnt--;
		}
	}

	void IRAM_ATTR doFinish()
	{
		if(this->handler && rxoffset>0){//wenn der Handler existiert und wirklich was geschrieben wurde
			this->handler->HandleWriteTransactionFromISR(&this->mem, memAddr, rxoffset);
		}
		// Set fifo full threshold to 1, so that the next address can be detected
		i2c_hal_set_rxfifo_full_thr(&hal, I2C_FIFO_FULL_THRESH_VAL_ADDR);
		// next rx transaction will fetch address first
		nextRxByteIsAddress = true;
	}

	void IRAM_ATTR doProvide()
	{
		for(int i=0;i<I2C_FIFO_BYTES_PER_TX;i++){
			uint8_t myAddr = memAddr + txoffset+i;
			if(this->handler){
				this->handler->ProvideDataForReadTransactionFromISR(myAddr, &mem[myAddr]);
			}
			i2c_hal_write_txfifo(&(this->hal), &mem[myAddr], 1);
		}
		txoffset+=I2C_FIFO_BYTES_PER_TX;
	}

	uint8_t memAddr = 0;
	uint8_t rxoffset = 0; //benötige rx und tx offset, weil beim Anfang jeder Transaktion ein txFifoEmpty-IRQ ausgelöst wird
	uint8_t txoffset =0;
	bool nextRxByteIsAddress = true;
};

extern "C" void IRAM_ATTR i2c_isr_handler_mem(void *arg)
{
	I2C_MemoryEmulation *theObjectToCall = (I2C_MemoryEmulation *)arg;
	theObjectToCall->isr_handler();
}

#undef TAG