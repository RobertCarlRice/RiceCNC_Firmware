
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

#ifndef MyEthernet_h
#define MyEthernet_h

#include "Energia.h"
#include "Retransmit.h"
#include "IPAddress.h"
#include "MyEthernetUdp.h"

class UDPResequence;

#define MaxRetransmissions 10

class MyEthernet {
private:
	Retransmit*		firstInBand;
	Retransmit*		currentInBand;
	Retransmit*		firstQueueEntry; // server uses this only for statistics
	Retransmit*		newTxBuffer;

	bool			configured;
	bool			connected;

	// statistics
	uint32_t		retransmissions;
	uint32_t		packetsSent;
	uint32_t		unacknowledged;

	uint8_t			sendAck;
	uint32_t		sendAckBitfield;

// An EthernetUDP instance to let us send and receive packets over UDP
    MyEthernetUDP	Udp;
	
	// Queue to enable retransmissions of unacknowledged packets
	Retransmit*		txQueue[ 256 ];
	UDPResequence*	rxQueue[ 256 ];

	uint8_t			txQueueHead;
	uint8_t			txQueueTail;
	uint8_t			rxQueueHead;

	uint8_t			txQueueCount;
	uint8_t			rxQueueCount;

	void
        firstDatagram( UDPResequence* receiveBuffer );
	bool
		addAck( uint8_t rcvSeq );

	void
		processAcks( UDPResequence* receiveBuffer );

	void
		ackEntry( uint8_t currentAck );

	void
		sendInBand();

	void
		retransmitExpired();

	void
		sendUDP( class Retransmit* payloadBuffer, bool sendDebug );

	void
		listenForDatagram();

	void
		addToNewBuffer( String* msgPtr );

	void
		sendNewTxBuffer();

public:
	bool
		udpConnected();

	bool
		isBusy();

	MyEthernet();

	void
		startEthernet();

	void
		loop();

	bool
		background();
};

#endif
