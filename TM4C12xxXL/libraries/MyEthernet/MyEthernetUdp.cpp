
// © RiceMotion ( Robert Carl Rice ) 2012-2016 - (GPLv3)

// As of 10/23/2016 RiceMotion is releasing this firmware to the open source community
// under GNU General Public License v3 (GPL-3) to encourage support from other developers
// while ensuring that the firmware portion of this application remains free.
// Details of the GPLv3 license are available at the website:
// https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3)

// Note that his firmware is incomplete without a complementary host softwae application.
// RiceMotion offers a Mac desktop host application, RiceCNC, on the iTunes store.
// but encourages the development of compatable host software for other systems.
// For example, a web based (HTTP) interface could be useful.
// Host applications may be chargeable, but firmware modifications must be
// coordintated through GITHUB.

// This firmware is currently edited via XCODE and compiled using Energia.

// © RiceMotion ( Robert Carl Rice ) 2012-2016 - (GPLv3)

#include <MyEthernetUdp.h>
#include "lwip/udp.h"
#include <lwip/dns.h>

MyEthernetUDP::MyEthernetUDP() {
	_read		= 0;
	
	// create a circularly linked list of buffers
	qIn			= new rxBuf;
	qIn->next	= qIn;
	
	for (	int i = 1;
			i < UDP_RX_MAX_PACKETS;
			i++ ) {
		qOut		= new rxBuf;
		qOut->next	= qIn->next;
		qIn->next	= qOut; };

	qIn			= qOut; // empty
};

uint8_t
	MyEthernetUDP::begin( uint16_t port ) {

	_port		= port;
	_pcb		= udp_new();

	err_t err	= udp_bind(
		_pcb,
		IP_ADDR_ANY,
		port );

	if( err == ERR_USE )
		return false;

	// Register my interrupt handler
	_pcb->recv		= do_recv;
	_pcb->recv_arg	= this;

	return true;
};

int
	MyEthernetUDP::available()
{
	return qOut->packetLength ?
		qOut->packetLength - _read :
		0;
};

void
	MyEthernetUDP::stop()
{
	udp_remove( _pcb );
};

void
	MyEthernetUDP::do_dns(
		const char		*name,
		struct ip_addr	*ipaddr,
		void			*arg)
{
	ip_addr_t *result	= (ip_addr_t*) arg;

	/* BEWARE: lwip stack has been modified to set ipaddr
	 * to IPADDR_NONE if the lookup failed */
	result->addr		= ipaddr->addr;
};

// Output methods

int // Because all three methods current methods must be called in sequence anyway
	MyEthernetUDP::sendDatagram (
		IPAddress		ip,
		uint16_t		port,
		const uint8_t*	buffer,
		size_t			size) {
	
	if ( ! beginPacket (
		ip,
		port,
		size ) )
		return 0;

	if ( ! write (
		buffer,
		size ) )
		return 0;

	return endPacket();
};

int
	MyEthernetUDP::beginPacket(
		const char	*host,
		uint16_t	port)
{
	ip_addr_t	ip;
	ip.addr		= 0;

	dns_gethostbyname( host, &ip, do_dns, &ip );

	while( ! ip.addr );
//		delay( 10 );

	if ( ip.addr == IPADDR_NONE )
		return 0;

	return beginPacket(
		IPAddress( ip.addr ),
		port );
}

int
	MyEthernetUDP::beginPacket(
		IPAddress	ip,
		uint16_t	port)
{
	return beginPacket(
		ip,
		port,
		RiceCncMaxPacketSize );
};

int // RCR added this method for efficiency if length is known in advance
	MyEthernetUDP::beginPacket(
		IPAddress	ip,
		uint16_t	port,
		size_t		size )
{
	_sendToIP		= ip;
	_sendToPort		= port;
	_sendLength		= size;
	_write			= 0;

	_sendTop		= pbuf_alloc(
		PBUF_TRANSPORT,
		size,
		PBUF_POOL );

	return _sendTop != NULL;
};

int
	MyEthernetUDP::endPacket()
{
	ip_addr_t dest;
	dest.addr		= _sendToIP;

	/* Shrink the pbuf to the actual size that was written to it */
	if ( _write < _sendLength )
		pbuf_realloc( _sendTop, _write );

	/* Send the buffer to the remote host */
	err_t err		= udp_sendto(
		_pcb,
		_sendTop,
		&dest,
		_sendToPort );

	/* udp_sendto is blocking and the pbuf is
	 * no longer needed so free it */
	pbuf_free( _sendTop );

	return err == ERR_OK;
};

