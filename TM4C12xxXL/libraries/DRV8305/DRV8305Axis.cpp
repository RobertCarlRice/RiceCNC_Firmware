
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

#include <DRV8305Axis.h>
#include <SsiDeviceQueue.h>
#include <MainCommands.h>
#include <SSI.h>
#include <MachineCommands.h>
#include <OutputPin.h>
#include <AxisTimer.h>
#include <ADC.h>
#include <BLDC3PWM.h>
#include <BLDC3PFM.h>
#include <QuadratureCounter4X.h>
#include <HallFeedback.h>
#include <CurrentFeedback.h>
#include <VoltageFeedback.h>
#include <InterpolationTimer.h>
#include <MotorVoltage.h>
#include <SysTick.h>
#include <QuadratureFeedback.h>

// TODO: 
//#include <AnalogPin.h>

#include <inc/hw_gpio.h>
#include <driverlib/ssi.h>

//#define DebugDelete 1

// Use SPI control - Inherit directly from Axis
DRV8305Axis::DRV8305Axis(
	char*		data,
	String*		msgPtr )

	:	Axis(
		data,
		msgPtr ) {

#ifdef DebugMotorStepISR
	debugCounter		= 0;
#endif

	motor				= NULL;
	quadratureFeedback	= NULL;
	hallFeedback		= NULL;
	currentFeedback		= NULL;
	voltageFeedback		= NULL;

	chipSelectPin		= NULL;
	enablePin			= NULL;

	drv8305AxisState	= ' ';
	configured			= false;

	reportedStatus		= 0;
	reportedMode		= 10;

	uint32_t  ssiNum	= 0;
	deviceNum			= 0;
	
	char	chipSelectPinString [8];
	char	enablePinString		[8];

	sscanf( data + parseLength + 1,
		"%lu %lu %s %s %x %x %x %x %x %x %x",
		&ssiNum,
		&deviceNum,

		chipSelectPinString,
		enablePinString,
	
		&hsGateDriveRegU.hsGateDriveReg,
		&lsGateDriveRegU.lsGateDriveReg,
		&gateDriverRegU.gateDriverReg,
		&vdsSenseControlRegU.vdsSenseControlReg,
		&icOperationRegU.icOperationReg,
		&currentSenseRegU.currentSenseReg,
		&voltageRegulatorControlRegU.voltageRegulatorControlReg );

	warnWatchdogRegU.st.addr		= 1;
	reportedWdStatus				= 0;

	ovFaultsRegU.st.addr			= 2;
	reportedOvStatus				= 0;

	icFaultsRegU.st.addr			= 3;
	reportedIcStatus				= 0;

	gateDriverFaultRegU.st.addr		= 4;
	reportedGdStatus				= 0;
	
	snprintf( data, 40,
		"\n SSI %lu device %lu",
		ssiNum,
		deviceNum );
	*msgPtr		+= String( data );

	ssi			= SSI::ssiWithNumber( ssiNum );
	if ( ! ssi ) {
		*msgPtr	+= "\nE SSI not configured";
		return; };

	*msgPtr		+= "\n Enable";
	enablePin	= new OutputPin(
		enablePinString,
		msgPtr );
	if ( ! enablePin->valid() ) {
		*msgPtr		+= " invalid";
		return; };

	enablePin->fmtPin( msgPtr );
	enable8305( false );	// Reset errors and enter standby mode

	*msgPtr		+= "\n Chip Select";
	chipSelectPin	= new OutputPin(
		chipSelectPinString,
		msgPtr );
	if ( ! chipSelectPin->valid() ) {
		*msgPtr		+= " invalid ";
		return; };

	chipSelectPin->fmtPin( msgPtr );

	if ( ! initialize( msgPtr ) ) {
		*msgPtr		+= "\nE Initialization failed";
		return; };

	configured		= true;

	setMotorState();

	*msgPtr		+= "\n Initialized"; };

DRV8305Axis::~DRV8305Axis() {

#ifdef DebugDelete
	Serial.println( " delete DRV8305Axis" );
#endif

	if ( enablePin )
		enable8305( false );	// Reset errors and enter standby mode

	Axis::removeMotorStepInterrupt( this );

	if ( motor )
		delete motor;

	if ( hallFeedback )
		delete hallFeedback;

	if ( quadratureFeedback )
		delete quadratureFeedback;

	if ( currentFeedback )
		delete currentFeedback;

	if ( voltageFeedback )
		delete voltageFeedback;

	if ( chipSelectPin )
		delete chipSelectPin;

	if ( enablePin )	// needs 25 µS to reset
		delete enablePin; };

