
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

#include <SetCoolant.h>
#include <Machine.h>
#include <Coolants.h>
#include <MachineCommands.h>

const char SetCoolant::myName[] =
	"SetCoolant";

const char*
	SetCoolant::name() {

	return
		myName; };

SetCoolant::SetCoolant( char* data ) {
	int    number;
	sscanf( data,
		"%d",
		&number );

	outputNumber  = abs( number ) - 1;
	on            = number >= 0;
	coolant       = NULL; };

bool
	SetCoolant::execute(
		Machine* machine ) {

    if (	outputNumber >= 0
		&&	outputNumber < AuxOutputs )
		coolant   = machine->coolants[ outputNumber ];

    if ( coolant )
		coolant->setCoolantOn( on );

    return
		true; };
  
void
	SetCoolant::report(
		Machine*	machine,
		String*		msgPtr ) {

    if ( coolant ) {
		char data[40];
		snprintf( data, 40,
			"%c%c%d %s\n",
			AUXILARY_CONTROL,
			COOLANT_ON_OFF,
			coolant->outputNumber,
			on ?
				"On"
			:	"Off" );
		*msgPtr  += String( data ); }; };
