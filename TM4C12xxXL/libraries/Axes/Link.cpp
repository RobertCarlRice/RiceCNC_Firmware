
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

#include <Link.h>
#include <Axis.h>


Link::Link( Axis* aSuperiorAxis, Axis* aSubordinateAxis, float aScale ) {
	superiorAxis       = aSuperiorAxis;
	subordinateAxis    = aSubordinateAxis;
	scale              = aScale; };

Link::~Link() {
	// Remove from the superior chain
	Link** superLinkPtrPtr			= &subordinateAxis->firstSuperiorLink;
	while ( true ) {
		Link* superLink				= *superLinkPtrPtr;
		if ( ! superLink )
			break; // program error

		if ( superLink == this ) { // byppass self
			*superLinkPtrPtr		= nextSuperiorLink;
			break; };

		superLinkPtrPtr				= &superLink->nextSuperiorLink; };

	// Remove from the subordinate chain
	Link** subordinateLinkPtrPtr	= &superiorAxis->firstSubordinateLink;
	while( true ) {
		Link*  subLink              = *subordinateLinkPtrPtr;
		if ( ! subLink )
			break; // not found program error

		if ( subLink == this ) {
			*subordinateLinkPtrPtr  = nextSubordinateLink;
			break; };

		subordinateLinkPtrPtr       = &subLink->nextSubordinateLink; }; };

void
	Link::calculateTransformedTargets( float microstepDeltaF ) {
	
	subordinateAxis->interpolationMicrostepBuffer += microstepDeltaF * scale; };
