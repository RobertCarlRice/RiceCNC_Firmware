
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

#include	<PowerStepBuffer.h>
#include	<SsiDeviceQueue.h>
#include	<PowerStepDefines.h>


PowerStepBuffer::PowerStepBuffer (
	SsiDeviceQueue*		aQueue ) : SsiBuffer( aQueue ) {

	target				= NULL;
	callBack			= NULL; };

 PowerStepBuffer::~PowerStepBuffer() {
	callBack			= NULL; };


void
	PowerStepBuffer::addCallback (
		PowerStepAxis*			aTarget,
		PowerStepAxisCallback	aCallBack ) {

	target				= aTarget;
	callBack			= aCallBack; };

void
	PowerStepBuffer::getParam( uint8_t param ) {

	queueWith(
		PowerStep_GET_PARAM | param,
		0,
		registerMask[ param ] ); };

bool
	PowerStepBuffer::setParam(
		uint8_t		param,
		uint32_t	value,
		String*		msgPtr ) {

	uint32_t  mask  = registerMask[ param ];
	 
	queueWith(
		PowerStep_SET_PARAM | param,
		value,
		mask );

	if ( ! msgPtr )
		return true;
	 
	queueWith(
		PowerStep_GET_PARAM | param,
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

	return
		readValue == value; };

void
	PowerStepBuffer::rxComplete() {

	if ( target && callBack )
		(target->*callBack)( myData.dataWord & mask ); };

void
	PowerStepBuffer::getMaxSpeed() {

	getParam( PowerStep_MAX_SPEED ); };

void
	PowerStepBuffer::getSpeed()    {

	getParam( PowerStep_SPEED ); };

void
	PowerStepBuffer::getPosition() {

	getParam( PowerStep_ABS_POS ); };

void
	PowerStepBuffer::getPhase()    {

	getParam( PowerStep_EL_POS ); };
  
bool
	PowerStepBuffer::setMinSpeed(
		float		stepsPerSecF,
		bool		lowSpeedOptimization,
		String*		msgPtr ) {

	uint32_t data	= stepsPerSecF * MinSpeedConversionFactor;
	if ( data > 0xFFF )
		data = 0xFFF;

	if ( lowSpeedOptimization )
		data |= 0x1000; // Set high order bit

	return
		setParam(
			PowerStep_MIN_SPEED,
			data,
			msgPtr ); };

bool
	PowerStepBuffer::setMaxSpeed(
		float		stepsPerSecF,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_MAX_SPEED,
			(uint32_t)( stepsPerSecF * MaxSpeedSetConversionFactor ),
			msgPtr ); };

// Full speed is the speed at which the chip stops microstepping and switches to
// full step (both phases on) to achieve higher motor speeds.
bool
	PowerStepBuffer::setFullSpeed(
		float		stepsPerSecF,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_FS_SPD,
			(uint32_t)( stepsPerSecF * MaxSpeedSetConversionFactor - 0.5 ),
			msgPtr ); };

bool
	PowerStepBuffer::setAcceleration(
		float		stepsPerSecPerSecF,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_ACC,
			(uint32_t)( stepsPerSecPerSecF * AccelerationConversionFactor ),
			msgPtr ); };
  
bool
	PowerStepBuffer::setDeceleration(
		float		stepsPerSecPerSecF,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_DEC,
			(uint32_t)( stepsPerSecPerSecF * AccelerationConversionFactor ),
			msgPtr ); };

// Power levels use an 8 bit scale, i.e., 255 is max
bool
	PowerStepBuffer::setRunPower(
		uint32_t	powerU,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_KVAL_RUN,
			powerU,
			msgPtr ); };

bool
	PowerStepBuffer::setAccelPower(
		uint32_t	powerU,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_KVAL_ACC,
			powerU,
			msgPtr ); };

bool
	PowerStepBuffer::setDecelPower(
		uint32_t	powerU,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_KVAL_DEC,
			powerU,
			msgPtr ); };

bool
	PowerStepBuffer::setHoldPower(
		uint32_t	powerU,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_KVAL_HOLD,
			powerU,
			msgPtr ); };

bool
	PowerStepBuffer::setOvercurrentThreshold(
		uint32_t	threshold,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_OCD_TH,
			threshold,
			msgPtr ); };

bool
	PowerStepBuffer::setStallDetectThreshold(
		uint32_t	threshold,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_STALL_TH,
			threshold,
			msgPtr ); };

bool
	PowerStepBuffer::setMicroStepMode(
		uint32_t	mode,
		String*		msgPtr ) {

	return
		setParam(
			PowerStep_STEP_MODE,
			mode,
			msgPtr ); };

// Fetch and return the 16-bit value in the STATUS register.
void
	PowerStepBuffer::getStatus( bool clear ) {

	if ( clear )
		queueWith(
			PowerStep_GET_STATUS,
			0,
			Bits16 ); // Reset any warning flags and exit any error state. 

	else
		getParam( PowerStep_STATUS ); };

