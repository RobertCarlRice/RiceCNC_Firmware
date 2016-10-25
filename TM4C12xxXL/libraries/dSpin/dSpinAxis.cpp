
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

#include <dSpinAxis.h>
#include <DSpinBuffer.h>
#include <SsiDeviceQueue.h>
#include <MainCommands.h>
#include <SSI.h>
#include <dSpinDefines.h>
#include <MachineCommands.h>
#include <SysTick.h>
#include <AxisTimer.h>
#include <Gpio.h>


dSpinAxis::dSpinAxis( char* data, String* msgPtr ) :
	Axis( data, msgPtr ) {
	
	configured          = false;

	gpBuf               =  NULL;
	getStatusCR         =  NULL;
	getSpeedCR          =  NULL;
	getPosCR            =  NULL;

	positionChangeI		= 0;
	lastReportedSpeed   = 0;
	lastPosU            = 0;
	dSpinStatusRegU.dSpinStatusReg	= 0;
	reportedStatus      = 0;

#ifdef DEBUG
// TODO: 
debugTimer	= 0;
debugTimer2	= 0;
#endif

	deviceNum           = 0;
	overCurrent         = 0;
	overCurrentEnable   = 0;
	runPower            = 0;
	accelPower          = 0;
	decelPower          = 0;
	holdPower           = 0;
	stallDetect         = 0;

	uint32_t  ssiNum    = 0;

	sscanf( data + parseLength + 1,
		"%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
		&ssiNum,
		&deviceNum,

		&slewRate,
		&overCurrent,
		&overCurrentEnable,

		&runPower,
		&accelPower,
		&decelPower,
		&holdPower,
		&stallDetect );

	snprintf( data, 40,
		"\n SSI %lu device %lu",
		ssiNum,
		deviceNum );
	*msgPtr			+= String( data );

	ssi				= SSI::ssiWithNumber( ssiNum );
	if ( ! ssi ) {
		*msgPtr	+= "\nE SSI not configured";
		return; };

	ssiDeviceQueue	= ssi->ssiDeviceQueueWithNumber( deviceNum );
	if ( ! ssiDeviceQueue ) {
		*msgPtr	+= "\nE dSpin not configured";
		return; };

	slewRate   &= 3;

	snprintf( data, 40,
		"\n Slew Rate %u V/uS",
		slewRates[ slewRate ] );
	*msgPtr    += String( data );

	snprintf( data, 40,
		"\n overCurrent %lu ma",
		375 * ( overCurrent + 1 ) );
	*msgPtr    += String( data );
	
	*msgPtr    += overCurrentEnable ?
		" shutdown enabled" :
		" shutdown disabled";

	snprintf( data, 40,
		"\n runPower %0.1f %%",
		0.390625 * runPower );
	*msgPtr    += String( data );

	snprintf( data, 40,
		"\n accelPower %0.1f %%",
		0.390625 * accelPower );
	*msgPtr    += String( data );

	snprintf( data, 40,
		"\n decelPower %0.1f %%",
		0.390625 * decelPower );
	*msgPtr    += String( data );

	snprintf( data, 40,
		"\n holdPower %0.1f %%",
		0.390625 * holdPower );
	*msgPtr    += String( data );

	snprintf( data, 40,
		"\n stallDetect %0.1f ma",
		31.25 * ( stallDetect + 1 ) );
	*msgPtr    += String( data );

	// General purpose buffer
	gpBuf           = new DSpinBuffer( ssiDeviceQueue );

	if ( ! initialize( msgPtr ) ) {
		*msgPtr		+= "\nE Initialization failed";
		return; };
	
	gpBuf->softReset();
	
	// Buffers with callbacks
	getStatusCR     = new DSpinBuffer( ssiDeviceQueue );
	getStatusCR->addCallback(
		this,
		&dSpinAxis::newStatus );

	getSpeedCR      = new DSpinBuffer( ssiDeviceQueue );
	getSpeedCR->addCallback(
		this,
		&dSpinAxis::newSpeed );

	getPosCR        = new DSpinBuffer( ssiDeviceQueue );
	getPosCR->addCallback(
		this,
		&dSpinAxis::newPosition );

	seekVelocity( 0.0 );

	configured      = true;
	*msgPtr			+= "\n Initialized"; };


