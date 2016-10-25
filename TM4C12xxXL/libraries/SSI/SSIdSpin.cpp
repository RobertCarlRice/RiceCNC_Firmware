
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

#include <SSIdSpin.h>
#include <SsiDeviceQueue.h>
#include <Machine.h>
#include <AxisTimer.h>
#include <OutputPin.h>
#include <Timers.h>

#include <inc/hw_ssi.h>
#include <inc/hw_gpio.h>
#include <driverlib/ssi.h>



SSIdSpin::SSIdSpin(
	char*		data,
	String*		msgPtr )
	
	:	SSI(
			data,
			msgPtr ) {

	firstSsiDeviceQueue		= NULL;

	resetPin				= NULL;
	chipSelect				= NULL;

	if ( ! configured )
		return;

	char    chipSelectPinString [8]; // or BOOST-DRV8711 sleep
	char    resetPinString      [8];

	sscanf( data + parseLength + 1,
		" %s %s",
		chipSelectPinString,
		resetPinString );

	// Reset pin is needed for both driver boards
	// to HARD Reset all devices on this SPI
	*msgPtr			+= "\n Reset";
	resetPin		= new OutputPin( resetPinString, msgPtr );
	if ( ! resetPin->valid() )
		return;
	resetPin->fmtPin( msgPtr );
	resetPin->setPin( true );

	*msgPtr			+= "\n Select";
	chipSelect		= new OutputPin( chipSelectPinString, msgPtr );
	if ( ! chipSelect->valid() )
		return;
	chipSelect->fmtPin( msgPtr );

// Can't use the frame select. Too fast for SsiDeviceQueue so use the pin to generate the chip select
//      ROM_GPIOPinTypeGPIOOutput( gpioBase, pinMask );

	configureSSI( ST_Protocol );
  
	resetPin->setPin( false ); // release reset

	if ( ! autoDetectDevices() ) {
		snprintf( data, 40,
			"E SSIdSpin %lu NO devices responding",
			number );
		*msgPtr     += String( data );
		return; }

	create_SsiDeviceQueues();

	snprintf( data, 60,
		"\n SSI %lu configured for %lu daisy-chained serial devices\n",
		number,
		devices );

	*msgPtr     += String( data );

//	configureInterrupts();
//	startSpiTransfer(); // Start continuous Run
//	reset_SsiDeviceQueues();

	configured	= true; };

// destructor
SSIdSpin::~SSIdSpin() {
	if ( resetPin )
		delete resetPin;

	if ( chipSelect )
		delete chipSelect; };


void
	SSIdSpin::create_SsiDeviceQueues() {

	// chain is in reverse sequence - write to last device in chain first
	for ( int deviceNumber = 0;
			deviceNumber < devices;
			deviceNumber++ ) {

		SsiDeviceQueue* ssiDeviceQueue		= new SsiDeviceQueue( this, deviceNumber );
		ssiDeviceQueue->nextSsiDeviceQueue  = firstSsiDeviceQueue;
		firstSsiDeviceQueue					= ssiDeviceQueue; }; };

SsiDeviceQueue*
	SSIdSpin::ssiDeviceQueueWithNumber( uint32_t deviceNumber ) {

	SsiDeviceQueue*   ssiDeviceQueue  = firstSsiDeviceQueue;
	while ( ssiDeviceQueue && ssiDeviceQueue->number != deviceNumber )
		ssiDeviceQueue  = ssiDeviceQueue->nextSsiDeviceQueue;

	return ssiDeviceQueue; };

void
	SSIdSpin::enableSsiInterrupt() {

	SSIIntEnable(
		ssiBase,
		SSI_TXEOT ); };

void
	SSIdSpin::disableSsiInterrupt() {

	SSIIntDisable(
		ssiBase,
		SSI_TXEOT ); };

bool // only if common select for daisy-chain
	SSIdSpin::autoDetectDevices() {

	// We try alot to enable watching with oscilloscope
	for ( int count = 0; count < 10000; count++ )
		if ( detectDevices() )
			return true;

	return
		false; };


bool // only if common select for daisy-chain
	SSIdSpin::detectDevices() {

	chipSelect->setPin( true ); // Select all SsiDeviceQueues
	SysCtlDelay( 10 ); // adjust for 500ns  tsetCS minimum is 350ns

	for ( int clear = 0; clear < 8; clear++ )
		HWREG( ssiBase + SSI_O_DR ); // Extraneous reads ensures Rx buffer is empty

	for ( devices = 0; devices <= 8; devices++ ) {
		HWREG( ssiBase + SSI_O_DR ) = 0xA5;        // Write byte test pattern
		while( ROM_SSIBusy( ssiBase ) );           // Wait for transmission to complete
		if ( HWREG( ssiBase + SSI_O_DR ) == 0xA5 ) // Read back byte test pattern
			break; }; // Success!

	for ( int clear = 0; clear < 8; clear++ ) // FIFO is 8 deep
		HWREG( ssiBase + SSI_O_DR ) = 0; // Clear to not confuse SsiDeviceQueues

	chipSelect->setPin( false ); // Deselect tdisCS minimum is 800ns
	SysCtlDelay( 10 ); // adjust for 1µs

	for ( int clear = 0; clear < 8; clear++ ) // FIFO is 8 deep
		HWREG( ssiBase + SSI_O_DR ); // Reads to empty Rx buffer

	if ( devices > 8 ) {
		devices  = 0;
		return
			false; };

	return
		true; };


void
	SSIdSpin::eotIntHandler() {

	// Data reading ends with the same timing as writing
	// Data input has been saved in the read FIFO buffers

	SSI::eotIntHandler();

	// Add SSI interrupt time to the axis interrupt time
	AxisTimer::startAxisInterrupt();

		// Could still be transmitting lasy byte if early txeot
		waitUntilIdle();

		chipSelect->setPin( false ); // Deselect tdisCS minimum is 800ns
		// TODO: 
		//		SysCtlDelay( 2 ); // adjust for 1µs

		// Transfer received data from read FIFO buffers to command-response buffers
		SsiDeviceQueue* ssiDeviceQueue = firstSsiDeviceQueue;
		while ( ssiDeviceQueue ) {
			ssiDeviceQueue->receiveByte( HWREG( ssiBase + SSI_O_DR ) );
			ssiDeviceQueue  = ssiDeviceQueue->nextSsiDeviceQueue; };

		startSpiTransfer(); // Loop continuously while buffers in queue

	AxisTimer::endAxisInterrupt(); // Run continuously
	};		// Deselect device
