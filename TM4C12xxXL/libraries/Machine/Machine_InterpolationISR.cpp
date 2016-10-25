
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

#include <Machine.h>
#include <MachineCommands.h>
#include <Axis.h>
#include <Probe.h>
#include <InterpolationTimer.h>
#include <AnalogPin.h>
#include <InputPin.h>
#include <Command.h>
#include <MachineOK.h>
#include <DoorAjar.h>
#include <DoorAjarCommand.h>
#include <MainCommands.h>
#include <MySerial.h>
#include <InterpolationTimer.h>
#include <AxisTimer.h>
#include <LimitCommand.h>
#include <MachineNotOkCommand.h>
#include <OutputPin.h>
#include <StallCommand.h>


void
	Machine::interpolationISR() {	// 1.5 kHz

	if ( watchdogPin )
		watchdogPin->togglePin(); // send Watchdog signal

	Axis* axisPtr  = firstAxis;
	while ( axisPtr ) {
		axisPtr->interpolationIsr( this );
        axisPtr   = axisPtr->nextAxis; };

	checkLimits();

	while (		currentCommand
			&&	currentCommand->execute( this ) ) {

		interpolationBusy	= true; // completed
		currentCommand		= currentCommand->nextCommand; };

	broadcastToAxes( &Axis::interpolationIsrEnd ); };


void
	Machine::checkLimits() {

	bool limited = false;

	Axis* axisPtr  = firstAxis;
	while ( axisPtr ) {
        if ( axisPtr->isSwitchLimited() )
			limited = true;
        axisPtr   = axisPtr->nextAxis; };

	if ( ! limited )
		return;

	if (	currentCommand
		&&	currentCommand->isLimitCommand() )
		return;

	insertAtTop( new LimitCommand() ); };

void
	Machine::stallDetected() {

	if (	currentCommand
		&&	currentCommand->isStallCommand() )
		return;

	insertAtTop( new StallCommand() ); };

void 
	Machine::insertAtTop( Command* command ) {
	// Insert in-band at top-of-queue
Serial.print( "E insertAtTop " );
Serial.println( command->name() );

	if ( currentCommand ) // just in case command was duplicated
		currentCommand->continueInterpolation( this );
	command->nextCommand		= currentCommand;
	if ( firstCommandInQueue == currentCommand )
		firstCommandInQueue		= command;
	currentCommand				= command; };


#define	ProgressThreshold	0.9
#define	MinimumStepSize		0.0000001
#define	MaximumStepSize		0.01

void
	Machine::adjustStepSize(	// callback from interpolation execute
		StepControl* stepControl ) {

	// Don't have progress information on first call
	if ( stepControl->t ) {
		float priorStepSize = stepSize;

		// Step size controls the axes speeds
		// with the axes slowly adapting to track the position

		Progress* progress	= &stepControl->progress;
		minAndMaxProgress( progress );

		if ( sqFeedrateIPS > 0.0 ) {
			float feedRateProgress
				=	scaledTargetSpeedSquared()
				/	sqFeedrateIPS;

			// If close to feedrate speed then lock onto it 
			if ( feedRateProgress > ProgressThreshold ) {	
				stepControl->status	= 'F';
				stepSize
					*=	0.8
					+	0.2 / feedRateProgress;
				return; }; };

		if ( maxInterpolationSpeed > 0.0 ) {
			// A maximum interpolation speed was specified
			float iSpeedProgress	=
					stepSize
				/	InterpolationTimer::interpolationInterruptPeriodSecF
				/	maxInterpolationSpeed;

			if ( iSpeedProgress > ProgressThreshold ) {	
				stepControl->status	= 'T';
				stepSize
					*=	0.8
					+	0.2 / iSpeedProgress;
				return; }; };

		stepSize
			*=	0.8
			+	0.1	* MainCommands::targetLead
			+	0.2	* progress->minimum;

		if		( progress->minimum < 1.0 )
			stepControl->status		= 'M'; // Waiting on a motor

		else if	( progress->maximum > 0.9 )
			stepControl->status		= 'I';

		else
			stepControl->status		= 'C'; // All motors constant speed - try to bump speed

		if ( stepSize < MinimumStepSize ) // Normal with a very slow feedrate
			stepSize	= MinimumStepSize;

		if ( stepSize > MaximumStepSize ) // Normal with very short interpolations
			stepSize	= MaximumStepSize; };

	stepControl->t		+= stepSize; };


float
	Machine::scaledTargetSpeedSquared() { // Calculate current feedrate

	float   linearPathSpeedSQ   = 0.0;   // Use Pythagorean theorem
	float   angularPathSpeedSQ  = 0.0;
	bool	useAngularSpeed     = true;  // Assume only angular

	Axis* axisPtr   = firstAxis;
	while ( axisPtr ) {
		if ( axisPtr->isSeekingPosition() ) {
			float scaledTargetSpeedSquared     = axisPtr->scaledTargetSpeedSquared();
			if ( axisPtr->angular() && axisPtr->rotationalRadiusF == 0.0 )
				angularPathSpeedSQ	+= scaledTargetSpeedSquared;

			else {
				linearPathSpeedSQ	+= scaledTargetSpeedSquared;
				useAngularSpeed		= false; }; };

		axisPtr					= axisPtr->nextAxis; };
 
	return useAngularSpeed ?
		angularPathSpeedSQ :
		linearPathSpeedSQ; };


void
	Machine::minAndMaxProgress(
		Progress* progress ) { // Check progress on current interpolation

	progress->minimum	= 2.0;
	progress->maximum	= 0.0;

	Axis* axisPtr  = firstAxis;
	while ( axisPtr ) {
		axisPtr->minAndMaxProgress( progress );
        axisPtr   = axisPtr->nextAxis; }; };


