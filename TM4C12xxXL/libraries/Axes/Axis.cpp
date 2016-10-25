
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

#include <Axis.h>
#include <AxisTimer.h>
#include <MainCommands.h>
#include <MachineCommands.h>

// These includes are needed for the compiler to call the virtual destructor
#include <LimitSwitch.h>
#include <QuadratureCounter4X.h>
#include <ClosedLoopCounter.h>

// derived classes
#include <DiscreteAxis.h>
#include <QuadratureAxis.h>
#include <StepDirectionAxis.h>
#include <ThreePhaseAxis.h>
#include <TorchHeightControl.h>
#include <DRV8711Axis.h>
#include <dSpinAxis.h>

Axis*	Axis::firstMotorStepAxis	= NULL;;

void
	Axis::addMotorStepInterrupt( Axis* thisAxis ) {
	
	// prevent duplicates
	Axis* motorStepAxis = Axis::firstMotorStepAxis;
	while( motorStepAxis ) {
		if ( motorStepAxis == thisAxis )
			return;	// already added
		motorStepAxis = motorStepAxis->nextMotorStepAxis; };
	
	// Add as start of chain
	thisAxis->nextMotorStepAxis	= Axis::firstMotorStepAxis;
	Axis::firstMotorStepAxis	= thisAxis; };

void
	Axis::removeMotorStepInterrupt( Axis* thisAxis ) {
	
	Axis** motorStepAxisPtr = &Axis::firstMotorStepAxis;
	while( *motorStepAxisPtr ) {
		Axis* motorStepAxis = *motorStepAxisPtr;
		if ( motorStepAxis == thisAxis ) {
			*motorStepAxisPtr	= thisAxis->nextMotorStepAxis;
			return; };
		motorStepAxisPtr = &motorStepAxis->nextMotorStepAxis; }; };

void
	Axis::motorStepInterrupt() { // every 1 µS
	
	Axis* motorStepAxis = Axis::firstMotorStepAxis;
	while( motorStepAxis ) {
		motorStepAxis->motorStepIsr();
		motorStepAxis = motorStepAxis->nextMotorStepAxis; }; };

Axis::Axis(
	char*		data,
	String*		msgPtr ) {

	stepsPerUnitDistanceF		= 0.0;
	maxStepsPerSecU				= 0;

    char	interfaceType;

	accelStepsPerSecPerSecU		= 0;
	decelStepsPerSecPerSecU		= 0;
	
	parseLength					= 0;
	sscanf( data,
		"%c%c%c %lu %f %lu %lu %lu%ln",
		&axisNameC,
		&axisFunctionC,
		&interfaceType,

		// Motor dynamics
		&microStepU,
		&stepsPerUnitDistanceF,
		&accelStepsPerSecPerSecU,
		&decelStepsPerSecPerSecU,
		&maxStepsPerSecU,

		&parseLength );
	
	// Efficiency calculations
	maxStepsPerSecF	=
		float( maxStepsPerSecU );

	accelStepsPerSecPerMicroSecF =
			float( accelStepsPerSecPerSecU )
		*	0.000001;

	decelStepsPerSecPerSecF	=
		float( decelStepsPerSecPerSecU );

	decelStepsPerSecPerMicroSecF	=
			decelStepsPerSecPerSecF
		*	0.000001;

	decelTime_SecPerStepsPerSecF	=
			1.0
		/	decelStepsPerSecPerSecF;

	setMicroStep();

	if ( axisNameC >= 97 )
		axisNameC -= 32; // x->X, y->Y, z->Z, a->A, b->B, c->C

	flags			= VELOCITY_CHANGED;

	switch ( axisFunctionC ) {
		case 'P' :	// Process control axis (no motor)
		case 'L' :	// Linear
		break;
	
		// For rotating plasma head
		case 'R' :	// Torch XY rotation axis
		case 'T' :	// Torch rotating tilt axis

		// For gimbal plasma head
		case 'X' :	// Torch head X tilt gimbal
		case 'Y' :	// Torch head Y tilt gimbal

		case 'A' :	// Angular motor axis
		flags |= ANGULAR; };

	setAxisState( IDLE );
	motorState				= DISABLED;

	// dynamics
	indexerOffsetF			= 0.0;
	mvStepsPerSecF			= 0.0;

	uStepProgramPositionI	= 0;
	uStepProgramPositionF	= 0.0;

	microStepTargetOffsetI	= 0;
	microStepTargetOffsetF	= 0.0;

	uStepMachinePositionI			= 0;
	uStepMachinePositionF			= 0.0;
	interpolationMicrostepBuffer	= 0.0;

	targetStepsPerSecondF	= 0.0;

	rotationalRadiusF		= 0.0;

	stepProgressF			= 2.0;

	limitSwHighPin			= NULL;
	limitSwLowPin			= NULL;

	nextAxis				= NULL;

	firstSuperiorLink		= NULL;
	firstSubordinateLink	= NULL;

	char echo[40];
	snprintf( echo, 40,
		" Axis %c %s %c ",
		axisNameC,
		angular() ?
			"Angular" :
			"Linear",
		interfaceType );
	*msgPtr  += String( echo );

	snprintf( echo, 40,
		"\n microstep 1/%lu",
		microStepsPerStepU );
	*msgPtr    += String( echo );

	snprintf( echo, 40,
		"\n SPUD %.1f",
		stepsPerUnitDistanceF );
	*msgPtr  += String( echo );

	snprintf( echo, 40,
		"\n accel %lu SPSPS",
		accelStepsPerSecPerSecU );
	*msgPtr  += String( echo );

	snprintf( echo, 40,
		"\n decel %lu SPSPS",
		decelStepsPerSecPerSecU );
	*msgPtr  += String( echo );

	snprintf( echo, 40,
		"\n max spd %lu SPS\n\n",
		maxStepsPerSecU );
	*msgPtr  += String( echo );

	limitSwHighPin			= NULL;
	limitSwLowPin			= NULL;

	slaveStepCounter		= NULL;
	closedLoopStepCounter	= NULL; };


