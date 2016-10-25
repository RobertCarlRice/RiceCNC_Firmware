
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

#include <DRV8711Axis.h>
#include <SsiDeviceQueue.h>
#include <MainCommands.h>
#include <SSI.h>
#include <MachineCommands.h>
#include <OutputPin.h>
#include <AxisTimer.h>
#include <ADC.h>
#include <AnalogPin.h>

#include <inc/hw_gpio.h>
#include <driverlib/ssi.h>


// Add hysteresis so that mode won't keep switching running at a constant velocity
#define AdaptMicroStepUp	3.0
#define AdaptMicroStepDown	0.6 // Be sure we go down to highr resolution

// Use SPI control - Inherit directly from Axis
DRV8711Axis::DRV8711Axis(
	char*		data,
	String*		msgPtr )
	
	:	Axis(
			data,
			msgPtr ) {

	configured				= false;

	potPin					=  NULL;
	chipSelectPin			=  NULL;

	lastPosU				= 0;
	reportedStatus			= 0;
	reportedMode			= 10;

	reportedAnalog			= 0;

	uint32_t  ssiNum		= 0;
	deviceNum				= 0;

	char    potPinString		[8];
	char	chipSelectPinString [8];

	sscanf( data + parseLength + 1,
		"%lu %lu %s %s %x %x %x %x %x %x %x %x %x",
		&ssiNum,
		&deviceNum,

		potPinString,
		chipSelectPinString,

		&ctrlRegU.ctrlReg,
		&torqueRegU.torqueReg,
		&offRegU.offReg,
		&blankRegU.blankReg,
		&decayRegU.decayReg,
		&stallRegU.stallReg,
		&driveRegU.driveReg,
		&powerLevelsU.powerLevels,
		&optionsU.options );

	snprintf( data, 40,
		"\n SSI %lu device %lu",
		ssiNum,
		deviceNum );
	*msgPtr				+= String( data );

	ssi					= SSI::ssiWithNumber( ssiNum );
	if ( ! ssi ) {
		*msgPtr	+= "\nE SSI not configured";
		return; };

	*msgPtr				+= "\n Chip Select ";
	chipSelectPin	= new OutputPin(
		chipSelectPinString,
		msgPtr );
	if ( ! chipSelectPin->valid() ) {
		*msgPtr			+= String( chipSelectPinString );
		*msgPtr			+= " invalid";
		return; };

	chipSelectPin->fmtPin( msgPtr );

	potPin				= new AnalogPin( potPinString, msgPtr );
	if ( potPin->aid ) {
		*msgPtr			+= "\n Potentiometer";
		potPin->fmtPin( msgPtr );
		ADC::addAnalogPin( potPin ); }

	else {
		delete potPin;
		potPin	= NULL; };

	if ( ! initialize( msgPtr ) ) {
		*msgPtr			+= "\nE Initialization failed";
		return; };

	// save a copy
	reverseDirection	= ctrlRegU.st.reverseDirection;

	configured			= true;
	setMotorState();

	*msgPtr				+= "\n Initialized"; };


DRV8711Axis::~DRV8711Axis() {

#ifdef DebugDelete
	Serial.println( " delete DRV8711Axis" );
#endif

	if ( chipSelectPin )
		delete chipSelectPin;

	if ( potPin )
		delete potPin; };