bool
	DRV8305Axis::initialize( String* msgPtr ) {

	// Block until each command is processed
	*msgPtr		+= "\n Initializing BOOST-DRV8305 registers";
	uint32_t errors = 0;

	*msgPtr		+= "\n  HS";
	if ( ! setRegister(
			hsGateDriveRegU.hsGateDriveReg,
			msgPtr ) )
		errors++;

	*msgPtr		+= "\n  LS";
	if ( ! setRegister(
			lsGateDriveRegU.lsGateDriveReg,
			msgPtr ) )
		errors++;

	*msgPtr		+= "\n  FET";
	if ( ! setRegister(
			gateDriverRegU.gateDriverReg,
			msgPtr ) )
		errors++;

	*msgPtr		+= "\n  VDS";
	if ( ! setRegister(
			vdsSenseControlRegU.vdsSenseControlReg,
			msgPtr ) )
		errors++;
  
	*msgPtr		+= "\n  CS";
	if ( ! setRegister(
			currentSenseRegU.currentSenseReg,
			msgPtr ) )
		errors++;

	*msgPtr		+= "\n  VR";
	if ( ! setRegister(
			voltageRegulatorControlRegU.voltageRegulatorControlReg,
			msgPtr ) )	
		errors++;

	*msgPtr		+= "\n  Op";
	if ( ! setRegister(
			icOperationRegU.icOperationReg,
			msgPtr ) )
		errors++;

	return
		errors == 0; }

bool
	DRV8305Axis::isConfigured() {

	return
		configured; };

bool
	DRV8305Axis::usesSSI( SSI* aSsi ) {

	return
		aSsi == ssi; };

bool	
	DRV8305Axis::setRegister ( // Write and verify
		uint16_t	ctrlRegister,
		String*		msgPtr ) {

	// on write the device reads all bits but will return all zeros
	writeRegister( ctrlRegister & 0x7FFF );
	SysCtlDelay( 1 );	// adjust for minimum deselect time 1µs

	// on read the device stops reading after the address bits then returns data
	writeRegister( ctrlRegister | 0x8000 ); 
	uint16_t readValue	= ssi->readWord();;

	bool	match		= ( ( readValue ^ ctrlRegister ) & 0x7FF ) == 0;

	char  data[40];

	if ( match )
		snprintf( data, 40,
			" 0x%X ok",
			ctrlRegister );

	else
		snprintf( data, 40,
			" set 0x%X read 0x%X",
			ctrlRegister,
			readValue );

	*msgPtr    += String( data );

	return
		match; };

uint16_t
	DRV8305Axis::readRegister (
		uint16_t	ctrlRegister ) {

	writeRegister( ctrlRegister | 0x8000 );

	return
		ssi->readWord(); };

void
	DRV8305Axis::writeRegister (
		uint16_t	ctrlRegister ) {

	chipSelectPin->setPin( true );
		ssi->readWord();	// ensure buffer is empty
		ssi->sendWord( ctrlRegister );
		ssi->waitUntilIdle();
	chipSelectPin->setPin( false ); };

void
	DRV8305Axis::interpolationIsr(
		Machine*	machine ) {	// 2 kHz

	if ( ! motor )
		return;

	switch ( drv8305AxisState ) {

		case AXIS_CURRENT_LIMITED :

		if ( currentFeedback->currentLimitExceeded( motor ) )
			return;

		enable8305( true );
		drv8305AxisState	= AXIS_RUNNING;
		break;

		case AXIS_POWER_SUPPLY_LIMITED :

		if ( MotorVoltage::Low() )
			return;

		enable8305( true );
		drv8305AxisState	= AXIS_RUNNING;
		break;
	
		case AXIS_VOLTAGE_LIMITED :
		case AXIS_HALL_UNITIALIZED :
		case AXIS_QUADRATURE_UNITIALIZED :
		case AXIS_ACCELERATION_LIMITED :
		case AXIS_DECELERATION_LIMITED :
		case AXIS_RUNNING :

		if	(	currentFeedback
			&&	currentFeedback->currentLimitExceeded( motor ) ) {

			enable8305( false );
			drv8305AxisState	= AXIS_CURRENT_LIMITED;
			return; };

		if ( MotorVoltage::Low() ) {
			enable8305( false );
			drv8305AxisState	= AXIS_POWER_SUPPLY_LIMITED;
			return; }; };

	motor->interpolationIsr(
		machine ); };

void
	DRV8305Axis::enable8305(
		bool	enable ) {

//	motor->activeFreewheel();
	enablePin->setPin( enable ); };	// diode freewheel if false