void
	Axis::updateAxis(
		char*		data,
		String*		msgPtr ) {

	*msgPtr		+= " Can't update axis\n"; };

void
	Axis::addMotor(
		char*		data,
		String*		msgPtr ) {
	
	*msgPtr		+= " Can't add motor"; };

Axis::~Axis() { // Called automatically after derived class destructor

	Axis::removeMotorStepInterrupt( this );

	if ( limitSwHighPin )
		delete limitSwHighPin;

	if ( limitSwLowPin )
		delete limitSwLowPin;

	if ( slaveStepCounter )
		delete slaveStepCounter;

	if ( closedLoopStepCounter )
		delete closedLoopStepCounter;

	deleteLinks(); };

void
	Axis::setMicroStep() {
	
	if ( microStepU <= 8 )
		microStepsPerStepU	= 1 << microStepU;
	
	else {
		microStepsPerStepU	= microStepU;

		microStepU	= 0;
		while( ( 1 << microStepU ) < microStepsPerStepU )
			microStepU++; };

	// efficiency pre-calculations
	microStepsPerStepF			= float( microStepsPerStepU );

	microStepsPerUnitDistanceF	=
		stepsPerUnitDistanceF
	*	microStepsPerStepF;

	updateTimerInterval(); };

// virtual methods
bool
	Axis::isTorchAxis() {

	return
		false; };

bool
	Axis::isConfigured() {

	return
		false; };

bool
	Axis::usesSSI( SSI* ssi ) {

	return
		false; };

bool
	Axis::is8305() {

	return
		false; };
// end virtual

bool
	Axis::angular() {

	return
		flags & ANGULAR; };

bool
	Axis::velocityChanged() {

	return
		flags & VELOCITY_CHANGED; };

bool
	Axis::statusChanged() {

	return
		flags & STATUS_CHANGED; };

bool
	Axis::setTargetVelocityWithData(
		char*		data,
		String*		msgPtr ) { // Overridden by dSpinAxis

	if ( isSeekingPosition() ) {
		snprintf( data, 40,
			" %c axis interpolating",
			axisNameC ); // ignore error
		*msgPtr        += String( data );
		return
			false; };

	float newTargetStepsPerSecondF = 0.0;
	AxisTimer::disableAxisInterrupts();
		sscanf( data,
			"%f",
			&newTargetStepsPerSecondF );

		if		( newTargetStepsPerSecondF > maxStepsPerSecF )
			newTargetStepsPerSecondF = maxStepsPerSecF;

		else if ( newTargetStepsPerSecondF < -maxStepsPerSecF )
			newTargetStepsPerSecondF = -maxStepsPerSecF;

		runMotor( newTargetStepsPerSecondF );
		setAxisState( SEEKING_VELOCITY );
	AxisTimer::enableAxisInterrupts();

	snprintf( data, 40,
		" %c target velocity %0.3f",
		axisNameC,
		targetStepsPerSecondF );
	*msgPtr		+= String( data );

	return
		true; };

