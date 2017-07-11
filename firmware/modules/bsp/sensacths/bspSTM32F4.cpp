#include <common.h>
#include <cBsp.h>
#include <stm32f4xx_hal.h>
#include <cModel.h>
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "BRDSP"
#include <cLog.h>
#include <console.h>
#include <cRCSwitch.h>
#include <cBusmaster.h>


namespace sensact{


void BSP::Init(void) {

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_CAN2_CLK_ENABLE();
	__HAL_RCC_CAN1_CLK_ENABLE();
	__HAL_RCC_CRC_CLK_ENABLE();

	__HAL_RCC_UART4_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();


	CRC_HandleTypeDef   CrcHandle;
	CrcHandle.Instance = CRC;
	//HAL_CRC_Init(&CrcHandle);

	GPIO_InitTypeDef gi;
	HAL_StatusTypeDef status;

	//Enable UART
	/*
	gi.Pin = GPIO_PIN_10 | GPIO_PIN_11; //C10=TX, C11=RX
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOC, &gi);
	*/
	gi.Pin = GPIO_PIN_9 | GPIO_PIN_10; //A09=TX, A10=RX
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &gi);

	InitAndTestUSART();

	if(InitDWTCounter())
	{
		LOGI(BSP::SUCCESSFUL_STRING, "DWTCounter");
	}
	else
	{
		LOGE(NOT_SUCCESSFUL_STRING, "DWTCounter");
	}

	//Onboard LEDs
	gi.Mode = GPIO_MODE_OUTPUT_PP;
	gi.Alternate = 0;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB, &gi);
	LOGI(BSP::SUCCESSFUL_STRING, "GPIO for LED");

	//MP3-Player
	gi.Pin = GPIO_PIN_0 | GPIO_PIN_1; //A0=USART4_TX, A1=USART4_RX, Kerbe nach oben; ansicht von Pinseite, rechts von oben
	//VCC, RX, TX, DACR, DACL, SPK1, GND, SPK2
	//Also: PA0 --> RX
	//PE2 ist Mute
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPIOA, &gi);
	BELL.Instance = UART4;
	BELL.Init.BaudRate = 9600;
	BELL.Init.WordLength = UART_WORDLENGTH_8B;
	BELL.Init.StopBits = UART_STOPBITS_1;
	BELL.Init.Parity = UART_PARITY_NONE;
	BELL.Init.Mode = UART_MODE_TX_RX;
	BELL.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	BELL.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&BSP::BELL);
	LOGI(SUCCESSFUL_STRING, "UART4 for MP3-Module");

	//For Standby
	gi.Pin = GPIO_PIN_2;
	gi.Mode = GPIO_MODE_AF_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = 0;
	HAL_GPIO_Init(GPIOE, &gi);

	/*
	PB08     ------> I2C1_SCL
	PB09     ------> I2C1_SDA
	PB10     ------> I2C2_SCL
	PB11     ------> I2C2_SDA
	*/
	__I2C1_CLK_ENABLE();
	__I2C2_CLK_ENABLE();
	gi.Pin = GPIO_PIN_8 | GPIO_PIN_9|GPIO_PIN_10 | GPIO_PIN_11;
	gi.Mode = GPIO_MODE_AF_OD;
	gi.Pull = GPIO_PULLUP;
	gi.Speed = GPIO_SPEED_MEDIUM;
	gi.Alternate = GPIO_AF4_I2C1;
	gi.Alternate = GPIO_AF4_I2C2;
	HAL_GPIO_Init(GPIOB, &gi);

	BSP::i2c1.Instance = I2C1;
	BSP::i2c1.Init.ClockSpeed = 100000;
	BSP::i2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	BSP::i2c1.Init.OwnAddress1 = 0;
	BSP::i2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	BSP::i2c1.Init.OwnAddress2 = 0;
	BSP::i2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	BSP::i2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	status=HAL_I2C_Init(&BSP::i2c1);
	if(status==HAL_OK)
	{
		LOGI(BSP::SUCCESSFUL_STRING, "I2C2 for onboard digital io");
	}
	else
	{
		LOGE(BSP::NOT_SUCCESSFUL_STRING, "I2C2 for onboard digital io");
	}
	BSP::i2c2.Instance = I2C2;
	BSP::i2c2.Init.ClockSpeed = 100000;
	BSP::i2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	BSP::i2c2.Init.OwnAddress1 = 0;
	BSP::i2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	BSP::i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
	BSP::i2c2.Init.OwnAddress2 = 0;
	BSP::i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	BSP::i2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	status=HAL_I2C_Init(&BSP::i2c2);
	if(status==HAL_OK)
	{
		LOGI(BSP::SUCCESSFUL_STRING, "I2C2 for 1wire and external");
	}
	else
	{
		LOGE(BSP::NOT_SUCCESSFUL_STRING, "I2C2 for 1wire and external");
	}

	/**CAN2 GPIO Configuration
	 PB12     ------> CAN2_RX
	 PB13     ------> CAN2_TX
	 */
	gi.Pin = CAN_PINS;
	gi.Mode = GPIO_MODE_AF_PP;
	gi.Pull = GPIO_NOPULL;
	gi.Speed = GPIO_SPEED_LOW;
	gi.Alternate = GPIO_AF9_CAN1; //GPIO_AF9_CAN2 has the same value!
	HAL_GPIO_Init(CAN_PORT, &gi);
	InitCAN();
