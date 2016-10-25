
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
#include <Command.h>
#include <InterpolationTimer.h>


void
	Machine::processCommand(
		char*	commandPtr,
		String* msgPtr ) {

	char* data	= commandPtr + 1;
 
	switch ( *commandPtr ) {

		// out-of-band commands
		case FEEDRATE_CONTROL :
		feedrateControl(
			data,
			msgPtr );
		break;

		case SPINDLE_CONTROL :
		spindleControl(
			data,
			msgPtr );
		break;

		case AUXILARY_CONTROL :
		auxilaryControl(
			data,
			msgPtr );
		break;

		case AXIS_CONTROL :
		axisControl(
			data,
			msgPtr );
		break;

		case HANDSHAKE_CONTROL :
		handshakeControl(
			data,
			msgPtr );
		break;

		case INTERPOLATION_CONTROL :
		interpolationControl(
			data,
			msgPtr ); 
		break;

		case TORCH_CONTROL :
		torchControl(
			data,
			msgPtr );
		break;

		case POSITION_CONTROL :
		positionControl(
			data,
			msgPtr );
		break;

		default :
		*msgPtr  +=
			*commandPtr +
			"E Invalid Machine Command - Ignored"; }; };


void
	Machine::queueCommand(
		Command* newCommand ) {

/*	DEBUG
	Serial.print( "E que " );
	Serial.println( newCommand->name() );
*/
	newCommand->nextCommand  = NULL;

	InterpolationTimer::disableInterpolationInterrupts();
		Command** commandPtrPtr	= &firstCommandInQueue;
		// Find end of queue
		while ( *commandPtrPtr )
			commandPtrPtr		= &(*commandPtrPtr)->nextCommand;
		// Add to end of queue
		*commandPtrPtr          = newCommand;

		if ( ! currentCommand )
			currentCommand		= firstCommandInQueue;
	InterpolationTimer::enableInterpolationInterrupts(); };
