
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

#include <SSI.h>
#include <SsiDeviceQueue.h>
#include <Machine.h>
#include <AxisTimer.h>
#include <OutputPin.h>
#include <Timers.h>

#include <inc/hw_ssi.h>
#include <inc/hw_gpio.h>
#include <driverlib/ssi.h>


SSI*	SSI::firstSsi	= NULL;

// Need a separate interrupt handler for each SSI
SSI*    ssi0;
void    ssi0InterruptHandler(void) {
	ssi0->eotIntHandler(); };

SSI*    ssi1;
void    ssi1InterruptHandler(void) {
	ssi1->eotIntHandler(); };

SSI*    ssi2;
void    ssi2InterruptHandler(void) {
	ssi2->eotIntHandler(); };

SSI*    ssi3;
void    ssi3InterruptHandler(void) {
	ssi3->eotIntHandler(); };

SSI*
	SSI::ssiWithNumber( uint32_t ssiNumber ) {
	
	SSI* ssi		= SSI::firstSsi;
	while ( ssi ) {
		if ( ssi->number == ssiNumber )
			return
				ssi;
		ssi = ssi->nextSSI; };

	return
		NULL; };

SSI::SSI(
	char*		data,
	String*		msgPtr ) {

	firstSsiDeviceQueue		= NULL;

	number					= 0;
	shiftClockFrequency		= 0;
	configured				= false;

	devices					= 0;
	buffersInQueue			= 0;
	chipSelectPin			= NULL;

	parseLength				= 0;
	sscanf( data,
		"%c%lu %lu%ln",
		&configType,
		&number,
		&shiftClockFrequency,
		&parseLength );

	ssiBase       = 0;
	if ( number >= 4 ) {
		*msgPtr		+= "\nE SSI Invalid parameters";
		return; };
  
	shiftClockFrequency		= 62500 << shiftClockFrequency;

	char	buffer[ 40 ];
	snprintf( buffer, 40,
		" SSI %lu\n Clock %lu",
		number,
		shiftClockFrequency );
	*msgPtr			+= String( buffer );

	configureGPIOs(
		&base[ number ],
		msgPtr ); // sets ssiBase

	configured	= true; };

// destructor
SSI::~SSI() {
	Machine::deleteAllAxesUsingSsi( this );

	if ( ssiBase )
		ROM_SSIDisable( ssiBase ); // Disable the SSI module.

	// chain is in reverse sequence - write to last device in chain first
	while ( firstSsiDeviceQueue ) {
		SsiDeviceQueue* ssiDeviceQueue  = firstSsiDeviceQueue;
		firstSsiDeviceQueue				= firstSsiDeviceQueue->nextSsiDeviceQueue;
		delete ssiDeviceQueue; };

	SSI**  ssiPtrPtr   = &SSI::firstSsi;
	while ( *ssiPtrPtr ) {
		if ( *ssiPtrPtr == this ) {
			*ssiPtrPtr	= nextSSI; // Bypass
			break; };
		ssiPtrPtr		= &(*ssiPtrPtr)->nextSSI; }; };

void
	SSI::create_SsiDeviceQueues() {

	// chain is in reverse sequence - write to last device in chain first
	for ( int deviceNumber = 0;
			deviceNumber < devices;
			deviceNumber++ ) {

		SsiDeviceQueue* ssiDeviceQueue		= new SsiDeviceQueue( this, deviceNumber );
		ssiDeviceQueue->nextSsiDeviceQueue  = firstSsiDeviceQueue;
		firstSsiDeviceQueue					= ssiDeviceQueue; }; };

SsiDeviceQueue*
	SSI::ssiDeviceQueueWithNumber( uint32_t deviceNumber ) {

	SsiDeviceQueue*   ssiDeviceQueue  = firstSsiDeviceQueue;
	while ( ssiDeviceQueue && ssiDeviceQueue->number != deviceNumber )
		ssiDeviceQueue  = ssiDeviceQueue->nextSsiDeviceQueue;

	return
		ssiDeviceQueue; };

void
	SSI::enableSsiInterrupt() {

	SSIIntEnable( ssiBase, SSI_TXEOT );	};

void
	SSI::disableSsiInterrupt() {

	SSIIntDisable( ssiBase, SSI_TXEOT ); };

