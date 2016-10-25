
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

#include <SetHallSensorPhasing.h>
#include <Machine.h>
#include <MachineCommands.h>
#include <Axis.h>
#include <DRV8305Axis.h>
#include <HallFeedback.h>
#include <Timers.h>

const char SetHallSensorPhasing::myName[] =
	"SetHallSensorPhasing";

const char*
	SetHallSensorPhasing::name() {
	
	return
		myName; };

SetHallSensorPhasing::SetHallSensorPhasing(
	char*	data ) {

	axisName		= ' ';
	setSpeed		= 10.0;	// StepsPerSecond
    sscanf( data,
		" %c %f",
		&axisName,
		&setSpeed );

	setHallState	= SetHallINIT;
	retryCount		= 0;
	success			= false; };

bool	// return true when completed
	SetHallSensorPhasing::execute(
		Machine*	machine ) {

	switch ( setHallState ) {

		case SetHallINIT :
		if ( pauseState == PauseWAIT_FOR_CONTINUATION ) {
			setHallState	= SetHallEND;	// Assume error
			axis8305 = (DRV8305Axis*)machine->findAxis(
				axisName,
				NULL );
			if (	axis8305
				&&	axis8305->is8305() ) {
		
				hallFeedback	= axis8305->hallFeedback;
				if ( hallFeedback ) {
					restart();
					return
						false; }; }; };
		break;

		case SetHallWAIT_FOR_CW :
		// 5 second timout
		if ( SysTick::microSecondTimer - startTime >= 3000000 ) {
			setHallState	= SetHallEND;	// error
			return
				false; };			

		if ( hallFeedback->initCount() < 6 )
			return
				false;

		axis8305->setTargetVelocity( -setSpeed );
		setHallState	= SetHallWAIT_FOR_CCW;
		startTime		= SysTick::microSecondTimer;
		return
			false;

		case SetHallWAIT_FOR_CCW :
		// 5 second timout
		if ( SysTick::microSecondTimer - startTime >= 3000000 ) {
			if ( retryCount++ < 1 )
				restart();
			else
				setHallState	= SetHallEND;	// error
			return
				false; };			
			
		if ( hallFeedback->initCount() < 12 )
			return
				false;

		axis8305->setTargetVelocity( 0 ); // StepsPerSecond
		setHallState	= SetHallEND;
		success			= true;
		return
			false;

		case SetHallEND :
		continueExecution = true;
		break; };

	return
		Pause::execute( machine ); };

void
	SetHallSensorPhasing::restart() {

	hallFeedback->initPhaseTransitionTable();
	axis8305->setTargetVelocity( setSpeed );
	setHallState	= SetHallWAIT_FOR_CW;
	startTime		= SysTick::microSecondTimer; };

void
	SetHallSensorPhasing::report(
		Machine*	machine,
		String*		msgPtr ) {
	
	*msgPtr	+= success ?
		" Hall sensors calibrated\n"
	:	"E Hall sensor calibration failed\n";

	char data[ 40 ];
	snprintf( data, 40,
		"%c%c%c",
		AXIS_CONTROL,
		SET_HALL_PHASING,
		axisName );
	*msgPtr  += String( data );

	hallFeedback->readHallPhasing(
		data,
		msgPtr ); };

void
	SetHallSensorPhasing::reportInterpolationStatus(
		Machine*	machine,
		String*		msgPtr ) {

	*msgPtr  += INTERPOLATOR_CHECKING_HALL_SENSORS; };
/*
SetHallSensorPhasing::~SetHallSensorPhasing() {
	
	Serial.println( "\nE delete SetHallSensorPhasing" ); };
*/
