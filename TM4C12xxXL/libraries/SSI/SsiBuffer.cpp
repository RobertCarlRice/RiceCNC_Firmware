
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

#include	<SsiBuffer.h>
#include	<SsiDeviceQueue.h>


SsiBuffer::SsiBuffer( SsiDeviceQueue* aSsiDeviceQueue ) {
	ssiDeviceQueue	= aSsiDeviceQueue;

	nextSsiBuffer	= NULL;
	bytePointer		= NULL; };

SsiBuffer::~SsiBuffer() { // called automatically after derived destructor
	while( bytePointer ); };

uint32_t
	SsiBuffer::getWord() {
	
	return myData.dataWord; };

byte
	SsiBuffer::getByte() { // TxEOT interrupt except for first

	return bytePointer ?
		*bytePointer :
		0; }; // error

void
	SsiBuffer::receiveByte( byte receivedData ) { // TxEOT interrupt

	*bytePointer	= receivedData;
  
	if ( dataBytes ) {
		dataBytes--;
		bytePointer = &myData.dataByte[ dataBytes ]; }

	else {
		bytePointer		= NULL; // received last byte
		rxComplete();	// run callback function if one
		ssiDeviceQueue->dequeue(); }; };

void
	SsiBuffer::receiveWord( uint16_t receivedData ) {
	
	myData.dataWord = receivedData; // copy

	bytePointer		= NULL; // received last byte
	rxComplete();	// run callback function if one
	ssiDeviceQueue->dequeue(); };

void
	SsiBuffer::queueWithRegister( uint16_t aRegister ) {
	
	while ( isInQueue() ); // Block until prior command is processed

	myData.dataWord		= aRegister;
	bytePointer			= &command;
	queue(); };

void
	SsiBuffer::queueWith( uint8_t aCommand ) {
		
	queueWith( aCommand, 0, 0 ); };

void
	SsiBuffer::queueWith(
		uint8_t		aCommand,
		uint32_t	data,
		uint32_t	aMask ) {

	while ( isInQueue() ); // Block until prior command is processed

	command          = aCommand;
	myData.dataWord  = data > mask ?
		mask :
		data;
	mask             = aMask;
 
	bytePointer      = &command;
 
	if      ( mask == 0 )       dataBytes = 0;
//  else if ( mask & 0xFF000000 ) dataBytes = 4; // No dSpin data this long
	else if ( mask & 0xFF0000 ) dataBytes = 3;
	else if ( mask & 0xFFFF00 ) dataBytes = 2;
	else                        dataBytes = 1; // little-endian

	queue(); };

bool
	SsiBuffer::isInQueue() {

	IntMasterDisable();
		bool  queued = ( bytePointer != NULL );
	IntMasterEnable();

	return queued; };

void
	SsiBuffer::queue() {

	ssiDeviceQueue->queue( this ); };