// accelerate the motor to a constant velocity
void
	PowerStepBuffer::setTargetVelocity( float velocityStepsPerSec ) {

	queueWith(
		velocityStepsPerSec >= 0 ?
			PowerStep_RUN | PowerStepForward
		:	PowerStep_RUN,
		(uint32_t) fabs( velocityStepsPerSec ) * SpeedSetConversionFactor + 0.5,
		Bits20 ); };

void
	PowerStepBuffer::runAtMinSpeed( bool forward ) {

	setTargetVelocity( forward ? 1 : -1 ); };
    
void
	PowerStepBuffer::setPosition( uint32_t position ) {

	if ( position )
		setParam(
			PowerStep_ABS_POS,
			position,
			false ); // will error if running

	else
		resetPos(); };

// Bring the motor to a halt using the deceleration curve.
void
	PowerStepBuffer::softStop()  {

	queueWith( PowerStep_SOFT_STOP ); };

// Stop the motor immediatly losing position.
void
	PowerStepBuffer::hardStop()  {

	queueWith( PowerStep_HARD_STOP ); };

// Put the bridges in Hi-Z state immediately with no deceleration.
void
	PowerStepBuffer::hardHiZ()   {

	queueWith( PowerStep_HARD_HIZ ); };

// Decelerate the motor and put the bridges in Hi-Z state.
void
	PowerStepBuffer::softHiZ()   {
		
	queueWith( PowerStep_SOFT_HIZ ); };

// Reset device to power up conditions.
// Equivalent to toggling the STBY pin or cycling power.
void
	PowerStepBuffer::softReset() {
		
	queueWith( PowerStep_RESET_DEVICE ); };

// Sets the ABS_POS register to 0, effectively declaring the current
//  position to be "HOME".
void
	PowerStepBuffer::resetPos()  {
		
	queueWith( PowerStep_RESET_POS ); };

/*
	setStepClock() puts the device in external step clocking mode. When active,
	pin 25, STCK, will microstep the device in the requested direction.
	Any motion command (RUN, MOVE, etc) will cause the device
	to exit step clock mode. */
void
	PowerStepBuffer::setStepClock( bool forward ) {

	queueWith(
		forward ?
			PowerStep_STEP_CLOCK | PowerStepForward :
			PowerStep_STEP_CLOCK,
		0,
		0 ); };

/*
	move() will send the motor n_step microsteps (size based on step mode) in the
	direction imposed by dir (PowerStepForward or REV constants may be used). The motor
	will accelerate according the acceleration and deceleration curves, and
	will run at MAX_SPEED. Stepping mode will adhere to FS_SPD value, as well. */
void
	PowerStepBuffer::move( int32_t n_step ) {

	queueWith(
		n_step >= 0 ?
			PowerStep_MOVE | PowerStepForward :
			PowerStep_MOVE,
		abs( n_step ),
		Bits22 ); };

/*
	goTo operates much like MOVE, except it produces absolute motion instead
	of relative motion. The motor will be moved to the indicated position
	in the shortest direction, i.e., ABS_POS register can wrap. */
void
	PowerStepBuffer::goTo( uint32_t pos ) {

	queueWith(
		PowerStep_GOTO,
		pos,
		Bits22 ); };

    /*
      goToWithDirection() is same as GOTO, but with user constrained rotational direction. */
void
	PowerStepBuffer::goToWithDirection( int32_t pos ) {

	queueWith(
		pos >= 0 ?
			PowerStep_GOTO_DIR | PowerStepForward :
			PowerStep_GOTO_DIR,
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
	PowerStepBuffer::goUntil( int32_t vel, bool copy ) {

	uint8_t         command = PowerStep_GO_UNTIL;
	if ( vel >= 0 ) command |= PowerStepForward;
	if ( copy )     command |= PowerStepActionCopy;

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
	PowerStepBuffer::releaseSW( bool forward, bool copy ) {

	uint8_t         command = PowerStep_RELEASE_SW;
	if ( forward )  command |= PowerStepForward;
	if ( copy )     command |= PowerStepActionCopy;

	queueWith( command ); };

/*
	goHome() is equivalent to goTo(0), but requires less time to send.
	Note that no direction is provided; motion occurs through shortest
	path. If the ABS_POS register may have wraped and a direction is required,
	use GoTo_DIR(). */
void
	PowerStepBuffer::goHome() {

	queueWith( PowerStep_GO_HOME ); };

/*
	goMark() is equivalent to GoTo( MARK ), but requires less time to send.
	Note that no direction is provided; motion occurs through shortest
	path. If a direction is required, use GoTo_DIR(). */
void
	PowerStepBuffer::goMark() {

	queueWith( PowerStep_GO_MARK ); };
