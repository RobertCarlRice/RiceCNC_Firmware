
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

#include "Retransmit.h"
#include "SysTick.h"
#include <MySerial.h>


Retransmit::Retransmit() {
	retransmissions		= 0;
	payloadLength		= 0;
};


bool
	Retransmit::expired() {
	
	uint32_t elapsedTime = SysTick::microSecondTimer - microSeconds;
	return
		( elapsedTime > 3000000 ); }; // 3.0 sec

void
	Retransmit::sendDebug() {

	char data[ 40 ];
	snprintf( data, 40,
		" tx>%d>%d>%d>%x>%d",
		packet.version,
		packet.sequence,
		packet.ack,
		packet.ackBitField,
		payloadLength );
	String msg = String( data );

	char* msgPtr = packet.payload;
	for ( int i = 0; i < packet.messageCount; i++ ) {
		msg		+= String( ">" );
		msg		+= String( msgPtr );
		while ( *msgPtr++ ); }; // advance to next string

	msg		+= ">>";
	MySerial::sendMessageNonBlocking( &msg ); };

bool
	Retransmit::addMessage( String* msgPtr ) {
	
	size_t msgLength = msgPtr->length() + 1;
	if ( msgLength > capacity() )
		return false; // won't fit in this packet

	msgPtr->getBytes( // adds null termination
		(unsigned char*) &packet.payload[ payloadLength ],
		msgLength );

	packet.messageCount++;
	payloadLength	+= msgLength;

	return true; };

size_t
	Retransmit::capacity() {
	
	return MaxPayloadSize - payloadLength; };
