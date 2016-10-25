
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

#include <SetFeedrate.h>
#include <Machine.h>
#include <MachineCommands.h>

const char SetFeedrate::myName[] =
	"SetFeedrate";

const char*
	SetFeedrate::name() {

	return
		myName; };

SetFeedrate::SetFeedrate( char* data ) {
    sscanf( data, "%f", &feedrate ); };

bool
	SetFeedrate::execute( Machine* machine ) {

	machine->sqFeedrateIPS	= feedrate * feedrate;

    return true; };

void
	SetFeedrate::report(  Machine* machine, String* msgPtr ) {

    char data[40];
    snprintf( data, 40,
		"%c%c%0.5f\n",
		FEEDRATE_CONTROL,
		FEEDRATE_CHANGED,
		feedrate );
    *msgPtr  += String( data ); };
