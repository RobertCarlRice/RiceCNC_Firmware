
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

#include "MachineSplineAxis.h"


// Initializer
MachineSplineAxis::MachineSplineAxis(
	char*		data,
	Axis*		firstAxisPtr,
	String*		msgPtr ) {

	nextSplineAxis	= NULL;
	machineType     = Invalid; // Assume invalid
	currentPoint	= 0.0;
  
	char            axisNameC;
	p4              = 0.0;
	int parsed      = sscanf( data,
		"%c%f",
		&axisNameC,
		&p4 );		// p4 is an offset in micro-steps

	// Find the axis
	if ( axisNameC >= 97 )
		axisNameC -= 32; // x->X, y->Y, z->Z, a->A, b->B, c->C

	axis            = firstAxisPtr;
	while (		axis
			&&	axis->axisNameC != axisNameC )
		axis = axis->nextAxis;

	if		( axis == NULL )
		*msgPtr		+= "\nE MachineSpline axis not Configured!";

	else if ( parsed == 2 )
		machineType		= Linear;

	else {
		snprintf( data, 40,
			"\nE %s Invalid",
			data );
		*msgPtr		+= String( data ); }; };

bool
	MachineSplineAxis::valid() {
	
	return
		machineType != Invalid; };

void
	MachineSplineAxis::calculateTarget( float t ) {

	float microstepTarget	= p4 * t;
	axis->interpolationMicrostepBuffer
		+=	microstepTarget
		-	currentPoint;
	currentPoint			= microstepTarget; };
