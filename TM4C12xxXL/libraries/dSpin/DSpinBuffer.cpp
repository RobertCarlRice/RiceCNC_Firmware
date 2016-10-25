
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

#include	<DSpinBuffer.h>
#include	<SsiDeviceQueue.h>
#include	<dSpinDefines.h>


DSpinBuffer::DSpinBuffer (
	SsiDeviceQueue*		aQueue ) : SsiBuffer( aQueue ) {

	target				= NULL;
	callBack			= NULL; };

 DSpinBuffer::~DSpinBuffer() {
	callBack			= NULL; };


void
	DSpinBuffer::addCallback (
		dSpinAxis*			aTarget,
		dSpinAxisCallback	aCallBack ) {

	target				= aTarget;
	callBack			= aCallBack; };

void
	DSpinBuffer::getParam( uint8_t param ) {

	queueWith(
		dSPIN_GET_PARAM | param,
		0,
		registerMask[ param ] ); };

bool
	DSpinBuffer::setParam(
		uint8_t		param,
		uint32_t	value,
		String*		msgPtr ) {

	uint32_t  mask  = registerMask[ param ];
	 
	queueWith(
		dSPIN_SET_PARAM | param,
		value,
		mask );

	if ( ! msgPtr )
		return true;
	 
	queueWith(
		dSPIN_GET_PARAM | param,
		0,
		mask );

	while ( isInQueue() ); // Block until prior command is processed
	uint32_t readValue  = myData.dataWord & mask;
	  
	char  data[40];
	if ( readValue == value )
		snprintf( data, 40,
			" 0x%X verified",
			value );

	else
		snprintf( data, 40,
			" set 0x%X read 0x%X",
			value,
			readValue );

	*msgPtr    += String( data );

	return readValue == value; };

void
	DSpinBuffer::rxComplete() {

	if ( target && callBack )
		(target->*callBack)( myData.dataWord & mask ); };

void
	DSpinBuffer::getMaxSpeed() {

	getParam( dSPIN_MAX_SPEED ); };

void
	DSpinBuffer::getSpeed()    {

	getParam( dSPIN_SPEED ); };

void
	DSpinBuffer::getPosition() {

	getParam( dSPIN_ABS_POS ); };

void
	DSpinBuffer::getPhase()    {

	getParam( dSPIN_EL_POS ); };
  
bool
	DSpinBuffer::setMinSpeed(
		float		stepsPerSecF,
		bool		lowSpeedOptimization,
		String*		msgPtr ) {

	uint32_t data	= stepsPerSecF * MinSpeedConversionFactor;
	if ( data > 0xFFF )
		data = 0xFFF;

	if ( lowSpeedOptimization )
		data |= 0x1000; // Set high order bit

	return setParam(
		dSPIN_MIN_SPEED,
		data,
		msgPtr ); };

bool
	DSpinBuffer::setMaxSpeed(
		float		stepsPerSecF,
		String*		msgPtr ) {

	return setParam(
		dSPIN_MAX_SPEED,
		(uint32_t)( stepsPerSecF * MaxSpeedSetConversionFactor ),
		msgPtr ); };

// Full speed is the speed at which the chip stops microstepping and switches to
// full step (both phases on) to achieve higher motor speeds.
bool
	DSpinBuffer::setFullSpeed(
		float		stepsPerSecF,
		String*		msgPtr ) {

	return setParam(
		dSPIN_FS_SPD,
		(uint32_t)( stepsPerSecF * MaxSpeedSetConversionFactor - 0.5 ),
		msgPtr ); };

bool
	DSpinBuffer::setAcceleration(
		float		stepsPerSecPerSecF,
		String*		msgPtr ) {

	return setParam(
		dSPIN_ACC,
		(uint32_t)( stepsPerSecPerSecF * AccelerationConversionFactor ),
		msgPtr ); };
  
bool
	DSpinBuffer::setDeceleration(
		float		stepsPerSecPerSecF,
		String*		msgPtr ) {

	return setParam(
		dSPIN_DEC,
		(uint32_t)( stepsPerSecPerSecF * AccelerationConversionFactor ),
		msgPtr ); };

// Power levels use an 8 bit scale, i.e., 255 is max
bool
	DSpinBuffer::setRunPower(
		uint32_t	powerU,
		String*		msgPtr ) {

	return setParam(
		dSPIN_KVAL_RUN,
		powerU,
		msgPtr ); };

bool
	DSpinBuffer::setAccelPower(
		uint32_t	powerU,
		String*		msgPtr ) {

	return setParam(
		dSPIN_KVAL_ACC,
		powerU,
		msgPtr ); };

bool
	DSpinBuffer::setDecelPower(
		uint32_t	powerU,
		String*		msgPtr ) {

	return setParam(
		dSPIN_KVAL_DEC,
		powerU,
		msgPtr ); };

bool
	DSpinBuffer::setHoldPower(
		uint32_t	powerU,
		String*		msgPtr ) {

	return setParam(
		dSPIN_KVAL_HOLD,
		powerU,
		msgPtr ); };

bool
	DSpinBuffer::setOvercurrentThreshold(
		uint32_t	threshold,
		String*		msgPtr ) {

	return setParam(
		dSPIN_OCD_TH,
		threshold,
		msgPtr ); };

bool
	DSpinBuffer::setStallDetectThreshold(
		uint32_t	threshold,
		String*		msgPtr ) {

	return setParam(
		dSPIN_STALL_TH,
		threshold,
		msgPtr ); };

bool
	DSpinBuffer::setMicroStepMode(
		uint32_t	mode,
		String*		msgPtr ) {

	return setParam(
		dSPIN_STEP_MODE,
		mode,
		msgPtr ); };