dSpinAxis::~dSpinAxis() {

	configured		= false; // stop requeueing

	if ( gpBuf )
		delete gpBuf;

	if ( getStatusCR )
		delete getStatusCR;

	if ( getSpeedCR )
		delete getSpeedCR;

	if ( getPosCR )
		delete getPosCR; };


bool
	dSpinAxis::initialize( String* msgPtr ) {

	// Block until each command is processed
	*msgPtr		+= " Initializing dSpin";
	uint32_t errors = 0;

	// Leave min speed set to zero
	*msgPtr		+= "\n  MaxSpeed";
	if ( ! gpBuf->setMaxSpeed(  maxStepsPerSecF, msgPtr ) )
		errors++;
  
	// Switch to full step above mid speed
	*msgPtr		+= "\n  FullSpeed";
	gpBuf->setFullSpeed( maxStepsPerSecF / 2.0,  msgPtr ); // Expect error verifying - Returns zero

	*msgPtr		+= "\n  Acceleration";
	if ( ! gpBuf->setAcceleration( float( accelStepsPerSecPerSecU ), msgPtr ) )
		errors++;
  
	*msgPtr		+= "\n  Deceleration";
	if ( ! gpBuf->setDeceleration( decelStepsPerSecPerSecF, msgPtr ) )
		errors++;

	*msgPtr		+= "\n  Run power";
	if ( ! gpBuf->setRunPower(     runPower,     msgPtr ) )
		errors++;

	*msgPtr		+= "\n  Accel power";
	if ( ! gpBuf->setAccelPower(   accelPower,   msgPtr ) )	
		errors++;

	*msgPtr		+= "\n  Decel power";
	if ( ! gpBuf->setDecelPower(   decelPower,   msgPtr ) )
		errors++;
    
// Cannot verify. Hold power reads back as zero
	*msgPtr		+= "\n  Hold power";
	gpBuf->setHoldPower( holdPower, msgPtr ); // Expect error verifying
  
	*msgPtr		+= "\n  Over-current";
	if ( ! gpBuf->setOvercurrentThreshold(   overCurrent, msgPtr ) )
		errors++;

// Cannot verify. Stall threshold reads back as 4 bits, i.e., mask 0x0F
	*msgPtr		+= "\n  Stall";
	gpBuf->setStallDetectThreshold( stallDetect, msgPtr ); // Expect error verifying
  
	*msgPtr		+= "\n  Micro-step";
	if ( ! gpBuf->setMicroStepMode( microStepU,   msgPtr ) )
		errors++;

	// Default configuration flags
	configurationFlags = 
		dSPIN_CONFIG_INT_16MHZ_OSCOUT_2MHZ || // default
		dSPIN_CONFIG_SW_USER ||               // switch has user function
		dSPIN_CONFIG_VS_COMP_ENABLE ||
		dSPIN_CONFIG_SR_320V_us ||            // Output slew rate
		dSPIN_CONFIG_PWM_MUL_1 || dSPIN_CONFIG_PWM_DIV_2; // 15.6 khz PWM
//	configurationFlags &= ~dSPIN_CONFIG_POW_SR;

	configurationFlags |= ( slewRate & 3 ) << 8;
  
	if ( overCurrentEnable ) // Shut down on overcurrent detection
		configurationFlags |= dSPIN_CONFIG_OC_SD_ENABLE;

	*msgPtr		+= "\n  Configuration";
	gpBuf->setParam(
		dSPIN_CONFIG,
		configurationFlags,
		msgPtr );
 
	return errors < 3; };
     

bool
	dSpinAxis::isConfigured() {

	return configured; };

bool
	dSpinAxis::usesSSI( SSI* aSsi ) {

	return aSsi == ssi; };

bool
	dSpinAxis::setTargetVelocityWithData(
		char*		data,
		String*		msgPtr ) { // Machine code

	if ( Axis::setTargetVelocityWithData(
			data,
			msgPtr ) )   // call super
		seekVelocity( targetStepsPerSecondF ); };

