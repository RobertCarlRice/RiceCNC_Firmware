
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
#include <Gpio.h>
#include <MainCommands.h>
#include <InputPin.h>
#include <StepCounter.h>
#include <ClosedLoopCounter.h>


//	All methods defined here are call with axis interrupts disabled

void	// axis interrupts disabled
	Axis::updateVelocity() { // Called from derived axis at axis interrupt 15 kHz

	/* Check for stop condition.
	This code stops motor faster to prevent overruns and oscillations.
	The PROGRAM_FAULT state persists only if not set to IDLE by interpolation
	*/
	if ( axisState == SEEKING_POSITION ) {
		if (	(	targetStepsPerSecondF > 0.0
				&&	microStepTargetOffsetI <= 0 )
			||	(	targetStepsPerSecondF < 0.0
				&&	microStepTargetOffsetI >= 0 ) ) {

			// Overrun target position
			setAxisState( PROGRAM_FAULT );
			targetStepsPerSecondF	= 0.0; }; }

	// TODO: Make sure motor remains stopped
	else if ( axisState == PROGRAM_FAULT )
		targetStepsPerSecondF	= 0.0;

	// Update motor velocity
	switch ( motorState ) {

		case DISABLED:
		return;

		case ACCELERATING:

		if		( mvStepsPerSecF < targetStepsPerSecondF ) {
			flags  |= VELOCITY_CHANGED;
			mvStepsPerSecF	+= accelSpeedStepSizeF;
			if ( mvStepsPerSecF >= targetStepsPerSecondF ) {
				mvStepsPerSecF	= targetStepsPerSecondF;
				setMotorState(); }; }

		else if ( mvStepsPerSecF > targetStepsPerSecondF ) {
			flags  |= VELOCITY_CHANGED;
			mvStepsPerSecF	-= accelSpeedStepSizeF;
			if ( mvStepsPerSecF <= targetStepsPerSecondF ) {
				mvStepsPerSecF	= targetStepsPerSecondF;
				setMotorState(); }; }

		else
			setMotorState();

		break;

		case DECELERATING: // Target velocity is zero

		if		( mvStepsPerSecF < targetStepsPerSecondF ) {
			flags  |= VELOCITY_CHANGED;
			mvStepsPerSecF	+= decelSpeedStepSizeF;
			if ( mvStepsPerSecF > targetStepsPerSecondF ) {
				mvStepsPerSecF = targetStepsPerSecondF;
				setMotorState(); }; }

		else if ( mvStepsPerSecF > targetStepsPerSecondF ) {
			flags  |= VELOCITY_CHANGED;
			mvStepsPerSecF	-= decelSpeedStepSizeF;
			if ( mvStepsPerSecF < targetStepsPerSecondF ) {
				mvStepsPerSecF = targetStepsPerSecondF;
				setMotorState(); }; }

		else
			setMotorState(); };

	// update predicted armature phase error (angle between coils and armature)
	indexerOffsetF	+= mvStepsPerSecF * indexerStepSize; };

void	// axis interrupts disabled
	Axis::stepped( float microStepsF ) {

	indexerOffsetF		-= microStepsF;

	moveMachinePositionAndTarget( microStepsF ); };

void	// axis interrupts disabled
	Axis::moveMachinePositionAndTarget( float microStepsF ) {
	
	// Record closed loop steps for adjustment and stall detection
	if ( closedLoopStepCounter )
		closedLoopStepCounter->stepped += microStepsF;

	moveMachinePosition( microStepsF );

	if ( axisState == SEEKING_POSITION )
		addToMicroStepTarget( -microStepsF ); };	

void	// axis interrupts disabled
	Axis::addToMicroStepTarget( float microStepsOffsetF ) {

	microStepTargetOffsetF	+= microStepsOffsetF;
	
	// carry the integer part to not overflow 23 bit float precision
	int32_t microsteps		= roundToInt( microStepTargetOffsetF );
	if ( microsteps ) {		// maintain within a half microstep
		microStepTargetOffsetI	+= microsteps;
		microStepTargetOffsetF	-= microsteps; }; };

void	// axis interrupts disabled
	Axis::updateTimerInterval() {

	indexerStepSize		=	// microsteps per second
		AxisTimer::axisInterruptPerioduSF	// µSec
	*	microStepsPerStepF
	*	0.000001;

	accelSpeedStepSizeF	=
		AxisTimer::axisInterruptPerioduSF
	*	accelStepsPerSecPerMicroSecF;

	decelSpeedStepSizeF	=
		AxisTimer::axisInterruptPerioduSF
	*	decelStepsPerSecPerMicroSecF; };

void	// axis interrupts disabled
	Axis::runMotor( float stepsPerSecondF ) {

	targetStepsPerSecondF	= stepsPerSecondF;
	setMotorState(); };

void	// axis interrupts disabled
	Axis::stopMotor() {

	targetStepsPerSecondF	= 0.0;
	setMotorState(); };


void	// Axis interrupt disabled
	Axis::setAxisState( AxisState newState ) {

	if ( axisState != newState ) {
		axisState	= newState;
		flags		|= STATUS_CHANGED; }; };


void	// Axis interrupt disabled
	Axis::setMotorState() {

	MotorState newState;

	if		( targetStepsPerSecondF == mvStepsPerSecF )
		newState	= mvStepsPerSecF ?
			CONSTANT_SPEED :
			HOLDING;

	else if ( mvStepsPerSecF > 0.0 )
		newState	= targetStepsPerSecondF > mvStepsPerSecF ?
			ACCELERATING :
			DECELERATING;

	else if ( mvStepsPerSecF < 0.0 )
		newState	= targetStepsPerSecondF < mvStepsPerSecF ?
			ACCELERATING :
			DECELERATING;

	else	// mvStepsPerSecF == 0.0
		newState	= ACCELERATING;

	if ( motorState != newState ) {
		motorState	= newState;
		flags		|= STATUS_CHANGED;
		motorStateChanged(); }; };