// Fetch and return the 16-bit value in the STATUS register.
void
	DSpinBuffer::getStatus( bool clear ) {

	if ( clear )
		queueWith(
			dSPIN_GET_STATUS,
			0,
			Bits16 ); // Reset any warning flags and exit any error state. 

	else
		getParam( dSPIN_STATUS ); };

// accelerate the motor to a constant velocity
void
	DSpinBuffer::setTargetVelocity( float velocityStepsPerSec ) {

	queueWith(
		velocityStepsPerSec >= 0 ?
			dSPIN_RUN | FWD :
			dSPIN_RUN,
		(uint32_t) fabs( velocityStepsPerSec ) * SpeedSetConversionFactor + 0.5,
		Bits20 ); };

void
	DSpinBuffer::runAtMinSpeed( bool forward ) {

	setTargetVelocity( forward ? 1 : -1 ); };
    
void
	DSpinBuffer::setPosition( uint32_t position ) {

	if ( position )
		setParam(
			dSPIN_ABS_POS,
			position,
			false ); // will error if running

	else
		resetPos(); };

// Bring the motor to a halt using the deceleration curve.
void
	DSpinBuffer::softStop()  {

	queueWith( dSPIN_SOFT_STOP ); };

// Stop the motor immediatly losing position.
void
	DSpinBuffer::hardStop()  {

	queueWith( dSPIN_HARD_STOP ); };

// Put the bridges in Hi-Z state immediately with no deceleration.
void
	DSpinBuffer::hardHiZ()   {

	queueWith( dSPIN_HARD_HIZ ); };

// Decelerate the motor and put the bridges in Hi-Z state.
void
	DSpinBuffer::softHiZ()   {
		
	queueWith( dSPIN_SOFT_HIZ ); };

// Reset device to power up conditions.
// Equivalent to toggling the STBY pin or cycling power.
void
	DSpinBuffer::softReset() {
		
	queueWith( dSPIN_RESET_DEVICE ); };

// Sets the ABS_POS register to 0, effectively declaring the current
//  position to be "HOME".
void
	DSpinBuffer::resetPos()  {
		
	queueWith( dSPIN_RESET_POS ); };

/*
	setStepClock() puts the device in external step clocking mode. When active,
	pin 25, STCK, will microstep the device in the requested direction.
	Any motion command (RUN, MOVE, etc) will cause the device
	to exit step clock mode. */
void
	DSpinBuffer::setStepClock( bool forward ) {

	queueWith(
		forward ?
			dSPIN_STEP_CLOCK | FWD :
			dSPIN_STEP_CLOCK,
		0,
		0 ); };

/*
	move() will send the motor n_step microsteps (size based on step mode) in the
	direction imposed by dir (FWD or REV constants may be used). The motor
	will accelerate according the acceleration and deceleration curves, and
	will run at MAX_SPEED. Stepping mode will adhere to FS_SPD value, as well. */
void
	DSpinBuffer::move( int32_t n_step ) {

	queueWith(
		n_step >= 0 ?
			dSPIN_MOVE | FWD :
			dSPIN_MOVE,
		abs( n_step ),
		Bits22 ); };

/*
	goTo operates much like MOVE, except it produces absolute motion instead
	of relative motion. The motor will be moved to the indicated position
	in the shortest direction, i.e., ABS_POS register can wrap. */
void
	DSpinBuffer::goTo( uint32_t pos ) {

	queueWith(
		dSPIN_GOTO,
		pos,
		Bits22 ); };

    /*
      goToWithDirection() is same as GOTO, but with user constrained rotational direction. */
void
	DSpinBuffer::goToWithDirection( int32_t pos ) {

	queueWith(
		pos >= 0 ?
			dSPIN_GOTO_DIR | FWD :
			dSPIN_GOTO_DIR,
		abs( pos ),
		Bits22 ); };

/*
	goUntil() will run at requested velocity
	until a falling edge is detected on the SW pin. Depending
	on bit SW_MODE in CONFIG, either a hard stop or a soft stop is
	then performed, and depending on the value of the copy parameter
	either the value in the ABS_POS register is RESET to 0 or
	the ABS_POS register is copied to the MARK register. */
void
	DSpinBuffer::goUntil( int32_t vel, bool copy ) {

	uint8_t         command = dSPIN_GO_UNTIL;
	if ( vel >= 0 ) command |= FWD;
	if ( copy )     command |= ACTION_COPY;

	queueWith(
		command,
		abs( vel ),
		Bits22 ); };

/*
	Similar to goUntil, releaseSW() runs at minimum speed (the higher
	of the value in MIN_SPEED or 5 steps/s) until a rising edge
	is detected on the switch input, then a hard stop is performed
	and the ABS_POS register is either copied into MARK or RESET to
	0, depending on the copy parameter. */
void
	DSpinBuffer::releaseSW( bool forward, bool copy ) {

	uint8_t         command = dSPIN_RELEASE_SW;
	if ( forward )  command |= FWD;
	if ( copy )     command |= ACTION_COPY;

	queueWith( command ); };

/*
	goHome() is equivalent to goTo(0), but requires less time to send.
	Note that no direction is provided; motion occurs through shortest
	path. If the ABS_POS register may have wraped and a direction is required,
	use GoTo_DIR(). */
void
	DSpinBuffer::goHome() {

	queueWith( dSPIN_GO_HOME ); };

/*
	goMark() is equivalent to GoTo( MARK ), but requires less time to send.
	Note that no direction is provided; motion occurs through shortest
	path. If a direction is required, use GoTo_DIR(). */
void
	DSpinBuffer::goMark() {

	queueWith( dSPIN_GO_MARK ); };