void
	DRV8305Axis::motorStepIsr() {	// 15 kHz

	if ( ! motor )
		return;

	updateVelocity();

	//	quadratureFeedback will update steps on interpolation interrupt
	if (	quadratureFeedback
		&&	quadratureFeedback->synchronized )

		return;

//	Running open-loop

/* TODO: Step very slowly
	if ( ( ++debugCounter & 0x3FFF ) == 0 ) { // about 1 Hz
		motorState	= CONSTANT_SPEED;
		flags		|= STATUS_CHANGED;
		motorStateChanged();

		if	( currentFeedback ) {
			Serial.print( " Current A " );
			Serial.print( currentFeedback->csa->analogValue() );
			Serial.print( " B " );
			Serial.print( currentFeedback->csb->analogValue() );
			Serial.print( " C " );
			Serial.println( currentFeedback->csc->analogValue() ); };

		indexerOffsetF	+= 1.0;
		motor->stepArmature( 1 );
		stepped( 1.0 ); };
	return;
*/

	int32_t steps	= roundToInt( indexerOffsetF );
	if ( steps ) {
		motor->stepArmature( steps ); // Step immediately
		motor->assertTorque();
		stepped( float( steps ) ); }; };

void
	DRV8305Axis::reportAxisStatus( String* msgPtr ) {

	// Read the status registers and update the status
	AxisTimer::disableAxisInterrupts();
		uint16_t currentWdStatus =
			readRegister( warnWatchdogRegU.warnWatchdogReg );
	AxisTimer::enableAxisInterrupts();

	AxisTimer::disableAxisInterrupts();
		uint16_t currentOvStatus =
			readRegister( ovFaultsRegU.ovFaultsReg );
	AxisTimer::enableAxisInterrupts();

	AxisTimer::disableAxisInterrupts();
		uint16_t currentIcStatus =
			readRegister( icFaultsRegU.icFaultsReg );
	AxisTimer::enableAxisInterrupts();

	AxisTimer::disableAxisInterrupts();
		uint16_t currentGdStatus =
			readRegister( gateDriverFaultRegU.gateDriverFaultReg );
	AxisTimer::enableAxisInterrupts();

	// Set status change if any changed
	if (	currentWdStatus != reportedWdStatus
		||	currentOvStatus != reportedOvStatus
		||	currentIcStatus != reportedIcStatus
		||	currentGdStatus != reportedGdStatus )

		setFlag( STATUS_CHANGED );

	Axis::reportAxisStatus( msgPtr );

	char  data[ 40 ];

	if ( msgPtr->length() ) {
		snprintf( data, 40,
			" %c%c",
			DRV8305_STATUS,
			drv8305AxisState );
		*msgPtr    += String( data );
/*
		snprintf( data, 40,
			" %c%d",
			DRV8305_STATOR_ANGLE,
			motor->speedLeadAngle );
		*msgPtr    += String( data );
*/
		snprintf( data, 40,
			" %c%d",
			DRV8305_LEAD_ANGLE,
			motor->assertedLeadAngle );
		*msgPtr    += String( data );

		snprintf( data, 40,
			" %c%x",
			DRV8305_VOLTAGE,
			motor->assertedVoltage );
		*msgPtr    += String( data );
		
		if ( hallFeedback ) {
			snprintf( data, 40,
				" %c%d %c%d %c%d",
				DRV8305_HALL_ERROR,
				hallFeedback->error,
				DRV8305_INTEGRAL_ERROR,
				hallFeedback->integralError,
				DRV8305_FEEDBACK_TORQUE,
				hallFeedback->feedbackTorque );
			*msgPtr    += String( data ); };

		if ( quadratureFeedback ) {
			snprintf( data, 40,
				" %c%0.3f %c%0.3f %c%d",
				DRV8305_HALL_ERROR,
				indexerOffsetF,
				DRV8305_INTEGRAL_ERROR,
				quadratureFeedback->integralError,
				DRV8305_COUNTER_ADJUSTMENT,
				quadratureFeedback->stepAdjustment );
			*msgPtr    += String( data ); };

		if ( currentFeedback ) {
			uint32_t current =
				currentFeedback->motorCurrent( motor );
			snprintf( data, 40,
				" %c%x",
				DRV8305_CURRENT,
				current );
			*msgPtr    += String( data ); }; };

	if ( currentWdStatus != reportedWdStatus ) {
		snprintf( data, 40,
			" %c%X",
			DRV8305_WATCHDOG_REGISTER,
			currentWdStatus );
		*msgPtr    += String( data );

		reportedWdStatus	= currentWdStatus; };

	if ( currentOvStatus != reportedOvStatus ) {
		snprintf( data, 40,
			" %c%X",
			DRV8305_OVERVOLTAGE_FAULTS,
			currentOvStatus );
		*msgPtr    += String( data );

		reportedOvStatus	= currentOvStatus; };

	if ( currentIcStatus != reportedIcStatus ) {
		snprintf( data, 40,
			" %c%X",
			DRV8305_IC_FAULTS,
			currentIcStatus );
		*msgPtr    += String( data );

		reportedIcStatus	= currentIcStatus; };

	if ( currentGdStatus != reportedGdStatus ) {
		snprintf( data, 40,
			" %c%X",
			DRV8305_GATE_DRIVER_FAULTS,
			currentGdStatus );
		*msgPtr    += String( data );

		reportedGdStatus	= currentGdStatus; }; };