void
	SSI::configureGPIOs(
		baseStruct*		myBase,
		String*			msgPtr ) {

	ROM_SysCtlPeripheralEnable( myBase->sysCtl );

	ssiBase     = myBase->base;
	ROM_SSIDisable( ssiBase );

	*msgPtr		+= "\n  Tx    ";
	configurePin( &myBase->txPin, msgPtr );

	*msgPtr		+= "\n  Rx    ";
	configurePin( &myBase->rxPin, msgPtr );

	*msgPtr		+= "\n  Clk   ";
	configurePin( &myBase->clkPin, msgPtr );

	*msgPtr		+= "\n";

	ROM_SSIClockSourceSet( ssiBase, SSI_CC_CS_SYSPLL ); };


bool // only if common select for daisy-chain
	SSI::autoDetectDevices() {

	// We try alot to enable watching with oscilloscope
	for ( int count = 0; count < 10000; count++ )
		if ( detectDevices() )
			return true;

  return false; };


bool // only if common select for daisy-chain
	SSI::detectDevices() {

	chipSelectPin->setPin( true ); // active low
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

	chipSelectPin->setPin( false ); // active low
	SysCtlDelay( 10 ); // adjust for 1µs

	for ( int clear = 0; clear < 8; clear++ ) // FIFO is 8 deep
		HWREG( ssiBase + SSI_O_DR ); // Reads to empty Rx buffer

	if ( devices > 8 ) {
		devices  = 0;
		return false; };

	return true; };


void
	SSI::configurePin(
		PortStruct*		aStruct,
		String*			msgPtr ) {

	gpioBaseStruct* baseStruct = &GPIOBaseVector[ aStruct->portC - 'A' ];
	SysCtlPeripheralEnable( baseStruct->sysCtl );

	uint32_t gpioBase   = baseStruct->base;
	uint8_t  pinMask    = 1 << aStruct->pin;

	ROM_GPIOPinConfigure( aStruct->config );

	ROM_GPIODirModeSet(
		gpioBase,
		pinMask,
		GPIO_DIR_MODE_HW ); // ROM_GPIOPinTypeSSI( gpioBase, pinMask );

	ROM_GPIOPadConfigSet(
		gpioBase,
		pinMask,
		GPIO_STRENGTH_2MA,
		GPIO_PIN_TYPE_STD);

	char data[8];
	snprintf( data, 8,
		" P%c%hu",
		aStruct->portC,
		aStruct->pin );
	*msgPtr	+= String( data ); };


void
	SSI::configureSSI( uint32_t protocol ) {

//ROM_SSIConfigSetExpClk( ssiBase, F_CPU, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 800000, 8);
	uint32_t preDiv			= 0; // minimum value 2
	uint32_t targetPeriod	= ClockFrequency / shiftClockFrequency;
	uint32_t scr			= 512;

	while ( scr > 256 ) {
		preDiv   += 2;
		scr      = targetPeriod / preDiv; };

	HWREG( ssiBase + SSI_O_CPSR ) = preDiv;

	HWREG( ssiBase + SSI_O_CR0 ) = ( scr - 1 ) << 8 | protocol;

	HWREG( ssiBase + SSI_O_CR1 ) = SSI_MODE_MASTER | SSI_CR1_MODE_LEGACY;

	ROM_SSIEnable( ssiBase ); }; // Enable the SSI module.

