
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

#ifndef MyEthernetUDP_h
#define MyEthernetUDP_h

#define UDP_RX_MAX_PACKETS		4 // 8 K

#include "Energia.h"
#include <Udp.h>
#include <Retransmit.h>
#include <UDPResequence.h>

// rxbufs are initially allocated and chained into a circular queue
// Upon receive interrupt, data is consolidate from linked pbufs into a
// single rxbuf.
// rxbufs must be allocated in advance of the interrupt service routine.

struct rxBuf {
	struct rxBuf*	next;

	IPAddress		remoteIP;
	uint16_t		remotePort;
	
	MyPacket		packet;
	size_t			packetLength;

	rxBuf() {
		next			= NULL;
		packetLength	= 0; };
};

class MyEthernetUDP : public UDP {
private:
	struct rxBuf*	qIn;
	struct rxBuf*	qOut;

	// Buffers for server
	struct udp_pcb*	_pcb;

	uint16_t		_port;

	// pbuf, pcb, IP and port used when acting as a client
	struct pbuf*	_sendTop;
	struct udp_pcb*	_sendToPcb;

	IPAddress		_sendToIP;
	uint16_t		_sendToPort;
	size_t			_sendLength;

	uint16_t		_read;
	uint16_t		_write;

	static void do_recv( // Interrupt handler
		void			*arg,
		struct			udp_pcb *upcb,
		struct			pbuf *p,
		struct			ip_addr* addr,
		uint16_t		port);

	static void do_dns(
		const char		*name,
		struct ip_addr	*ipaddr,
		void			*arg);

public:
	MyEthernetUDP(); // constructor

	uint8_t
		begin( uint16_t );

	void
		stop();

	int // Calls beginPacket, write and endPacket in sequence
		sendDatagram (
			IPAddress		ip,
			uint16_t		port,
			const uint8_t*	buffer,
			size_t			size);

	int
		beginPacket (
			IPAddress ip,
			uint16_t port);

	int
		beginPacket (

		const char *host,
		uint16_t port);

	int // RCR added this method for efficiency if length is known in advance
		beginPacket(
			IPAddress	ip,
			uint16_t	port,
			size_t		size );

	int
		endPacket ();

// Can't work but must be defined
	size_t
		write(uint8_t);

	size_t
		write(
			const uint8_t *buffer,
			size_t size);

	using
		Print::write;

	UDPResequence*
		getBuffer();

	MyPacket*
		getPacket();

	int
		parsePacket();

	int
		available();

	int
		read();

	int
		read(
			char* buffer,
			size_t len);

	int
		read(
			unsigned char* buffer,
			size_t len);

	int
		peek();
	void
		flush();

	IPAddress
		remoteIP();

	uint16_t
		remotePort();
};

#endif
