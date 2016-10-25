
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
#include <Gpio.h>
#include <InterpolationTimer.h>
#include <Machine.h>
#include <InputPin.h>


// All of these methods are called with the interpolation interrupt disabled

bool
	Axis::isSeekingPosition() {

	return
		axisState	== SEEKING_POSITION ; };

void
	Axis::abort() { // Stop if seeking velocity
	// stop as fast as possible
	microStepTargetOffsetI	=
		roundToInt( microStepStopDisplacement() );
	stopMotor(); };


void
	Axis::interpolationIsrEnd() {	// called once per interpolation interrupt

	// Has interpolation moved the targert position for this axis?
	if		( interpolationMicrostepBuffer )
		interpolating();

	else if (	axisState == SEEKING_POSITION
			||	axisState == PROGRAM_FAULT ) {

		stepProgressF	= 2.0;	// Don't wait on this axis
	
		if ( motorState == HOLDING ) {
			setAxisState( IDLE );
			return; }

		else	// Stopping
			calculateStopVelocity(); }	// update targetStepsPerSecondF

	else	// idle or velocity mode
		return;

	AxisTimer::disableAxisInterrupts();
		setMotorState();
	AxisTimer::enableAxisInterrupts(); };

void
	Axis::interpolating() {

	adjustMicroStepTarget( interpolationMicrostepBuffer );

	calculateStopVelocity();	// update targetStepsPerSecondF

	if ( targetStepsPerSecondF ) {
	
		if		( targetStepsPerSecondF > maxStepsPerSecF ) {
			targetStepsPerSecondF	= maxStepsPerSecF;
			stepProgressF	= 0.1; }	// Too fast - slow interpolation

		else if	( targetStepsPerSecondF < -maxStepsPerSecF ) {
			targetStepsPerSecondF	= -maxStepsPerSecF;
			stepProgressF	= 0.1; }

		else {
			float microStepsPerInterruptPeriodSecF =
					InterpolationTimer::interpolationInterruptPeriodSecF
				*	microStepsPerStepF;

			// Calculate the axis speed intended by the interpolation
			float interpolationStepsPerSecF =
					interpolationMicrostepBuffer
				/	microStepsPerInterruptPeriodSecF;

			// Calculate progress by ratio of speed rather than distance
			stepProgressF	=
					targetStepsPerSecondF
				/	interpolationStepsPerSecF;

			// Feed forward a small amount of the intended speed to
			// prevent motor surging
			targetStepsPerSecondF	=
					targetStepsPerSecondF		* 0.99	// mostly use stop velocity
				+	interpolationStepsPerSecF	* 0.01; }; }

		else
			stepProgressF	= 0.0;	// used to adjust interpolation step size

	interpolationMicrostepBuffer	= 0.0; };

void
	Axis::minAndMaxProgress( Progress* progress ) { 

	// stepProgressF is zero if movement was less than a microstep
	if ( isSeekingPosition() && stepProgressF ) {

		if ( stepProgressF < progress->minimum )
			progress->minimum	= stepProgressF;

		if ( stepProgressF > progress->maximum )
			progress->maximum	= stepProgressF; }; };

void
	Axis::calculateStopVelocity() {
	// I tried lots of algorithms to avoid using sqrt() but all were unstable so I gave up
	// but then I learned that the FPU implements a fast sqrt so no speed problem

	if ( microStepTargetOffsetI ) {
		// Calculate a target velocity to easily stop before the target offset
		// Conservative by 50%
		targetStepsPerSecondF	= sqrt(
				float( labs( microStepTargetOffsetI ) )
			/	microStepsPerStepF
			*	decelStepsPerSecPerSecF );

		if	( microStepTargetOffsetI < 0 )
			targetStepsPerSecondF	= -targetStepsPerSecondF; }

	else
		targetStepsPerSecondF	= 0.0;	// optimization
	};


float	// used for feedrate calculation
	Axis::scaledTargetSpeedSquared() {	// target velocity to IPS squared
	
	if ( targetStepsPerSecondF == 0.0 )
		return
			0.0;

	float unitDistancePerSec	=
			targetStepsPerSecondF
		/	stepsPerUnitDistanceF;

	if ( angular() && rotationalRadiusF )
		unitDistancePerSec		*= rotationalRadiusF; // convert from degrees to inchs

	return 
			unitDistancePerSec
		*	unitDistancePerSec; };


// Used only for aborting motor movement
float
	Axis::microStepStopDisplacement() {	// velocity to signed µStep stopping distance

	if ( mvStepsPerSecF == 0.0 )
		return
			0.0;

	float avgVel		=
			mvStepsPerSecF
		*	microStepsPerStepF
		/	2.0; // µsps

	float stopTime	=
			fabs( mvStepsPerSecF )
		*	decelTime_SecPerStepsPerSecF; // seconds

	return
			avgVel
		*	stopTime; };


void
	Axis::adjustMicroStepTarget( float microstepTargetOffsetF ) {

	AxisTimer::disableAxisInterrupts();
		if ( axisState == SEEKING_VELOCITY ) { // Abort velocity mode at stop position
			microStepTargetOffsetI	=
				roundToInt( microStepStopDisplacement() ); };

		addToMicroStepTarget( microstepTargetOffsetF );
		setAxisState( SEEKING_POSITION );
	AxisTimer::enableAxisInterrupts(); };


void	// called on fixture or tool change
	Axis::adjustMachinePosition( float offset ) {

	// adjust machine position without running motor
	float scaledOffset		= offset * microStepsPerUnitDistanceF;

	AxisTimer::disableAxisInterrupts();
		moveMachinePosition( scaledOffset );
	AxisTimer::enableAxisInterrupts(); };

void
	Axis::moveMachinePosition( float scaledOffset ) {

	uStepMachinePositionF	+= scaledOffset;
	// extend to double precision
	int32_t microSteps		= roundToInt( uStepMachinePositionF );
	uStepMachinePositionI	+= microSteps;
	uStepMachinePositionF	-= microSteps; };
