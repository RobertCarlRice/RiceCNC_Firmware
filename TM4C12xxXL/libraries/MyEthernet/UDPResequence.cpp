
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

#include <UDPResequence.h>
#include "SysTick.h"
#include <MySerial.h>
#include <MyEthernetUdp.h>


UDPResequence::UDPResequence( struct rxBuf* aRxBuf ) {
	remoteIP		= aRxBuf->remoteIP;
	remotePort		= aRxBuf->remotePort;
	packetLength	= aRxBuf->packetLength;
	
	memcpy(
		(char*)			&packet,			// destination pointer
		(const char*)	&aRxBuf->packet,	// source pointer
		packetLength ); };

size_t
	UDPResequence::payloadSize() {

	return
		packetLength - PacketHeaderSize; };

bool
	UDPResequence::isRestart() {
	
	return
		packet.version == 0; };

void
	UDPResequence::sendDebug() {

	char data[ 40 ];
	snprintf( data, 40,
		" rx<%d<%d<%d<%x<%d",
		packet.version,
		packet.sequence,
		packet.ack,
		packet.ackBitField,
		packetLength );
	String msg = String( data );

	char* payloadPtr = packet.payload;
	for ( int i = 0; i < packet.messageCount; i++ ) {
		msg		+= "<";
		msg		+= String( payloadPtr );
		while ( *payloadPtr++ ); }; // advance to next string

	msg		+= "<<";
	MySerial::sendMessageNonBlocking( &msg ); };
