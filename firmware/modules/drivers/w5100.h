#pragma once

#include <cBsp.h>
#include <iMessageReceiverCallback.h>

namespace drivers
{

enum struct eSocketStatus:int8_t {
	BUS_ERROR			= -1,
	SOCK_CLOSED 		= 0x00,
	SOCK_ARP			= 0x01,
	SOCK_INIT			= 0x13,
	SOCK_LISTEN			= 0x14,
	SOCK_SYNSENT		= 0x15,
	SOCK_SYNRECV		= 0x16,
	SOCK_ESTABLISHED	= 0x17,
	SOCK_FIN_WAIT		= 0x18,
	SOCK_CLOSING		= 0x1A,
	SOCK_TIME_WAIT		= 0x1B,
	SOCK_LAST_ACK		= 0x1D,
	SOCK_CLOSE_WAIT		= 0x1C,
	SOCK_UDP			= 0x22,
	SOCK_IPRAW			= 0x32,
	SOCK_MACRAW			= 0x42,
	SOCK_PPPOE			= 0x5F
};

enum struct eSocketCmd : uint8_t
{
	OPEN		=0x01,
	LISTEN		=0x02,
	CONNECT		=0x04,
	DISCONNECT	=0x08,
	CLOSE		=0x10,
	SEND		=0x20,
	SEND_MAC	=0x21,
	SEND_KEEP	=0x22,
	RECV		=0x40,
};

enum struct eSocketProtocol:uint8_t {
	SOCK_PROTO_TCP		= 0x1,
	SOCK_PROTO_UDP		= 0x2,
	SOCK_PROTO_IPRAW	= 0x3,
	SOCK_PROTO_MACRAW	= 0x4,
	SOCK_PROTO_PPoE		= 0x5,
};

enum struct eSocketResult:uint8_t
{
	OK = 0,
	BUS_ERROR,
	WRONG_STATE,
	PAYLOAD_TOO_LARGE,
	SEND_ERROR,
};

enum SocketFlags
{
    Multicast = 0x80,
    MacFilter =0x40,
    UseNoDelayedAck = 0x20,
	UseIGMPv1 = 0x20,
};

inline SocketFlags operator|(SocketFlags a, SocketFlags b)
{return static_cast<SocketFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));}

enum struct eSocketId:uint8_t
{
	S0=0,
	S1=1,
	S2=2,
	S3=3,
};

#define IPADDR(a,b,c,d) (((uint8_t)a >> 0) + ((uint8_t)b >> 8) + ((uint8_t)c >> 16) + ((uint8_t)d >> 24))

class cW5100 {

private:
	SPI_HandleTypeDef * const hspi;
	GPIO_TypeDef * const ssGPIOx;
	uint16_t const ssGPIOpin;
	uint8_t const * const dns;
	uint8_t const * const gw;
	uint8_t const * const ip;
	uint8_t const * const mac;
	uint8_t const * const nm;
	bool acceptConnections;
	sensact::iMessageReceiverCallback *callback;
	eSocketResult write(uint16_t reg, uint8_t data);
	eSocketResult write16(uint16_t firstReg, uint16_t data);
	eSocketResult write32(uint16_t firstReg, uint32_t data);
	void write_data(eSocketId s, const uint8_t * src, uint16_t dstAddr, uint16_t len);
	void read_data(eSocketId s, uint16_t srcAddr, const uint8_t * dst, uint16_t len);
	uint16_t wiz_write_buf(uint16_t addr, const uint8_t* buf,uint16_t len);
	uint16_t wiz_read_buf(uint16_t addr, const uint8_t* buf,uint16_t len);
	eSocketResult read(uint16_t reg, const uint8_t *data);
	eSocketResult read16(uint16_t reg, uint16_t *data);
	eSocketResult read32(uint16_t firstReg, uint32_t *data);
	eSocketResult sendCommand(eSocketId id, eSocketCmd cmd);
    eSocketStatus getStatus(eSocketId sock);
    uint16_t socketFreeTXMEM(eSocketId sock);
    uint16_t getSn_TX_FSR(eSocketId s);
    eSocketResult send_data_processing(eSocketId s, const uint8_t *data, uint16_t len);
    eSocketResult recv_data_processing(eSocketId s, const uint8_t *data, uint16_t len);
    uint8_t buf[256];
public:
    cW5100(sensact::iMessageReceiverCallback *callback, SPI_HandleTypeDef * const hspi, GPIO_TypeDef * const ssGPIOx, uint16_t const ssGPIOpin, uint8_t const * const dns, uint8_t const * const gw, uint8_t const * const ip, uint8_t const * const mac, uint8_t const * const nm);
    eSocketResult Init();
    eSocketResult InitSocket(eSocketId id, eSocketProtocol protocol, uint16_t port, SocketFlags flags);
    eSocketResult Close(eSocketId id);
    eSocketResult Listen(eSocketId id);
    eSocketResult Connect(eSocketId s, uint32_t addr, uint16_t port);
    eSocketResult Disconnect(eSocketId s);
    eSocketResult Send(eSocketId s, const uint8_t * buf, uint16_t len);
    eSocketResult Recv(eSocketId s, const uint8_t * buf, uint16_t len);
    eSocketResult Sendto(
    		eSocketId s,     /**< socket index */
    		const uint8_t * buf,  /**< a pointer to the data */
    		uint16_t len,     /**< the data size to send */
    		uint32_t addr,     /**< the peer's Destination IP address */
    		uint16_t port   /**< the peer's destination port number */
    );
    eSocketResult Recvfrom(
    		eSocketId s,   /**< the socket number */
    		uint8_t * buf,  /**< a pointer to copy the data to be received */
    		uint16_t len,   /**< the data size to read */
    		uint32_t *addr,   /**< a pointer to store the peer's IP address */
    		uint16_t *port  /**< a pointer to store the peer's port number. */
    );
	bool ListenServerUDP(eSocketId socketId,  uint8_t const * const myIP, uint16_t myPort);
	void CallMeRegularly();
	void WriteUDPMessage(uint32_t destinationIp, uint16_t destinationPort, uint8_t * payload, uint16_t payloadLength);
};
}

