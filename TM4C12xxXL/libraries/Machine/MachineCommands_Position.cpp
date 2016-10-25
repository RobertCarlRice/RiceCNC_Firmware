
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

#include <MachineCommands.h>
#include <Machine.h>
#include <AdjustProgramPosition.h>
#include <Axis.h>
#include <SplineCommand.h>
#include <ProbingSplineCommand.h>
#include <SlaveModeCommand.h>
#include <MachineSplineCommand.h>
#include <InterpolationTimer.h>

void
	Machine::positionControl(
		char*		commandPtr,
		String*		msgPtr ) {

	char* data	= commandPtr + 1;

	switch ( *commandPtr ) {
	
// in-band - acknowledge immediately
		case ADJUST_PROGRAM_POSITION: // not used
		queueCommand(
			new AdjustProgramPosition( data ) );
		*msgPtr
			+=	String( POSITION_CONTROL )
			+	String( ADJUST_PROGRAM_POSITION );
		break;

// out-of-band
		case ENTER_SLAVE_MODE :
		enterSlaveMode(
			data,
			msgPtr );
		break;

		case CREATE_SPLINE :
		createSpline(
			data,
			msgPtr );
		break;

		case CREATE_PROBING_SPLINE :
		createProbingSpline(
			data,
			msgPtr );
		break;

		case TRACK_PAD_JOG :
		trackPadJog(
			data,
			msgPtr );
		break;

	// Move in machine space
		case TRAVERSE_MACHINE :
		createMachineSpline(
			data,
			msgPtr ); 
		break;

		case TRACK_PAD_JOG_MACHINE :
		trackPadJogMachine(
			data,
			msgPtr );
		break;

		case SET_AXIS_VELOCITY :
		setVelocity(
			data,
			msgPtr );
		break;

		default :
		*msgPtr
			+=	*commandPtr
			+	"E Invalid Machine Command - Ignored"; }; };


void
	Machine::setVelocity(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( POSITION_CONTROL )
		+	String( SET_AXIS_VELOCITY );

	Axis* axisPtr = findAxis(
		*data,
		msgPtr );
	if ( axisPtr )
		axisPtr->setTargetVelocityWithData(
			data + 1,
			msgPtr ); };


void
	Machine::enterSlaveMode(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( POSITION_CONTROL )
		+	String( ENTER_SLAVE_MODE );

	queueCommand(
		new SlaveModeCommand( data, firstAxis, msgPtr ) ); };


void
	Machine::createSpline(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( POSITION_CONTROL )
		+	String( CREATE_SPLINE );

	queueCommand(
		new Spline( data, firstAxis, msgPtr ) ); };

void
	Machine::createProbingSpline(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( POSITION_CONTROL )
		+	String( CREATE_PROBING_SPLINE );

	queueCommand(
		new ProbingSplineCommand( data, firstAxis, msgPtr ) ); };

void
	Machine::createMachineSpline( char* data, String* msgPtr ) {

	*msgPtr
		+=	String( POSITION_CONTROL )
		+	String( TRAVERSE_MACHINE );

	queueCommand(
		new MachineSpline( data, firstAxis, msgPtr ) ); };


void
	Machine::trackPadJog(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( POSITION_CONTROL )
		+	String( TRACK_PAD_JOG );

	// Same as createSpline but abort current interpolation
	InterpolationTimer::disableInterpolationInterrupts();
		if ( currentCommand )
			currentCommand->trackPadJogAbort( this );
	InterpolationTimer::enableInterpolationInterrupts();

	queueCommand( new Spline( data, firstAxis, msgPtr ) ); };


void
	Machine::trackPadJogMachine(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( POSITION_CONTROL )
		+	String( TRACK_PAD_JOG_MACHINE );

	// Same as createMachineSpline but abort current interpolation
	InterpolationTimer::disableInterpolationInterrupts();
		if ( currentCommand )
			currentCommand->trackPadJogAbort( this );
	InterpolationTimer::enableInterpolationInterrupts();

	queueCommand( new MachineSpline( data, firstAxis, msgPtr ) ); };