bool
	Axis::setTargetVelocity(
		float		newTargetStepsPerSecondF ) {

	if ( isSeekingPosition() ) {
		return
			false; };

	AxisTimer::disableAxisInterrupts();
		if		( newTargetStepsPerSecondF > maxStepsPerSecF )
			newTargetStepsPerSecondF = maxStepsPerSecF;

		else if ( newTargetStepsPerSecondF < -maxStepsPerSecF )
			newTargetStepsPerSecondF = -maxStepsPerSecF;

		runMotor( newTargetStepsPerSecondF );
		setAxisState( SEEKING_VELOCITY );
	AxisTimer::enableAxisInterrupts();

	return
		true; };

void
	Axis::setFlag( uint32_t flag ) {

	AxisTimer::disableAxisInterrupts();
		flags	|= flag;
	AxisTimer::enableAxisInterrupts();	};

void
	Axis::clearFlag( uint32_t flag ) {

	AxisTimer::disableAxisInterrupts();
		flags	&= ~flag;
	AxisTimer::enableAxisInterrupts();	};

void // overridden by dSpin
	Axis::reportAxisStatus( String* msgPtr ) {

	if ( ! anyChange() )
		return;

	clearFlag( STATUS_CHANGED );
	char data[ 80 ];
	
	snprintf( data, 80,
		"%c%c%c%c",
		STATUS_REPORT,
		axisNameC,
		axisStateChar(),
		motorStateChar() );

	*msgPtr  += String( data );

	if ( flags & VELOCITY_CHANGED ) {
		clearFlag( VELOCITY_CHANGED );

		AxisTimer::disableAxisInterrupts();
			float  mVelocity = mvStepsPerSecF;
		AxisTimer::enableAxisInterrupts();

		snprintf( data, 80,
			" %c%.3g",
			VELOCITY,
			mVelocity );
		*msgPtr  += String( data ); };
	
	AxisTimer::disableAxisInterrupts();

		int32_t currentMsMachinePositionI = uStepMachinePositionI;
		uStepMachinePositionI	= 0;

		int32_t currentMsProgramPositionI = uStepProgramPositionI;
		uStepProgramPositionI	= 0;

	AxisTimer::enableAxisInterrupts();

	if ( currentMsMachinePositionI ) {
		snprintf( data, 80,
			" %c%d",
			MACHINE_POSITION_CHANGE,
			currentMsMachinePositionI );
		*msgPtr  += String( data ); };
	
	if ( currentMsProgramPositionI ) {
		snprintf( data, 80,
			" %c%d",
			PROGRAM_POSITION_CHANGE,
			currentMsProgramPositionI );
		*msgPtr  += String( data ); }; };

bool
	Axis::anyChange() {

	return
		flags & ( STATUS_CHANGED | VELOCITY_CHANGED )
	||	uStepMachinePositionI
	||	uStepProgramPositionI; };

char
	Axis::axisStateChar() {

	switch ( axisState ) {

		case UNCONFIGURED:
		return AXIS_UNCONFIGURED;

		case IDLE:
		return AXIS_IDLE;

		case SEEKING_POSITION:
		return AXIS_SEEKING_POSITION;

		case SEEKING_VELOCITY:
		return AXIS_SEEKING_VELOCITY;

		case SWITCH_LIMIT_HIGH:
		return AXIS_SWITCH_LIMITED_HIGH;

		case SWITCH_LIMIT_LOW:
		return AXIS_SWITCH_LIMITED_LOW;
		
		case PROGRAM_FAULT:
		return AXIS_PROGRAM_FAULT; };

	return 'E'; };

char
	Axis::motorStateChar() {
	
	switch ( motorState ) {
		case HOLDING:
		return MOTOR_HOLDING;

		case ACCELERATING:
		return MOTOR_ACCELERATING;

		case DECELERATING:
		return MOTOR_DECELERATING;

		case CONSTANT_SPEED:
		return MOTOR_CONSTANT_SPEED; };
	
	return 'E'; };

void
	Axis::reportAnalog( String* msgPtr ) {};

int32_t
	Axis::roundToInt( float value ) {

	return
		(int32_t) floor( value ) + 0.5; };

void
	Axis::addMotorStepInterrupt() {

	Axis::addMotorStepInterrupt( this ); };
