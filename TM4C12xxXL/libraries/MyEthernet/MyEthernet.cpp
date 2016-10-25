
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

#include "MyEthernet.h"
#include "Ethernet.h"
#include "MyEthernetUdp.h"
#include "Timers.h"
#include "MainCommands.h"
#include "SysTick.h"
#include <MyEEProm.h>
#include <Machine.h>
#include <MySerial.h>
#include <Retransmit.h>
#include <UDPResequence.h>

#include "driverlib/eeprom.h"
#include <driverlib/sysctl.h>
#include "inc/hw_eeprom.h"
#include "inc/hw_sysctl.h"


#define	debug false

MyEthernet::MyEthernet() {
	newTxBuffer			= NULL;

    connected			= false;

	retransmissions		= 0;
	packetsSent			= 0;
	unacknowledged		= 0;

	sendAck				= 0;
	sendAckBitfield		= 0;

	// init transmit queue
	for (	int i = 0;
			i < 256;
			i++ ) {
		txQueue[i]		= NULL;
		rxQueue[i]		= NULL; };

	txQueueCount		= 0;
	rxQueueCount		= 0;

	txQueueHead			= 0;
	txQueueTail			= 0;
	rxQueueHead			= 0;
};

bool
	MyEthernet::udpConnected() {
		
	return connected; };

bool
	MyEthernet::isBusy() {

	return
		txQueueCount > 2	||
		rxQueueCount > 2;	};

void
	MyEthernet::startEthernet() {

	configured		= false;
    connected		= false;

    if ( MyEEProm::U.S.useKey != UseKey ) { // EEProm has been recorded
		String msg	= "E Ethernet not configured";
		MySerial::sendMessageNonBlocking( &msg );
        return; };

// Just in-case the ethernet server startup crashes the board
	MyEEProm::setUseKey( false );

    // Actual start
    Ethernet.begin(
        MyEEProm::U.S.macAddress,
        MyEEProm::U.S.myIpAddress,
        MyEEProm::U.S.routerAddress,
        MyEEProm::U.S.subnetMask );

    if ( ! Udp.begin( MyEEProm::U.S.port ) )
		return;

	// Not connected yet but able to receive
	IPAddress ip   = Ethernet.localIP();
	char data[ 60 ];
	snprintf( data, 60,
		"\nE TM4C1294XL\n UDP server at %d.%d.%d.%d port %d",
		ip[0],
		ip[1],
		ip[2],
		ip[3],
		MyEEProm::U.S.port );
	String msg	= String( data );;
	MySerial::sendMessageNonBlocking( &msg );
	
	configured		= true;
	MyEEProm::setUseKey( true ); };

void
	MyEthernet::loop() {

	if ( configured ) {
		MyEthernet::listenForDatagram();
		MyEthernet::retransmitExpired(); }; };

bool
	MyEthernet::background() {

	if ( ! connected )
		return false; // send bckground messages via serial

	Machine* machinePtr = Machine::firstMachine;
	if ( machinePtr ) {
		String msg	= "";
		machinePtr->background( &msg );
		addToNewBuffer( &msg ); };

// TODO: transmit could be delayed
	sendNewTxBuffer();

	return true; }; // send bckground messages via ethernet

void
	MyEthernet::listenForDatagram() {
 
	UDPResequence* receiveBuffer	= Udp.getBuffer(); // advances to next packet
	if ( ! receiveBuffer	)
		return;
	
	if ( debug )
		receiveBuffer->sendDebug();

	if ( receiveBuffer->payloadSize() < 2 ) { // too short - ignore it
		String msg	= " received empty packet";;
		MySerial::sendMessageNonBlocking( &msg );
	
		delete receiveBuffer;
		return; };

    Timers::setD4LED( true );

    if ( ! connected ) { // accept any remote for first packet
		firstDatagram( receiveBuffer );
		return; };

	if ( ! MyEEProm::remoteIsRecorded(
			receiveBuffer->remoteIP,
			receiveBuffer->remotePort ) ) {

		String msg	= " datagram from unknown host";;
		MySerial::sendMessageNonBlocking( &msg );
	
		delete receiveBuffer;
		return; }; // sorry not connected to you

	if ( receiveBuffer->isRestart() ) {
		MainCommands::softwareReset( MyEEProm::ResetRequestFromHost );
		return; };

    processAcks( receiveBuffer );

	uint8_t rcvSeq		= receiveBuffer->packet.sequence;
	if (	rxQueue[ rcvSeq ]	||
			addAck( rcvSeq ) ) {

		// Duplicate if already received and acked - don't process again
		if ( debug ) {
			String msg	= " duplicate packet received";;
			MySerial::sendMessageNonBlocking( &msg ); };

		delete receiveBuffer;
		return; };

	rxQueue[ rcvSeq ]	= receiveBuffer;
	rxQueueCount++;

	if ( debug && rcvSeq != rxQueueHead ) {
		String msg		= " rx out-of-sequence";;
		MySerial::sendMessageNonBlocking( &msg ); };

	// Process packets received in sequence
	while ( rxQueueCount && rxQueue[ rxQueueHead ] ) {
		UDPResequence* nextBuffer	= rxQueue[ rxQueueHead ];
		rxQueue[ rxQueueHead++ ]	= NULL;
		rxQueueCount--;

	//	UnPack multiple null terminated strings from receive packet
	//	and pack response messages into one packet
		char* command				= nextBuffer->packet.payload;

		if ( debug ) {
			String msg		= " processing: ";;
			msg				+= String( command );
			MySerial::sendMessageNonBlocking( &msg ); };

		for (	int i = 0;
				i < nextBuffer->packet.messageCount;
				i++ ) {
			String msg		= "";
			MainCommands::processCommand( command, &msg );
			addToNewBuffer( &msg );

			while ( *command++ ); }; // advance to next string

		delete nextBuffer; };
	
	sendNewTxBuffer(); }; // send immediate response

