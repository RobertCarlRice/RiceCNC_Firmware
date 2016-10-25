
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

#include <SetArcVector.h>
#include <Axis.h>
#include <TorchHeightControl.h>
#include <Machine.h>

const char SetArcVector::myName[] =
	"SetArcVector";

const char*
	SetArcVector::name() {

	return
		myName; };

SetArcVector::SetArcVector( char* data ) {
	axisNameC				= ' ';
	
	xyArcLength				= 0.0;
	headRotationAngleRad	= 0.0;
	zArcOffset				= 0.0;
	
	sscanf( data+3,
		"%c %f %f %f %f %f",
		&axisNameC,
		&xyArcLength,
		&headRotationAngleRad,
		&zArcOffset );

	headRotationAngleRad	*= M_PI / 180.0; };

bool
	SetArcVector::execute( Machine* machine ) {

	TorchHeightControl*	axis	=
		(TorchHeightControl*)machine->findAxis(
			axisNameC,
			NULL );

	if ( axis )
		axis->setArcVector(
			xyArcLength,
			headRotationAngleRad,
			zArcOffset );

	return
		true; };

void
	SetArcVector::report( String* msgPtr ) {

	char data[40] = "";
	if ( axis )
		snprintf( data, 40,
			" Plasma %c arc vector set\n",
			axisNameC );
	else
		snprintf( data, 40,
			"E Plasma axis %c invalid\n",
			axisNameC );

	*msgPtr += String( data ); };
