
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
#include <Pause.h>
#include <InterpolationTimer.h>
#include <Axis.h>
#include <DoorAjarCommand.h>
#include <Spindle.h>
#include <AbortCommand.h>


void
	Machine::interpolationControl(
		char*	commandPtr,
		String* msgPtr ) {

	char* data	= commandPtr + 1;

	switch ( *commandPtr ) {

// in-band - acknowledge immediately
		case PAUSE_COMMAND :
		queueCommand(
			new Pause() );
		*msgPtr
			+=	String( INTERPOLATION_CONTROL )
			+	String( PAUSE_COMMAND );
		break;

// out-of-band
		case PAUSE_INTERPOLATION :
		pause();
		break;

		case CONTINUE_INTERPOLATION :
		continueInterpolation();
		break;

		case ABORT_INTERPOLATION :
		abort();
		break;

		default :
		*msgPtr
			+=	*commandPtr
			+	"E Invalid Machine Command - Ignored"; }; };


void
	Machine::abort() {

	broadcastToAxes( &Axis::abort );
	insertAtTopBG( new AbortCommand() ); };

void 
	Machine::pause() { // User pause

	insertAtTopBG( new Pause() ); };

void 
	Machine::insertAtTopBG( Command* command ) {
	// Insert in-band at top-of-queue
	InterpolationTimer::disableInterpolationInterrupts();
		insertAtTop( command );
	InterpolationTimer::enableInterpolationInterrupts(); };

void
	Machine::continueInterpolation() {

	InterpolationTimer::disableInterpolationInterrupts();
		if ( currentCommand )
			currentCommand->continueInterpolation( this );
	InterpolationTimer::enableInterpolationInterrupts(); };