size_t // write a buffer
	MyEthernetUDP::write(
		const uint8_t*	buffer,
		size_t			size)
{
	uint16_t avail = _sendTop->tot_len - _write;

	// If there is no more space available then return immediately
	if( avail == 0 )
		return 0;

	/* If size to send is larger than is available,
	 * then only send up to the space available */
	if( size > avail )
		size = avail;
	/* Copy buffer into the pbuf */
	// ToDo: pbuf_take does not allow an offset into the payload buffer
	// therefore multiple writes between beginPacket and endPacket does not work
	pbuf_take( _sendTop, buffer, size );

	_write		+= size;

	return size;
};

size_t // write a byte
	MyEthernetUDP::write( uint8_t byte )
{
	return write( &byte, 1 );
};


// Input methods

void // Interrupt handler
	MyEthernetUDP::do_recv(

	void				*arg,
	struct udp_pcb		*upcb,
	struct pbuf			*pbuf_ref,
	struct ip_addr*		addr,
	uint16_t			port)
{
	MyEthernetUDP* udp	= static_cast<MyEthernetUDP*>( arg );

	struct rxBuf* qIn		= udp->qIn;
	struct rxBuf* qInNext	= qIn->next;

	if ( qInNext != udp->qOut ) { // Otherwise Full

		/* Record the IP address and port the packet was received from */
		qInNext->remoteIP		= IPAddress( addr->addr );
		qInNext->remotePort		= port;

		// copy the pbufs immediately to free the pool
		// don't depend on pbuf->tot_len to be correct

		struct pbuf* currentBuf	= pbuf_ref;
		char* destptr			= reinterpret_cast<char*>( &qInNext->packet );
		qInNext->packetLength	= 0;

		while ( currentBuf ) {
			qInNext->packetLength	+= currentBuf->len;
			if ( qInNext->packetLength > RiceCncMaxPacketSize ) {
				// packet is too long - no partial packets!!
				pbuf_free( pbuf_ref );
				return; };
	
			memcpy( destptr, (const char*) currentBuf->payload, currentBuf->len );
			destptr			+= currentBuf->len;
			currentBuf		= currentBuf->next; };

		/* Add packet to the queue */
		udp->qIn		= qInNext; };

	pbuf_free( pbuf_ref ); // receive queue full - lose the datagram
};

UDPResequence*
	MyEthernetUDP::getBuffer()
{
	if( qOut == qIn )
		return NULL; // empty

	// Accept the packet
	qOut		= qOut->next;
	_read		= 0;

	return
		new UDPResequence( qOut );
};
	
MyPacket*
	MyEthernetUDP::getPacket()
{
	if( qOut == qIn )
		return NULL; // empty

	// Accept the packet
	qOut		= qOut->next;
	_read		= 0;

	return &qOut->packet;
};

int // Return the length of the packet
	MyEthernetUDP::parsePacket()
{
	if( qOut == qIn )
		return 0; // empty

	// Accept the packet
	qOut		= qOut->next;
	_read		= 0;

	return qOut->packetLength;
};

int // read into a buffer
	MyEthernetUDP::read(
		char*	buffer,
		size_t	len )
{
	if ( len > available() )
		len		= available();
	
	if ( ! len )
		return -1;

	const char* srcptr	= reinterpret_cast<const char*>( &qOut->packet );
	memcpy( buffer, srcptr + _read, len );
	_read		+= len;

	return		len;
};

int // read a byte returning -1 on failure
	MyEthernetUDP::read()
{
	const char* srcptr	= reinterpret_cast<const char*>( &qOut->packet );
	return available() ?
		(uint8_t) srcptr[ _read++ ] :
		-1;
};

int
	MyEthernetUDP::read(
		unsigned char* buffer,
		size_t len) {
		
	return read(
		(char*)	buffer,
		len); };

int
	MyEthernetUDP::peek()
{
	const char* srcptr	= reinterpret_cast<const char*>( &qOut->packet );
	return available() ?
		(uint8_t) srcptr[ _read ] :
		-1;
};

void // Marks the current buffer as completely read
	MyEthernetUDP::flush()
{
	if ( available() )
		_read	= qOut->packetLength;
};

IPAddress
	MyEthernetUDP::remoteIP() {

	return qOut->remoteIP; };

uint16_t
	MyEthernetUDP::remotePort() {

	return qOut->remotePort; };