void
	dSpinAxis::reportAxisStatus( String* msgPtr ) {

	Axis::reportAxisStatus( msgPtr );
  
	if ( dSpinStatusRegU.dSpinStatusReg != reportedStatus ) {

		char  data[ 20 ];
		snprintf( data, 20,
			" s0x%X",
			reportedStatus );
		*msgPtr    += String( data );
		
		reportedStatus	= dSpinStatusRegU.dSpinStatusReg; }; };


void
	dSpinAxis::interpolationIsrEnd() { // Called on interpolation timer interrupt

	Axis::interpolationIsrEnd();

	if ( ! configured )
		return;

	// Requeue all data request buffers on varialble period axis timer interrupt 
	// Requeueing at EOT interrupt uses too much CPU time processing responses

	// Continuously fetch status while clearing error flags
	if ( ! getStatusCR->isInQueue() )
		getStatusCR->getStatus( true );

	if ( dSpinStatusRegU.dSpinStatusReg ) { // Will read as zero on motor power loss

		if ( ! getPosCR->isInQueue() ) // Fetch position
			getPosCR->getPosition();
 
		if ( motorState != HOLDING ) { // motor running
			if ( ! getSpeedCR->isInQueue() ) // Fetch motor speed
				getSpeedCR->getSpeed(); } 
  
		else if ( mvStepsPerSecF ) { // No need to fetch velocity if stopped
			mvStepsPerSecF	= 0.0;
			setFlag( VELOCITY_CHANGED ); }; }; };


void
	dSpinAxis::newStatus( uint32_t newStatus ) {

	if ( newStatus != dSpinStatusRegU.dSpinStatusReg ) {
		dSpinStatusRegU.dSpinStatusReg	= newStatus;
		motorState						= dSpinStatusRegU.st.motorState;
		setFlag( STATUS_CHANGED ); }; };

void
	dSpinAxis::newPosition( uint32_t newPosU ) {

	// Position is reported in micro-steps
	// Treat dSpin ABS_POS position as relative and extend to 32 bits.
	positionChangeI		= newPosU - lastPosU;

	if		( positionChangeI ) {
		lastPosU         = newPosU;

		if		( positionChangeI < -0x200000 )
			positionChangeI		+= 0x400000;

		else if ( positionChangeI >= 0x200000 )
			positionChangeI		-= 0x400000;

		AxisTimer::disableAxisInterrupts();
			stepped( float( positionChangeI ) );
		AxisTimer::enableAxisInterrupts(); }; };
	
//	Beacuse dSpin movement commands run to completion, i.e., full stop,
//	we coordinate axes movement by constantly adjusting the motor speeds,
//	always lagging behind the interpolated target position enough to stop
//	without overrunning target should the interpolation pause

void
	dSpinAxis::newSpeed( uint32_t newSpeed ) {

	// Speed is reported in steps/tick
	// convert from steps/tick to steps/sec
	float newSpeedF		= float( newSpeed ) * SpeedConversionFactor;
	if ( positionChangeI < 0 )
		newSpeedF		= -newSpeedF;

	if ( newSpeedF != mvStepsPerSecF ) {
		mvStepsPerSecF  = newSpeedF;
		setFlag( VELOCITY_CHANGED ); }; };

void
	dSpinAxis::calculateStopVelocity() {

	Axis::calculateStopVelocity();

	if		( targetStepsPerSecondF > maxStepsPerSecF ) {
		targetStepsPerSecondF	= maxStepsPerSecF;
		stepProgressF	= 0.5; }	// Too fast - slow interpolation

	else if	( targetStepsPerSecondF < -maxStepsPerSecF ) {
		targetStepsPerSecondF	= -maxStepsPerSecF;
		stepProgressF	= 0.5; };

	seekVelocity( targetStepsPerSecondF ); };

void
	dSpinAxis::seekVelocity( float stepsPerSecF ) {

	// Can't use softStop - it is not the same as setting the speed to zero
	gpBuf->setTargetVelocity( stepsPerSecF ); };