bool
	DRV8711Axis::initialize( String* msgPtr ) {

	// extraneous read ensures rx FIFO is empty
	ssi->readWord();

	// Block until each command is processed
	*msgPtr		+= "\n Initializing BOOST-DRV8711 registers";
	uint32_t errors = 0;

	*msgPtr		+= "\n  Control";
	ctrlRegU.st.microStep	= microStepU;
	if ( ! setRegister(	ctrlRegU.ctrlReg,		msgPtr ) )
		errors++;

	*msgPtr		+= "\n  Torque";
	torqueRegU.st.torque	= powerLevelsU.st.holdTorque;
	if ( ! setRegister(	torqueRegU.torqueReg,	msgPtr ) )
		errors++;

	*msgPtr		+= "\n  Off";
	if ( ! setRegister(	offRegU.offReg,			msgPtr ) )
		errors++;
  
	*msgPtr		+= "\n  Blanking";
	if ( ! setRegister(	blankRegU.blankReg,		msgPtr ) )
		errors++;

	*msgPtr		+= "\n  Decay";
	if ( ! setRegister(	decayRegU.decayReg,		msgPtr ) )
		errors++;

	*msgPtr		+= "\n  Stall";
	if ( ! setRegister(	stallRegU.stallReg,		msgPtr ) )	
		errors++;

	*msgPtr		+= "\n  Drive";
	if ( ! setRegister(	driveRegU.driveReg,		msgPtr ) )
		errors++;

	if ( errors )
		return
			false;

	*msgPtr		+= "\n  Status";

	// Clear faults
	statusRegU.statusReg	= 0;
	statusRegU.st.addr		= 7;
	setRegister( statusRegU.statusReg, msgPtr );

	// enable the motor
	*msgPtr		+= "\n  Control";
	ctrlRegU.st.enableMotor	= 1;
	if ( ! setRegister(	ctrlRegU.ctrlReg,		msgPtr ) )
		errors++;

	// prepare for SPI control for axis interrupts
	ctrlRegU.st.forceStep	= 1;
	microStepSizeU			= 1;
	microStepSizeF			= 1.0;
	halfMicroStepSizeF		= 0.5;
	indexerPositionU		= 0;
	minStallDetectSpeedF	= float( optionsU.st.minStallDetectSpeed );

	return
		true; };

bool
	DRV8711Axis::isConfigured() {

	return
		configured; };

bool
	DRV8711Axis::usesSSI( SSI* aSsi ) {

	return
		aSsi == ssi; };

void
	DRV8711Axis::motorStepIsr() { // Called on axes timer interrupt

	if ( ! configured )
		return;

	updateVelocity();

	if ( optionsU.st.adaptiveMicroStepping ) {

		float indexerErrorF	= fabs( indexerOffsetF );

		if	(	indexerErrorF > microStepSizeF * AdaptMicroStepUp &&
				~indexerPositionU & microStepSizeU && // only degrade on even steps
				ctrlRegU.st.microStep )
			decreaseResolution();

		else if ( indexerErrorF <= microStepSizeF * AdaptMicroStepDown &&
				microStepSizeU > 1 )
			increaseResolution(); };

	// Try to stay within a half microstep
	if		( indexerOffsetF > halfMicroStepSizeF ) {
		ctrlRegU.st.reverseDirection	= ~reverseDirection;
		writeRegister( ctrlRegU.ctrlReg );	// Send forced step
		indexerPositionU				+= microStepSizeU;
		stepped( microStepSizeF ); }

	else if ( indexerOffsetF < -halfMicroStepSizeF ) {
		ctrlRegU.st.reverseDirection	= reverseDirection;
		writeRegister( ctrlRegU.ctrlReg );	// Send forced step
		indexerPositionU				-= microStepSizeU;
		stepped( -microStepSizeF ); }; };

void
	DRV8711Axis::increaseResolution() {

	ctrlRegU.st.microStep++;
	microStepSizeU		>>= 1;
	microStepSizeF		= float( microStepSizeU );
	halfMicroStepSizeF	= microStepSizeF * 0.5; };
	
void
	DRV8711Axis::decreaseResolution() {

	ctrlRegU.st.microStep--;
	halfMicroStepSizeF	= microStepSizeF;
	microStepSizeU		<<= 1;
	microStepSizeF		= float( microStepSizeU ); };

