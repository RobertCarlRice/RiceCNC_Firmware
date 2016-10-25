
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

#ifndef Retransmit_h
#define Retransmit_h

#include "Energia.h"

#define RiceCncMaxPacketSize	1024
#define MaxPayloadSize			RiceCncMaxPacketSize - 8

struct MyPacket {
	uint8_t		version;
	uint8_t		messageCount;
	uint8_t		sequence;
	uint8_t		ack;
	uint32_t	ackBitField;
	char		payload[ MaxPayloadSize ];
	
	MyPacket() { // define initializer
		version			= 1; // version 0 is reset
		messageCount	= 0;
		sequence		= 0;
		ack				= 0;
		ackBitField		= 0;
		payload[ 0 ]	= 0; }; };

#define PacketHeaderSize 8

class Retransmit {
private:

public:
	Retransmit();

	uint32_t	retransmissions;
    uint32_t	microSeconds;	// transmissionTime
	uint32_t	payloadLength; // bytes

	MyPacket	packet;

    bool
		expired();

	void
		sendDebug();

	bool
		addMessage( String* msgPtr );

	size_t
		capacity();
};

#endif
