
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
#include <SetArcVector.h>
#include <PlasmaStartStop.h>


void
	Machine::torchControl(
		char*	commandPtr,
		String* msgPtr ) {

	char* data	= commandPtr + 1;

	switch ( *commandPtr ) {

// in-band - acknowledge immediately
		case SET_ARC_VECTOR :
		queueCommand(
			new SetArcVector( data ) );
		*msgPtr
			+=	String( TORCH_CONTROL )
			+	String( SET_ARC_VECTOR );
		break;

		case START_PLASMA :
		queueCommand(
			new PlasmaStartStop( data ) );
		*msgPtr
			+=	String( TORCH_CONTROL )
			+	String( START_PLASMA );
		break;

		default :
		*msgPtr
			+=	*commandPtr
			+	"E Invalid Machine Command - Ignored"; }; };