void // axis interrupt
	DRV8711Axis::motorStateChanged() {
	
	switch ( motorState ) {

		case HOLDING:
		torqueRegU.st.torque	= powerLevelsU.st.holdTorque;
		break;
		
		case ACCELERATING:
		torqueRegU.st.torque	= powerLevelsU.st.acceleratingTorque;
		break;
					
		case DECELERATING:
		torqueRegU.st.torque	= powerLevelsU.st.deceleratingTorque;
		break;

		case CONSTANT_SPEED:
		torqueRegU.st.torque	= powerLevelsU.st.constantTorque; };

	writeRegister( torqueRegU.torqueReg ); };

bool	
	DRV8711Axis::setRegister ( // Write and verify
		uint16_t	ctrlRegister,
		String*		msgPtr ) {

	writeRegister( ctrlRegister & 0x7FFF ); // should read back all ones on write
	SysCtlDelay( 1 );	// adjust for minimum deselect time 1µs

	// on read the device stops reading after the address bits then returns data
	writeRegister( ctrlRegister | 0x8000 );
	uint16_t readValue	= ssi->readWord();

	bool	match		= ( ( readValue ^ ctrlRegister ) & 0xFFF ) == 0;

	char  data[40];

	if ( match )
		snprintf( data, 40,
			" 0x%X verified",
			ctrlRegister );

	else
		snprintf( data, 40,
			" set 0x%X read 0x%X",
			ctrlRegister,
			readValue );

	*msgPtr    += String( data );

	return match; };


uint16_t
	DRV8711Axis::readRegister (
		uint16_t	ctrlRegister ) {

	writeRegister( ctrlRegister | 0x8000 );

	return
		ssi->readWord(); };

void
	DRV8711Axis::writeRegister (
		uint16_t	ctrlRegister ) {

	chipSelectPin->setPin( true );
		ssi->readWord();	// ensure buffer is empty
		ssi->sendWord( ctrlRegister );
		ssi->waitUntilIdle();
	chipSelectPin->setPin( false ); };

void
	DRV8711Axis::reportAxisStatus( String* msgPtr ) {

	// Read the status register and update the status
	AxisTimer::disableAxisInterrupts();
		uint16_t currentStatus =
			readRegister( statusRegU.statusReg );
	AxisTimer::enableAxisInterrupts();

	if ( ! optionsU.st.enableStallDetect )
		currentStatus &= ~0xC0;	// Ignore stall detect bits

	bool analogChange	= false;
	uint32_t currentAnalog;
	if ( potPin ) {	// Read and update the potentiometer analog output
		currentAnalog	= potPin->analogValue();
		analogChange	= abs( currentAnalog - reportedAnalog ) > 4; };
	
	// Set status change if either changed
	if (	currentStatus != reportedStatus
		||	ctrlRegU.st.microStep != reportedMode
		||	analogChange )
		setFlag( STATUS_CHANGED );

	Axis::reportAxisStatus( msgPtr );

	if ( ctrlRegU.st.microStep != reportedMode ) {
		char  data[ 20 ];
		snprintf( data, 20,
			" %c%d",
			MICROSTEP_MODE_CHANGE,
			ctrlRegU.st.microStep );
		*msgPtr    += String( data );

		reportedMode	= ctrlRegU.st.microStep; };

	if ( currentStatus != reportedStatus ) {
		char  data[ 20 ];
		snprintf( data, 20,
			" %c0x%X",
			DRV8711_STATUS,
			currentStatus );
		*msgPtr    += String( data );

		reportedStatus	= currentStatus; };

	if ( analogChange ) {
		char  data[ 20 ];
		snprintf( data, 20,
			" %c0x%X",
			POT_ANALOG,
			currentAnalog );
		*msgPtr    += String( data );

		reportedAnalog	= currentAnalog; }; };

	//	clear any faults
	//	writeRegister( statusRegU.statusReg ); };

void
	DRV8711Axis::reportAnalog(	String* msgPtr ) {
	
	if ( potPin )
		potPin->reportAnalog( msgPtr ); };

	