void
	MyEthernet::firstDatagram( UDPResequence* receiveBuffer ) {

	// First received packet after reset
	IPAddress remote_IP		= receiveBuffer->remoteIP;
	uint16_t remote_port	= receiveBuffer->remotePort;

	char data[ 60 ];
	snprintf( data, 60,
		" UDP connection from %d.%d.%d.%d port %d",
		remote_IP[0],
		remote_IP[1],
		remote_IP[2],
		remote_IP[3],
		remote_port );

	String msg	= String( data );;
	MySerial::sendMessageNonBlocking( &msg );

	// Do we know this guy?
	MyEEProm::recordConnectionFrom(
		remote_IP,
		remote_port );

	msg			= "";
	MainCommands::signOnMsg( &msg ); // "IR Version xx"
	addToNewBuffer( &msg );

	// set version to 0 to indicate reset
	newTxBuffer->packet.version = 0;
	sendNewTxBuffer();
	
	connected	= true; };


void
	MyEthernet::addToNewBuffer( String* msgPtr ) {

	size_t msgLength	= msgPtr->length();
	if ( msgLength && msgLength < MaxPayloadSize ) {
		*msgPtr			+= "\n"; // Same as for serial

		// Strings are created in a temporary autorelease pool
		// combine multiple responses into packet
		if ( ! newTxBuffer )
			newTxBuffer		= new Retransmit();

		if ( ! newTxBuffer->addMessage( msgPtr ) ) {
			sendNewTxBuffer(); // overcapacity - need a new buffer
			newTxBuffer		= new Retransmit();
			newTxBuffer->addMessage( msgPtr ); }; }; };

void
	MyEthernet::sendNewTxBuffer() {

	if ( newTxBuffer ) {

		newTxBuffer->packet.ack			= sendAck;
		newTxBuffer->packet.ackBitField	= sendAckBitfield;
		newTxBuffer->packet.sequence	= txQueueTail;
		newTxBuffer->microSeconds		= SysTick::microSecondTimer;

		sendUDP( newTxBuffer, debug );
		txQueueCount++;

		txQueue[ txQueueTail++ ]	= newTxBuffer;
		newTxBuffer					= NULL; }; };

void
	MyEthernet::sendUDP( Retransmit* payloadBuffer, bool sendDebug ) {

	if ( sendDebug )
		payloadBuffer->sendDebug();

	Udp.sendDatagram(
		MyEEProm::U.S.remoteIpAddress,
		MyEEProm::U.S.remote_Port,
		( const uint8_t* ) &payloadBuffer->packet,
		PacketHeaderSize + payloadBuffer->payloadLength );

	payloadBuffer->microSeconds		= SysTick::microSecondTimer;
	packetsSent++; };


bool // return true if duplicated
	MyEthernet::addAck( uint8_t rcvSeq ) {

	while ( (int8_t) ( rcvSeq - sendAck ) > 0 ) {
		sendAckBitfield <<= 1; // normally process once
		sendAck++; };

	uint32_t ackBit	= 1;
	while ( rcvSeq != sendAck ) {
		 // packet was received out-of-sequence

		if ( ackBit ==  0x80000000 ) {
			// This retransmission is really old but we still have to acknowledge it
			sendAckBitfield >>= 1;
			sendAck--; }

		else {
			ackBit <<= 1;
			rcvSeq++; }; };

	if ( sendAckBitfield & ackBit )
		return true; // duplicate

	sendAckBitfield |= ackBit;
	return false; };


void
	MyEthernet::retransmitExpired() {

	for ( uint8_t seq = txQueueHead; seq != txQueueTail; seq++ ) {
		Retransmit* entryPtr = txQueue[ seq ];
		if ( entryPtr && entryPtr->expired() ) {

			if ( entryPtr->retransmissions++ > MaxRetransmissions ) {
				MainCommands::softwareReset( MyEEProm::MaxPacketRetransmission );
				return; };

			sendUDP( entryPtr, true );
			retransmissions++; }; }; };

void
	MyEthernet::processAcks( UDPResequence* receiveBuffer ) {

	uint8_t currentAck	= receiveBuffer->packet.ack;
	uint32_t bits		= receiveBuffer->packet.ackBitField;

	while ( bits ) {
		if ( bits & 1 )
			ackEntry( currentAck );

		if ( currentAck == txQueueHead )
			break;

		bits >>= 1;
		currentAck--; };
	
	while ( txQueueHead != txQueueTail &&
			! txQueue[ txQueueHead ] )
		txQueueHead++;

	if ( txQueueCount == 0 )
		Timers::setD4LED( false ); };

void
	MyEthernet::ackEntry( uint8_t currentAck ) {

	Retransmit* current		= txQueue[ currentAck ];
	if ( current ) {
		delete current;
		txQueue[ currentAck ]	= NULL;
		txQueueCount--; }; };
