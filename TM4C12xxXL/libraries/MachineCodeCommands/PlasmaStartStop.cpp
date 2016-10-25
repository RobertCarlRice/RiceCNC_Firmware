
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

#include <PlasmaStartStop.h>
#include <Axis.h>
#include <TorchHeightControl.h>
#include <Machine.h>

const char PlasmaStartStop::myName[] =
	"PlasmaStartStop";

const char*
	PlasmaStartStop::name() {
	
	return
		myName; };

PlasmaStartStop::PlasmaStartStop( char* data ) {
	axis			= NULL;

	axisNameC		= ' ';
	int	number		= 0;
	setPoint		= 0.0;
	sscanf( data,
		"%c%d",
		&axisNameC,
		&number,
		&setPoint );
	on				= number > 0; };

bool
	PlasmaStartStop::execute( Machine* machine ) {

	if ( ! axis ) {
		axis		= (TorchHeightControl*)machine->findAxis(
			axisNameC,
			NULL );
		if ( setPoint != 0.0 && axis && axis->isTorchAxis() ) {
			axis->setVoltage	= setPoint;
			axis->startPlasma( on ); }

		else {
			axis	= NULL;
			return true; }; };

	bool established = axis->arcEstablished();
    return on ?
		established :
		! established; };

void
	PlasmaStartStop::report( String* msgPtr ) {

	char data[40] = "";
	if ( axis )
		snprintf( data, 40,
			" Plasma %c %s\n",
			axisNameC,
			on ?
				"Started" :
				"Stopped" );
	else
		snprintf( data, 40,
			"E Plasma axis %c invalid\n",
			axisNameC );

	*msgPtr
		+= String( data ); };
