#include "common.h"
#include "w5100.h"
#define LOGLEVEL LEVEL_INFO
#define LOGNAME "W5100"
#include <cLog.h>
namespace drivers {

enum struct eW5100Register : uint8_t
{
	MR= 0x0000,
	GAR0=	0x0001, 	//GATEWAY
	GAR1=	0x0002,
	GAR2=	0x0003,
	GAR3=	0x0004,

	SUBR0=	0x0005, 	//NETMASK
	SUBR1=	0x0006,
	SUBR2=	0x0007,
	SUBR3=	0x0008,

	SHAR0=	0x0009, 	//MAC ADDRESS
	SHAR1=	0x000A,
	SHAR2=	0x000B,
	SHAR3=	0x000C,
	SHAR4=	0x000D,
	SHAR5=	0x000E,

	SIPR0=	0x000F, 	//SOURCE IP ADDRESS
	SIPR1=	0x0010,
	SIPR2=	0x0011,
	SIPR3=	0x0012,

	IR = 0x0015, //INTERRUPT
	IMR = 0x001, //Interrupt Mask

	RMSR = 0x001A, // RX memory size
	TMSR = 0x001B, //TX memory size
};


#define SOCK_MAX_NUM 4
#define TX_RX_MAX_BUF_SIZE	2048
#define TX_BUF	0x1100
#define RX_BUF	(TX_BUF+TX_RX_MAX_BUF_SIZE)

#define SOCKn_REG_SIZE	(uint16_t)0x0100
#define SOCK0_BASE_ADDR	(uint16_t)0x0400
#define SOCK1_BASE_ADDR	(uint16_t)0x0500
#define SOCK2_BASE_ADDR	(uint16_t)0x0600
#define SOCK3_BASE_ADDR	(uint16_t)0x0700


#define Sn_MR(s)	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0000) //Sn_MR register - Mode register
#define Sn_CR(s)	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0001) //Sn_CR register - Command register
#define Sn_IR(s)	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0002) //Sn_IR register - Socket interrupt register
#define Sn_SR(s)	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0003) //Sn_SR register - Socket status register
#define Sn_PORT0(s)  (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0004) //Sn_PORT0 register - Source port register
#define Sn_PORT1(s)  (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0005) //Sn_PORT1 register - Source port register

#define Sn_DHAR0(s) 	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0006) //DEST MAC ADDR
#define Sn_DHAR1(s) 	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0007)
#define Sn_DHAR2(s) 	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0008)
#define Sn_DHAR3(s) 	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0009)
#define Sn_DHAR4(s) 	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x000A)
#define Sn_DHAR5(s) 	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x000B)
#define Sn_DIPR0(s)	 (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x000C) //Sn_DIPR0 register - Destination IP register
#define Sn_DPORT0(s) (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0010) //Sn_DPORT0 register - Destination Port register
#define Sn_TX_FSR0(s)(uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0020) //Sn_TX_FSR0 register - Socket n TX Free Size
#define Sn_TX_RD0(s) (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0022) //Sn_TX_RD0 register - Socket n TX Read pointer
#define Sn_TX_WR0(s) (uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0024) //Sn_TX_WR0 register - Socket n TX Write pointer
#define Sn_RX_RSR0(s)(uint16_t)(SOCK0_BASE_ADDR + ((uint8_t)(s)*SOCKn_REG_SIZE) + 0x0026) //Sn_RX_RSR0 register - Socket n Received size
#define Sn_RX_RD0(s) (uint16_t)(SOCK0_BASE_ADDR + (((uint8_t)(s))*SOCKn_REG_SIZE) + 0x0028) //Sn_RX_RD0 register - Socket n RX Read pointer

#define Sn_IR_CONNECTED 0x01
#define Sn_IR_DISCONNECTED 0x02
#define Sn_IR_DATA_RECV 0x04
#define Sn_IR_TIMEOUT 0x08
#define Sn_IR_SEND_OK 0x10

#define SOCK_CONNECTED(im) 		((im & Sn_IR_CONNECTED))
#define SOCK_DISCONNECTED(im) 	((im & Sn_IR_DISCONNECTED))
#define SOCK_DATA_RECV(im) 		((im & Sn_IR_DATA_RECV))
#define SOCK_TIMEOUT(im) 		((im & Sn_IR_TIMEOUT))
#define SOCK_DATA_SENT(im) 		((im & Sn_IR_SEND_OK))



enum struct _eSockConnMode:uint8_t {
	SOCK_MODE_CLIENT	= 0x0,
	SOCK_MODE_SERVER	= 0x1,
};



#define GWIP_ADDR_REG 	GAR0
#define GWIP_ADDR_LEN 	GAR3-GAR0
#define IP_ADDR_REG 	SIPR0
#define IP_ADDR_LEN 	SIPR3-SIPR0
#define NET_MASK_REG 	SUBR0
#define NET_MASK_LEN 	SUBR3-SUBR0
#define MAC_ADDR_REG 	SHAR0
#define MAC_ADDR_LEN 	SHAR5-SHAR0

#define MSR_VALUE 0x55
#define n(s) ((uint8_t)s)
#define TxSIZE(s) ((int16_t)(2048))
#define TxMASK(s) ((uint16_t)(TxSIZE(s)-1))
#define RxSIZE(s) ((int16_t)(2048))
#define RxMASK(s) (uint16_t)(RxSIZE(s)-1)



#define TX_memory_base_address 0x4000
#define RX_memory_base_address 0x6000
#define TxBASE(s) 	(TX_memory_base_address + n(s)*TxSIZE(s))
#define TxMAX(s) 	(TxBASE(s)+TxSIZE(s))
#define RxBASE(s) 	(RX_memory_base_address + n(s)*RxSIZE(s))
#define RxMAX(s) 	(RxBASE(s)+RxSIZE(s))




cW5100::cW5100(
		sensact::iMessageReceiverCallback *callback,
		SPI_HandleTypeDef * const hspi,
		GPIO_TypeDef * const ssGPIOx,
		uint16_t const ssGPIOpin,
		uint8_t const * const dns,
		uint8_t const * const gw,
		uint8_t const * const ip,
		uint8_t const * const mac,
		uint8_t const * const nm)
:callback(callback), hspi(hspi), ssGPIOx(ssGPIOx), ssGPIOpin(ssGPIOpin), dns(dns), gw(gw), ip(ip), mac(mac), nm(nm)
{
	this->acceptConnections=false;
}


eSocketResult cW5100::Init()
{
	write((uint16_t)eW5100Register::MR, 0);
	write((uint16_t)eW5100Register::SIPR0, this->ip[0]);
	write((uint16_t)eW5100Register::SIPR1, this->ip[1]);
	write((uint16_t)eW5100Register::SIPR2, this->ip[2]);
	write((uint16_t)eW5100Register::SIPR3, this->ip[3]);

	write((uint16_t)eW5100Register::SUBR0, this->nm[0]);
	write((uint16_t)eW5100Register::SUBR1, this->nm[1]);
	write((uint16_t)eW5100Register::SUBR2, this->nm[2]);
	write((uint16_t)eW5100Register::SUBR3, this->nm[3]);

	write((uint16_t)eW5100Register::GAR0, this->gw[0]);
	write((uint16_t)eW5100Register::GAR1, this->gw[1]);
	write((uint16_t)eW5100Register::GAR2, this->gw[2]);
	write((uint16_t)eW5100Register::GAR3, this->gw[3]);

	write((uint16_t)eW5100Register::SHAR0, this->mac[0]);
	write((uint16_t)eW5100Register::SHAR1, this->mac[1]);
	write((uint16_t)eW5100Register::SHAR2, this->mac[2]);
	write((uint16_t)eW5100Register::SHAR3, this->mac[3]);
	write((uint16_t)eW5100Register::SHAR4, this->mac[4]);
	write((uint16_t)eW5100Register::SHAR5, this->mac[5]);

	write((uint16_t)eW5100Register::RMSR, MSR_VALUE); //2k each
	write((uint16_t)eW5100Register::TMSR, MSR_VALUE); //2k each
	return eSocketResult::OK;
}
static uint16_t local_port=0;

eSocketResult cW5100::InitSocket(eSocketId id, eSocketProtocol protocol, uint16_t port, SocketFlags flags)
{
	eSocketResult ret;
	LOGD("socket()");
	Close(id);
	write((uint16_t)Sn_MR(id), (uint8_t)((uint8_t)protocol | (uint8_t)flags));
	if (port == 0) port=++local_port;
	write16((uint16_t)Sn_PORT0(id),port);
	return sendCommand(id, eSocketCmd::OPEN);// run sockinit Sn_CR
}
/**
@brief  This function close the socket and parameter is "s" which represent the socket number
 */
eSocketResult cW5100::Close(eSocketId id)
{
	LOGD("close()");
	eSocketResult ret=sendCommand(id, eSocketCmd::CLOSE);
	if(ret!=eSocketResult::OK) return ret;
	return write((uint16_t)Sn_IR(id), 0xFF);
}

eSocketStatus cW5100::getStatus(eSocketId sock) {
	int8_t sockstatus;
	if(read((uint16_t)Sn_SR(sock), (uint8_t*)&sockstatus)==eSocketResult::OK)
		return (eSocketStatus)sockstatus;

	return eSocketStatus::BUS_ERROR;
}

/**
@brief  This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
 */
eSocketResult cW5100::Listen(eSocketId s)
{
	eSocketResult ret;
	LOGD("listen()\r\n");
	switch (getStatus(s)) {
	case eSocketStatus::SOCK_INIT:
		return sendCommand(s, eSocketCmd::LISTEN);
	case eSocketStatus::BUS_ERROR:
		return eSocketResult::BUS_ERROR;
	default:
		return eSocketResult::WRONG_STATE;
	}
}



eSocketResult cW5100::Connect(eSocketId s, uint32_t addr, uint16_t port)
{
	eSocketResult ret;
	LOGD("connect()");
	// set destination IP
	write32(Sn_DIPR0(s), addr);
	write16(Sn_DPORT0(s), port);
	ret=sendCommand(s, eSocketCmd::CONNECT);
	return ret;
}

eSocketResult cW5100::Disconnect(eSocketId s)
{
	eSocketResult ret;
	LOGD("disconnect()");
	ret=sendCommand(s, eSocketCmd::DISCONNECT);
	return ret;
}

/**
@brief	 This function is being called by send() and sendto() function also.

This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
register. User should read upper byte first and lower byte later to get proper value.
*/
eSocketResult cW5100::send_data_processing(eSocketId s, uint8_t *data, uint16_t len)
{
	uint16_t ptr;
	uint8_t foo;
	eSocketResult ret;
	read16(Sn_TX_WR0(s), &ptr);
	write_data(s, data, ptr, len);
	ptr += len;
	write16(Sn_TX_WR0(s), ptr);
	return eSocketResult::OK;
}

/**
@brief	This function is being called by recv() also.

This function read the Rx read pointer register
and after copy the data from receive buffer update the Rx write pointer register.
User should read upper byte first and lower byte later to get proper value.
*/
eSocketResult cW5100::recv_data_processing(eSocketId s, uint8_t *data, uint16_t len)
{
	uint16_t ptr;
	read16(Sn_RX_RD0(s), &ptr);
	LOGD("ISR_RX: rd_ptr : %.4x\r\n", ptr);
	read_data(s, ptr, data, len); // read data
	ptr += len;
	write16(Sn_RX_RD0(s), ptr);
	return eSocketResult::OK;
}

/**
@brief	This function writes into W5100 memory(Buffer)
*/
uint16_t cW5100::wiz_write_buf(uint16_t addr, uint8_t* buf,uint16_t len)
{
	for(uint16_t idx=0;idx<len;idx++)
	{
		write(addr+idx, buf[idx]);
	}
	return len;
}

/**
@brief	This function reads into W5100 memory(Buffer)
*/
uint16_t cW5100::wiz_read_buf(uint16_t addr, uint8_t* buf,uint16_t len)
{
	for (uint16_t idx=0; idx<len; idx++)
    {
		read(addr+idx, (uint8_t*)&buf[idx]);
    }
	return len;
}

/**
@brief	for copy the data form application buffer to transmit buffer of the chip.

This function is being used for copy the data form application buffer to transmit
buffer of the chip. It calculate the actual physical address where one has to write
the data in transmit buffer. Here also take care of the condition while it exceed
the Tx memory upper-bound of socket.
*/
void cW5100::write_data(eSocketId s, const uint8_t * src, uint16_t dstAddr, uint16_t len)
{
	uint16_t size;
	uint16_t dst_ptr;

	uint16_t dst_mask = dstAddr & TxMASK(s);
	dst_ptr = TxBASE(s) + dst_mask;

	if (dst_mask + len > TxMAX(s))
	{
		size = TxMAX(s) - dst_mask;
		wiz_write_buf(dst_ptr, (uint8_t*)src, size);
		src += size;
		size = len - size;
		dst_ptr = TxBASE(s);
		wiz_write_buf(dst_ptr, (uint8_t*)src, size);
	}
	else
	{
		wiz_write_buf(dst_ptr, (uint8_t*)src, len);
	}
}

/**
@brief	This function is being used for copy the data form Receive buffer of the chip to application buffer.

It calculate the actual physical address where one has to read
the data from Receive buffer. Here also take care of the condition while it exceed
the Rx memory uper-bound of socket.
*/
void cW5100::read_data(eSocketId s, uint16_t srcAddr, uint8_t * dst, uint16_t len)
{
	uint16_t size;

	uint16_t src_ptr;

	uint16_t src_mask = srcAddr & RxMASK(s);
	src_ptr = RxBASE(s) + src_mask;

	if( (src_mask + len) > RxMAX(s) )
	{
		size = RxMAX(s) - src_mask;
		wiz_read_buf(src_ptr, dst, size);
		dst += size;
		size = len - size;
		src_ptr = RxBASE(s);
		wiz_read_buf(src_ptr, dst, size);
	}
	else
	{
		wiz_read_buf(src_ptr, dst, len);
	}
}

eSocketResult cW5100::Send(eSocketId s, uint8_t * buf, uint16_t len)
{
	eSocketStatus status;
	uint16_t ret;
	uint16_t freesize=0;
	LOGD("send()");

	if (len > TX_RX_MAX_BUF_SIZE )
	{
		return eSocketResult::PAYLOAD_TOO_LARGE; // check size not to exceed MAX size.
	}
	else
	{
		ret = len;
	}

	// if freebuf is available, start.
	do
	{
		freesize = getSn_TX_FSR(s);
		status = getStatus(s);
		if ((status != eSocketStatus::SOCK_ESTABLISHED) && (status != eSocketStatus::SOCK_CLOSE_WAIT))
		{
			ret=0;
		}
		LOGD("socket %d freesize(%d) empty or error\r\n", s, freesize);
	} while (freesize < ret);

	// copy data
	send_data_processing(s, buf, ret);
	sendCommand(s, eSocketCmd::SEND);
	uint8_t isr;
	read(Sn_IR(s), &isr);

		while (!SOCK_DATA_SENT(isr))
		{
			if (getStatus(s) == eSocketStatus::SOCK_CLOSED)
			{
				LOGD("SOCK_CLOSED.\r\n");
				Close(s);
				return eSocketResult::SOCKET_CLOSED;
			}
			read(Sn_IR(s), &isr);
		}

	write(Sn_IR(s), Sn_IR_SEND_OK);
	return eSocketResult::OK;
}

uint16_t cW5100::getSn_TX_FSR(eSocketId s)
{
	uint16_t val0=0,val1=0;
	do
	{
		read16(Sn_TX_FSR0(s),&val1);
		if (val1 != 0)
		{
			read16(Sn_TX_FSR0(s), &val0);

		}
	} while (val0 != val1);
	return val0;
}

eSocketResult cW5100::ListenServerUDP(eSocketId s, uint16_t port)
{
	this->acceptConnections=true;
	if(getStatus(s)!=eSocketStatus::SOCK_CLOSED) return eSocketResult::WRONG_STATE;
	write(Sn_MR(s), (uint8_t)eSocketProtocol::SOCK_PROTO_UDP);
	sendCommand(s, eSocketCmd::OPEN);
	if(getStatus(s)!=eSocketStatus::SOCK_INIT)
	{
		sendCommand(s, eSocketCmd::CLOSE);
		return eSocketResult::WRONG_STATE;
	}
	sendCommand(s, eSocketCmd::LISTEN);
	if(getStatus(s)!=eSocketStatus::SOCK_LISTEN)
	{
		sendCommand(s, eSocketCmd::CLOSE);
		return eSocketResult::WRONG_STATE;
	}
	return eSocketResult::OK;
}

eSocketResult cW5100::sendCommand(eSocketId sock, eSocketCmd cmd)
{
	write((uint16_t)Sn_CR(sock), (uint8_t)cmd);
	uint8_t cr = 0;

	do{
		read(Sn_CR(sock), &cr);
	} while(cr!=0);
	return eSocketResult::OK;
}


void cW5100::CallMeRegularly()
{
	uint8_t buf[256];
	if(this->acceptConnections)
	{
		for(uint8_t socknum = 0; socknum < SOCK_MAX_NUM; socknum++)
		{
			eSocketId socket = (eSocketId)socknum;
			if(getStatus(socket) != eSocketStatus::SOCK_ESTABLISHED) continue;
			uint8_t im;
			read(Sn_IR(socket), &im);
			if(!SOCK_CONNECTED(im)) continue;
			if(!SOCK_DATA_RECV(im)) continue;
			uint16_t get_size;
			uint16_t readPointer;
			write(Sn_IR(socket), 0x1); //Clears IR register

			read16(Sn_RX_RSR0(socket), &get_size);
			if(get_size==0) continue;
			read16(Sn_RX_RD0(socket), &readPointer);
			uint16_t memaddr;
			uint16_t bufptr=0;
			for(int i=0; i < get_size; i++) {
				memaddr = RxBASE(socket) + (readPointer++ & RxMASK(socket));
				read(memaddr, &buf[bufptr]);
				bufptr++;
			}
			write16(Sn_RX_RD0(socket), readPointer);
			sendCommand(socket, eSocketCmd::RECV);
		}
	}
}





/**
 * @brief  Write a single byte inside a given W5100 memory register
 * @param  hw5100: pointer to a W5100_Handle_TypeDef structure that contains
 *                the configuration information for W5100 ic.
 * @param  regaddr: first byte of the address location of memory register
 * @param  regl: second byte of the memory register
 * @param  Size: amount of data to be sent
 * @param  Timeout: Timeout duration
 * @retval HAL status
 */
eSocketResult cW5100::write(uint16_t regaddr, uint8_t data) {
	/* Every W5100 write command starts with 0xF0 byte, followed by the register address (2 bytes) and data (1 byte) */
	uint8_t buf[] = {0xF0, (uint8_t)(regaddr >> 8), (uint8_t)(regaddr & 0x00FF), data};

	HAL_GPIO_WritePin(this->ssGPIOx, this->ssGPIOpin, GPIO_PIN_RESET); //CS LOW
	HAL_SPI_Transmit(this->hspi, buf, 4, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(this->ssGPIOx, this->ssGPIOpin, GPIO_PIN_SET); //CS HIGH

	return eSocketResult::OK;
}


// Processor has little endina: lowest address is least significant byte
//w5100 has big endian - lowest address has the most significant e.g part of an IP-Address
//valid as well for socket: lowest address has MSB
eSocketResult cW5100::read16(uint16_t reg, uint16_t *data)
{
	uint8_t* ptr1=(uint8_t*)data;
	uint8_t* ptr2 = ptr1+1;
	read(reg, ptr1);
	read(reg+1, ptr2);
	return eSocketResult::OK;

}

eSocketResult cW5100::write16(uint16_t firstReg, uint16_t data)
{
	write(firstReg+0,(uint8_t)((data & 0xff00) >> 8));
	write(firstReg+1,(uint8_t)((data & 0x00ff) >> 0));
	return eSocketResult::OK;
}

eSocketResult cW5100::read32(uint16_t firstReg, uint32_t *data)
{
	uint8_t* ptr0=(uint8_t*)data;
	uint8_t* ptr1 = ptr0+1;
	uint8_t* ptr2 = ptr0+2;
	uint8_t* ptr3 = ptr0+3;
	read(firstReg+0, ptr0);
	read(firstReg+1, ptr1);
	read(firstReg+2, ptr2);
	read(firstReg+3, ptr3);
	return eSocketResult::OK;

}

eSocketResult cW5100::write32(uint16_t firstReg, uint32_t data)
{
	write(firstReg+0,(uint8_t)((data & 0xff000000) >> 24));
	write(firstReg+1,(uint8_t)((data & 0x00ff0000) >> 16));
	write(firstReg+2,(uint8_t)((data & 0x0000ff00) >>  8));
	write(firstReg+3,(uint8_t)((data & 0x000000ff) >>  0));
	return eSocketResult::OK;
}

eSocketResult cW5100::read(uint16_t regaddr, uint8_t * data) {
	HAL_StatusTypeDef status = HAL_OK;

	/* Every W5100 read command starts with 0x0F byte, followed by the register address (2 bytes) and data (1 byte) */
	uint8_t wbuf[] = {0x0F, (uint8_t)(regaddr >> 8), (uint8_t)(regaddr & 0x00FF), 0x00};
	uint8_t rbuf[4];

	HAL_GPIO_WritePin(this->ssGPIOx, this->ssGPIOpin, GPIO_PIN_RESET); //CS LOW
	status = HAL_SPI_TransmitReceive(this->hspi, wbuf, rbuf, 4, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(this->ssGPIOx, this->ssGPIOpin, GPIO_PIN_SET); //CS HIGH

	*data = rbuf[3];
	return eSocketResult::OK;
}

/**
@brief  This function is an application I/F function which is used to receive the data in TCP mode.
    It continues to wait for data as much as the application wants to receive.

@return received data size for success else -1.
 */
eSocketResult cW5100::Recv(eSocketId s, uint8_t * buf, uint16_t len)
{
	uint16_t ret=0;
	LOGD("recv()");
	if ( len > 0 )
	{
		recv_data_processing(s, buf, len);
		sendCommand(s, eSocketCmd::RECV);
		ret = len;
	}
	return eSocketResult::OK;
}


/**
@brief  This function is an application I/F function which is used to send the data for other than TCP mode.
    Unlike TCP transmission, The peer's destination address and the port is needed.

@return This function return send data size for success else -1.
 */
eSocketResult cW5100::Sendto(
		eSocketId s,     /**< socket index */
		uint8_t * buf,  /**< a pointer to the data */
		uint16_t len,     /**< the data size to send */
		uint32_t addr,     /**< the peer's Destination IP address  */
		uint16_t port   /**< the peer's destination port number */
)
{
	uint16_t ret=0;


	LOGD("sendto(%s)\r\n", buf);

	if (len > TxMAX(s)) ret = TxMAX(s); // check size not to exceed MAX size.
	else ret = len;

	write32(Sn_DIPR0(s),addr);
	write16(Sn_DPORT0(s), port);
	send_data_processing(s, buf, ret);
	sendCommand(s, eSocketCmd::SEND);
	uint8_t ir;
	read(Sn_IR(s), &ir);
	while ((ir & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
	{
		read(Sn_IR(s), &ir);
		if (ir & Sn_IR_SEND_OK)
		{
			LOGE("send fail.\r\n");
			write(Sn_IR(s), (Sn_IR_SEND_OK | Sn_IR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */
			return eSocketResult::SEND_ERROR;
		}
		read(Sn_IR(s), &ir);
	}
	write(Sn_IR(s), Sn_IR_SEND_OK);
	return eSocketResult::OK;
}

/**
@brief  This function is an application I/F function which is used to receive the data in other then
  TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well.

@return This function return received data size for success else -1.
 */
eSocketResult cW5100::Recvfrom(
		eSocketId s,   /**< the socket number */
		uint8_t * buf,  /**< a pointer to copy the data to be received */
		uint16_t len,   /**< the data size to read */
		uint32_t *addr,   /**< a pointer to store the peer's IP address */
		uint16_t *port  /**< a pointer to store the peer's port number. */
)
{
	uint8_t head[8];
	uint16_t data_len=0;
	uint16_t ptr=0;
	LOGD("recvfrom()\r\n");

	if ( len > 0 )
	{
		read16(Sn_RX_RD0(s), &ptr);
		LOGD("ISR_RX: rd_ptr : %.4x\r\n", ptr);
		uint8_t mr;
		read(Sn_MR(s), & mr);

		switch ((eSocketProtocol)(mr & 0x07))
		{
		case eSocketProtocol::SOCK_PROTO_UDP :
			read_data(s, ptr, head, 0x08);
			ptr += 8;
			// read peer's IP address, port number.
			addr[0] = head[0];
			addr[1] = head[1];
			addr[2] = head[2];
			addr[3] = head[3];
			*port = head[4];
			*port = (*port << 8) + head[5];
			data_len = head[6];
			data_len = (data_len << 8) + head[7];
			LOGD("UDP msg arrived\r\n");
			LOGD("source Port : %d\r\n", *port);
			LOGD("source IP : %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2], addr[3]);
			read_data(s, ptr, buf, data_len); // data copy.
			ptr += data_len;
			write16(Sn_RX_RD0(s), ptr);
			break;
		case eSocketProtocol::SOCK_PROTO_IPRAW :
			read_data(s, ptr, head, 0x06);
			ptr += 6;
			addr[0] = head[0];
			addr[1] = head[1];
			addr[2] = head[2];
			addr[3] = head[3];
			data_len = head[4];
			data_len = (data_len << 8) + head[5];
			LOGD("IP RAW msg arrived\r\n");
			LOGD("source IP : %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2], addr[3]);
			read_data(s, ptr, buf, data_len); // data copy.
			ptr += data_len;
			write16(Sn_RX_RD0(s),ptr);
			break;
		case eSocketProtocol::SOCK_PROTO_MACRAW:
			read_data(s,ptr,head,2);
			ptr+=2;
			data_len = head[0];
			data_len = (data_len<<8) + head[1] - 2;
			read_data(s,ptr,buf,data_len);
			ptr += data_len;
			write16(Sn_RX_RD0(s),ptr);
			LOGD("MAC RAW msg arrived\r\n");
			LOGD("dest mac=%.2X.%.2X.%.2X.%.2X.%.2X.%.2X\r\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
			LOGD("src  mac=%.2X.%.2X.%.2X.%.2X.%.2X.%.2X\r\n",buf[6],buf[7],buf[8],buf[9],buf[10],buf[11]);
			LOGD("type    =%.2X%.2X\r\n",buf[12],buf[13]);
			break;
		default :
			break;
		}
		sendCommand(s, eSocketCmd::RECV);
	}
	LOGD("recvfrom() end ..\r\n");
	return eSocketResult::OK;
}

}














