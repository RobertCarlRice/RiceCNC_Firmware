
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

#include <SplineAxis.h>
#include <Axis.h>

// Initializer
SplineAxis::SplineAxis(
	char*		data,
	Axis*		firstAxisPtr,
	String*		msgPtr ) {

	nextSplineAxis  = NULL;
	type            = Invalid; // Assume invalid
  	currentPoint	= 0.0;

	char            axisNameC;
	p2 = p3 = p4    = 0.0;
	int parsed      = sscanf( data,
		"%c%f,%f,%f",
		&axisNameC,
		&p2,
		&p3,
		&p4 );
  
	// Find the axis
	if ( axisNameC >= 97 )
		axisNameC	-= 32; // x->X, y->Y, z->Z, a->A, b->B, c->C

	axis		= firstAxisPtr;
	while (		axis
			&&	axis->axisNameC != axisNameC )
		axis = axis->nextAxis;

	if		( axis == NULL )
		*msgPtr     += "\nE Spline axis not Configured!";

	else if ( parsed == 4 )
		type		= Bezier;

	else if ( parsed == 2 ) {
		type		= Linear;
		p4			= p2;
		p2			= 0.0; }

	else {
		snprintf( data, 40,
			"\nE %s Invalid",
			data );
		*msgPtr		+= String( data ); }; };

bool
	SplineAxis::valid() {
	
	return
		type != Invalid; };

void
	SplineAxis::calculateProgramTarget(
		float t,
		float p2f,
		float p3f,
		float p4f ) {

	float  microstepTargetF;
 
	if ( type == Bezier ) // Cubic Bezier Interpolation
        microstepTargetF	=
				p2 * p2f
			+	p3 * p3f
			+	p4 * p4f;

	else // Linear Interpolation
		microstepTargetF	=
			p4 * t;
 
	axis->calculateTransformedTargets( microstepTargetF - currentPoint );
	currentPoint	= microstepTargetF; };
