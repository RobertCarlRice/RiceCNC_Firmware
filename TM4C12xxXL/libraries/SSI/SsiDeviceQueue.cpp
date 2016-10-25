
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

#include <SsiDeviceQueue.h>
#include <SsiBuffer.h>
#include <SSI.h>
#include <GPIO.h>

#include <inc/hw_gpio.h>


SsiDeviceQueue::SsiDeviceQueue( SSI* aSsi, uint32_t myNumber ) {
	ssi					= aSsi;
	number				= myNumber;

	nextSsiDeviceQueue	= NULL;
	startPointer		= NULL;
	currentCR			= NULL; };

SsiDeviceQueue::~SsiDeviceQueue() {};

// byte mode is used for ST Microelectronics daisy-chain dSpin
byte
	SsiDeviceQueue::getByte() {

	return currentCR ?
		currentCR->getByte() :
		0; };

void
	SsiDeviceQueue::receiveByte( uint32_t data ) { // Always called by TxEOT Interrupt

	if ( currentCR )
		currentCR->receiveByte( data );

	else
		currentCR	= startPointer; };

void
	SsiDeviceQueue::dequeue() { // CR ended - txEOT interrupts already disabled

	startPointer	= startPointer->nextSsiBuffer;
	currentCR		= startPointer;
	ssi->buffersInQueue--; };

void
	SsiDeviceQueue::queue( SsiBuffer* request ) {

	// Add to end of buffer queue
	request->nextSsiBuffer  = NULL;
	SsiBuffer** crPtrPtr	= &startPointer;

	ssi->disableSsiInterrupt();
		while ( *crPtrPtr ) // end pointer will be NULL
			crPtrPtr    = &(*crPtrPtr)->nextSsiBuffer;
		*crPtrPtr   = request;

		if ( ssi->buffersInQueue++ == 0 )
			ssi->startSpiTransfer();
	ssi->enableSsiInterrupt(); };
