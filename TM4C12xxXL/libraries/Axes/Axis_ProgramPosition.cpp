
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

#include <Axis.h>
#include <AxisTimer.h>
#include <Link.h>


void
	Axis::deleteLinks() {

	deleteSuperiorLinks();
	while ( firstSubordinateLink )
		delete firstSubordinateLink; };
		
void
	Axis::deleteSuperiorLinks() {

	while ( firstSuperiorLink )
		delete firstSuperiorLink; };


// virtual methods
void
	Axis::adjustProgramPosition( float positionF ) {

	// adjust program position without causing motor movement
	AxisTimer::disableAxisInterrupts();
		moveProgramPosition(
			positionF
		*	microStepsPerUnitDistanceF );
	AxisTimer::enableAxisInterrupts();	};

void
	Axis::moveProgramPosition( float scaledOffset ) {

	uStepProgramPositionF	+= scaledOffset;
	// extend to double precision
	int32_t microSteps		= roundToInt( uStepProgramPositionF );
	uStepProgramPositionI	+= microSteps;
	uStepProgramPositionF	-= microSteps; };

void
	Axis::calculateTransformedTargets( float microstepDeltaF ) {

	if ( microstepDeltaF == 0.0 )
		return;

	moveProgramPosition( microstepDeltaF );
	
	/*
	Program space is coupled to machine space by the transformation matrix

	A default identity transform axis will not be dependent on other axes,
	i.e., no superiors. For the default we adjust the machine position
	according to the delta. However, if the axis has superiors, then this
	axis machine position changes only if the axis machine position has a
	scaled dependency upon its program position.
	*/
	if ( ! firstSuperiorLink )
		interpolationMicrostepBuffer	+= microstepDeltaF;

	// adjustTargetPositions for subordinate axes but not programPositions.
	// For scaling, this axis will be a subordinate of itself
	Link* link		= firstSubordinateLink;
	while ( link ) {
		link->calculateTransformedTargets( microstepDeltaF );
		link        = link->nextSubordinateLink; }; };