void
	DRV8305Axis::reportHallPinChange( String* msgPtr ) {
	
	if ( hallFeedback )
		hallFeedback->reportHallPinChange( msgPtr ); };

void
	DRV8305Axis::addMotor(
		char*		data,
		String*		msgPtr ) {

	enable8305( false );	// reset gates

	if ( motor ) {
		delete motor;
		motor	= NULL; };

	if ( configured ) {
		switch ( *data ) {
			case MOTOR_TYPE_BLDC3PFM :
			motor	= new BLDC3PFM (
				data,
				msgPtr,
				this );
			break;
			
			case MOTOR_TYPE_BLDC3PWM :
			motor	= new BLDC3PWM (
				data,
				msgPtr,
				this );
			break;
			
			default :
			*msgPtr		+= " Invalid BLDC Motor Type";
			return; };

		if ( ! motor->valid() ) {
			delete motor;
			motor	= NULL; }; };

	if ( motor ) {
		enable8305( true );	// enable gates
		motorStateChanged();
		*msgPtr		+= " BLDC Motor Added"; }

	else
		*msgPtr		+= " BLDC Motor Invalid"; };

void
	DRV8305Axis::addQuadratureFeedback(
		char*		data,
		String*		msgPtr,
		char		counterType ) {

	drv8305AxisState	= AXIS_QUADRATURE_UNITIALIZED;
	enable8305( false );

	if ( quadratureFeedback )
		delete quadratureFeedback;
	
	quadratureFeedback	= new QuadratureFeedback(
		data,
		msgPtr,
		this,
		counterType );

	if ( quadratureFeedback->valid() ) {
		*msgPtr		+= " BLDC Quadrature feedback added";
		enable8305( true ); }

	else {
		delete quadratureFeedback;
		quadratureFeedback	= NULL;
		*msgPtr		+= " BLDC Quadrature feedback invalid"; }; };

void
	DRV8305Axis::addHallFeedback(
		char*		data,
		String*		msgPtr ) {

	drv8305AxisState	= AXIS_HALL_UNITIALIZED;
	enable8305( false );

	if ( hallFeedback )
		delete hallFeedback;

	hallFeedback	= new HallFeedback(
		data,
		msgPtr,
		this );

	if ( hallFeedback->valid() ) {
		*msgPtr		+= " BLDC Hall feedback added";
		enable8305( true ); }

	else {
		delete hallFeedback;
		hallFeedback	= NULL;
		*msgPtr		+= " BLDC Hall feedback invalid"; }; };

void
	DRV8305Axis::addVoltageFeedback(
		char*		data,
		String*		msgPtr ) {

	if ( voltageFeedback )
		delete voltageFeedback;
	
	voltageFeedback	= new VoltageFeedback(
		data,
		msgPtr );

	if ( voltageFeedback->valid() )
		*msgPtr		+= " BLDC Voltage feedback added";

	else {
		delete voltageFeedback;
		voltageFeedback	= NULL;
		*msgPtr		+= " BLDC Voltage feedback invalid"; }; };

void
	DRV8305Axis::addCurrentFeedback(
		char*		data,
		String*		msgPtr ) {

	if ( currentFeedback )
		delete currentFeedback;

	currentFeedback	= new CurrentFeedback(
		data,
		msgPtr );

	if ( currentFeedback->valid() ) {
		motorStateChanged();
		*msgPtr		+= " BLDC Current feedback added"; }

	else {
		delete currentFeedback;
		currentFeedback	= NULL;
		*msgPtr		+= " BLDC Current feedback invalid"; }; };

void
	DRV8305Axis::motorStateChanged() {
	
	if ( motor )
		motor->motorStateChanged( motorState );

	if ( currentFeedback )
		currentFeedback->motorStateChanged( motorState ); };