#if defined(SENSACTHS08) && defined(ETHERNET)
	    spi2.Instance = SPI2;
	    spi2.Init.Mode = SPI_MODE_MASTER;
	    spi2.Init.Direction = SPI_DIRECTION_2LINES;
	    spi2.Init.DataSize = SPI_DATASIZE_8BIT;
	    spi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	    spi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	    spi2.Init.NSS = SPI_NSS_SOFT;
	    spi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	    spi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	    spi2.Init.TIMode = SPI_TIMODE_DISABLED;
	    spi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;

	    __SPI2_CLK_ENABLE();
	    /**SPI2 GPIO Configuration
	    PB13     ------> SPI2_SCK
	    PB14     ------> SPI2_MISO
	    PB15     ------> SPI2_MOSI
	    */
	    gi.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	    gi.Mode = GPIO_MODE_AF_PP;
	    gi.Pull = GPIO_NOPULL;
	    gi.Speed = GPIO_SPEED_HIGH;
	    gi.Alternate = GPIO_AF5_SPI2;
	    HAL_GPIO_Init(GPIOB, &gi);

	    /*Configure GPIO pin : PB12 as chip select */
	    gi.Pin = GPIO_PIN_12;
	    gi.Mode = GPIO_MODE_OUTPUT_PP;
	    gi.Pull = GPIO_PULLUP;
	    gi.Speed = GPIO_SPEED_LOW;
	    HAL_GPIO_Init(GPIOB, &gi);

		HAL_SPI_Init(&spi2);

		uint8_t retVal, sockStatus;
		int16_t rcvLen;
		uint8_t rcvBuf[20], bufSize[] = {2, 2, 2, 2};

		PRINT_HEADER();

		reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
		reg_wizchip_spi_cbfunc(spi_rb, spi_wb);

		wizchip_init(bufSize, bufSize);
		wiz_NetInfo netInfo = { .mac 	= {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},	// Mac address
		                          .ip 	= {192, 168, 1, 192},					// IP address
		                          .sn 	= {255, 255, 255, 0},					// Subnet mask
		                          .gw 	= {192, 168, 1, 1}};					// Gateway address
		wizchip_setnetinfo(&netInfo);
		wizchip_getnetinfo(&netInfo);
		  PRINT_NETINFO(netInfo);
		  reconnect:
		    /* Open socket 0 as TCP_SOCKET with port 5000 */
		    if((retVal = socket(0, Sn_MR_TCP, 5000, 0)) == 0) {
		  	  /* Put socket in LISTEN mode. This means we are creating a TCP server */
		  	  if((retVal = listen(0)) == SOCK_OK) {
		  		  /* While socket is in LISTEN mode we wait for a remote connection */
		  		  while(sockStatus = getSn_SR(0) == SOCK_LISTEN)
		  			  HAL_Delay(100);
		  		  /* OK. Got a remote peer. Let's send a message to it */
		  		  while(1) {
		  			  /* If connection is ESTABLISHED with remote peer */
		  			  if(sockStatus = getSn_SR(0) == SOCK_ESTABLISHED) {
		  				  uint8_t remoteIP[4];
		  				  uint16_t remotePort;
		  				  /* Retrieving remote peer IP and port number */
		  				  getsockopt(0, SO_DESTIP, remoteIP);
		  				  getsockopt(0, SO_DESTPORT, (uint8_t*)&remotePort);
		  				  sprintf(msg, CONN_ESTABLISHED_MSG, remoteIP[0], remoteIP[1], remoteIP[2], remoteIP[3], remotePort);
		  				  PRINT_STR(msg);
		  				  /* Let's send a welcome message and closing socket */
		  				  if(retVal = send(0, (uint8_t *)GREETING_MSG, strlen(GREETING_MSG)) == (int16_t)strlen(GREETING_MSG))
		  					  PRINT_STR(SENT_MESSAGE_MSG);
		  				  else { /* Ops: something went wrong during data transfer */
		  					  sprintf(msg, WRONG_RETVAL_MSG, retVal);
		  					  PRINT_STR(msg);
		  				  }
		  				  break;
		  			  }
		  			  else { /* Something went wrong with remote peer, maybe the connection was closed unexpectedly */
		  				  sprintf(msg, WRONG_STATUS_MSG, sockStatus);
		  				  PRINT_STR(msg);
		  				  break;
		  			  }
		  		  }

		  	  } else /* Ops: socket not in LISTEN mode. Something went wrong */
		  		  PRINT_STR(LISTEN_ERR_MSG);
		    } else { /* Can't open the socket. This means something is wrong with W5100 configuration: maybe SPI issue? */
		  	  sprintf(msg, WRONG_RETVAL_MSG, retVal);
		  	  PRINT_STR(msg);
		    }

		    /* We close the socket and start a connection again */
		    disconnect(0);
		    close(0);
		    goto reconnect;

		    /* Infinite loop */
		    while (1)
		    {
		    }

#endif


	for(uint8_t i = 0; i<BSP::busCnt;i++)
	{
		MODEL::busses[i]->Init();
	}


	return;
}



void BSP::DoEachCycle(Time_t now) {
	for(uint8_t i = 0; i<BSP::busCnt;i++)
	{
		MODEL::busses[i]->Process(now);
	}
	//StartConversion und 1 sek später reihum einsammeln
	if (now > nextLedToggle) {
		for(uint8_t i=0;i<COUNTOF(BSP::ErrorCounters);i++)
		{
			if(BSP::ErrorCounters[i]!=0)
			{
				LOGW("ErrorCounters[%i] = %i", i, BSP::ErrorCounters[i]);
			}
		}
		nextLedToggle += 1000;
	}


}
}
