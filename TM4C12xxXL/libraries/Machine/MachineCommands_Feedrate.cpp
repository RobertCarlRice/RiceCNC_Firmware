
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
#include <SetFeedrate.h>
#include <SetMaxInterpolationSpeed.h>
#include <Dwell.h>


void
	Machine::feedrateControl(
		char*	commandPtr,
		String* msgPtr ) {

	char* data	= commandPtr + 1;

	switch ( *commandPtr ) {

// in-band - acknowledge immediately
		case SET_FEEDRATE :
		queueCommand(
			new SetFeedrate( data ) );
		*msgPtr
			+=	String( FEEDRATE_CONTROL )
			+	String( SET_FEEDRATE );
		break;

		case MAX_INTERPOLATION_SPEED :
		queueCommand(
			new SetMaxInterpolationSpeed( data ) );
		*msgPtr
			+=	String( FEEDRATE_CONTROL )
			+	String( MAX_INTERPOLATION_SPEED );
		break;

		case DWELL :
		queueCommand(
			new Dwell( data ) );
		*msgPtr
			+=	String( FEEDRATE_CONTROL )
			+	String( DWELL );
		break;

// out-of-band
		case FEEDRATE_OVERRIDE :
		feedrateOverride(
			data,
			msgPtr );
		break;

		default :
		*msgPtr
			+=	*commandPtr
			+	"E Invalid Machine Command - Ignored"; }; };


void
	Machine::feedrateOverride(
		char*	data,
		String* msgPtr ) {

    float       feedrateIPS;            // inches per second
	sscanf( data,
		" %f",
		&feedrateIPS );

	snprintf( data, 40,
		"%c%c %.3f",
		FEEDRATE_CONTROL,
		FEEDRATE_OVERRIDE,
		feedrateIPS );
	*msgPtr  += String( data );

	// Only keep the square of the feedrate to optimize Pythagorean speed comparison
	sqFeedrateIPS	= feedrateIPS * feedrateIPS; };