/*
void
	SSI::configureInterrupts() { // Now set up interrupt handling

//	uint32_t priority	= daisyChain ?
//		AxisTimerPriorityLevel :
//		SSI_TxEotPriorityLevel; // higher than axis

	// Set handler and priority to 4 - just higher than axis int
	switch ( number ) {
		case 0: // Int 7 
		ssi0  = this;
		SSIIntRegister( ssiBase, ssi0InterruptHandler );
		// NVIC_PRI1 has priority for Ints 4..7
		HWREG( NVIC_PRI1  ) =
			( HWREG( NVIC_PRI1  ) & ~NVIC_PRI1_INT7_M ) |
			SSI_TxEotPriorityLevel << NVIC_PRI1_INT7_S; // 29
		break;
    
		case 1: // Int 34
		ssi1  = this;
		SSIIntRegister( ssiBase, ssi1InterruptHandler );
		// NVIC_PRI8 has priority for Ints 32..35
		HWREG( NVIC_PRI8  ) =
			( HWREG( NVIC_PRI8  ) & ~NVIC_PRI8_INT34_M ) |
			SSI_TxEotPriorityLevel << NVIC_PRI8_INT34_S; // 21
		break;
    
		case 2: // Int 54
		ssi2  = this;
		SSIIntRegister( ssiBase, ssi2InterruptHandler );
		// NVIC_PRI13 has priority for Ints 52..55
		HWREG( NVIC_PRI13 ) =
			( HWREG( NVIC_PRI13 ) & ~NVIC_PRI13_INT54_M ) |
			SSI_TxEotPriorityLevel << NVIC_PRI13_INT54_S; // 21
		break;
    
		case 3: // Int 55
		ssi3  = this;
		SSIIntRegister( ssiBase, ssi3InterruptHandler );
		// NVIC_PRI13 has priority for Ints 52..55
		HWREG( NVIC_PRI13 ) =
			( HWREG( NVIC_PRI13 ) & ~NVIC_PRI13_INT55_M ) |
			SSI_TxEotPriorityLevel << NVIC_PRI13_INT55_S; // 29
  };

	SSIIntClear(  ssiBase, SSI_TXEOT | SSI_DMATX | SSI_DMARX | SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR );
	enableSsiInterrupt(); };
*/

// Except for starting invocation, startSpiTransfer runs on TxEOT interrupt 
// TxEOT interrupts are disabled.
void
	SSI::startSpiTransfer() {

	if ( ! buffersInQueue )
		return;

	chipSelectPin->setPin( true ); // active low
//	TODO: SysCtlDelay( 2 ); // adjust for 500ns  tsetCS minimum is 350ns

	HWREG( ssiBase + SSI_O_DR ); // Extraneous read ensures Rx buffer is empty

	// send one byte from each daisy-chained device using transmit FIFO
	SsiDeviceQueue* ssiDeviceQueue = firstSsiDeviceQueue;
	while ( ssiDeviceQueue ) { // SPI FIFO queue is 8 deep
		HWREG( ssiBase	+ SSI_O_DR ) = ssiDeviceQueue->getByte();
		ssiDeviceQueue  = ssiDeviceQueue->nextSsiDeviceQueue; }; };


void
	SSI::sendWord( uint16_t data ) {
	// data is right justified - high order 16 bits ignored

	HWREG( ssiBase	+ SSI_O_DR ) = data; };

void
	SSI::waitUntilIdle() {

	while ( HWREG( ssiBase + SSI_O_SR ) & SSI_SR_BSY ); };

uint16_t // FIFO is defined as 16 bits wide
	SSI::readWord() {
	// data is right justified - high order 16 bits ignored

	return
		HWREG( ssiBase + SSI_O_DR ); };

void
	SSI::eotIntHandler() {

	// Data reading ends with the same timing as writing
	// Data input has been saved in the read FIFO buffers

	SSIIntClear( ssiBase, SSI_TXEOT | SSI_RXTO );

//	if ( daisyChain ) {

		// Add SSI interrupt time to the axis interrupt time
		AxisTimer::startAxisInterrupt();
	
			// Could still be transmitting lasy byte if early txeot
			waitUntilIdle();

			chipSelectPin->setPin( false ); // active low
			// TODO: 
			//		SysCtlDelay( 2 ); // adjust for 1µs

			// Transfer received data from read FIFO buffers to command-response buffers
			SsiDeviceQueue* ssiDeviceQueue = firstSsiDeviceQueue;
			while ( ssiDeviceQueue ) {
				ssiDeviceQueue->receiveByte( HWREG( ssiBase + SSI_O_DR ) );
				ssiDeviceQueue  = ssiDeviceQueue->nextSsiDeviceQueue; };

			startSpiTransfer(); // Loop continuously while buffers in queue

		AxisTimer::endAxisInterrupt(); }; // Run continuously

void
	SSI::selectUsingPin( OutputPin* pin ) {

	deselect();

	readWord();	// prevent RxOverrun

	if ( pin == chipSelectPin )	// wait for minimum deselect time of 100 ns
		SysCtlDelay( 2 );	// adjust for 1µs

	chipSelectPin	= pin;
	chipSelectPin->setPin( true ); };

void
	SSI::deselect() {

	waitUntilIdle();

	if ( chipSelectPin )
		chipSelectPin->setPin( false );	};
