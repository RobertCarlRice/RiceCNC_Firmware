
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

#include <LinkAxisCommand.h>
#include <Timers.h>
#include <InterpolationTimer.h>
#include <Axis.h>
#include <Link.h>

const char LinkAxisCommand::myName[] =
	"LinkAxis";

const char*
	LinkAxisCommand::name() {
	
	return
		myName; };

LinkAxisCommand::LinkAxisCommand( char* data ) { 
	firstLinkAxis		= NULL;
	char coordinate[20];
	uint32_t length		= 0;

	axisNameC       = data[0];
	while( sscanf( data+1,
			" %s%ln",
			&coordinate,
			&length ) > 0 ) { // returns -1 at eof

		LinkAxis* linkAxisPtr      = new LinkAxis( coordinate );
		linkAxisPtr->nextLinkAxis  = firstLinkAxis;
		firstLinkAxis              = linkAxisPtr;
		data                       += length; }; };

LinkAxisCommand::~LinkAxisCommand() {

    LinkAxis*  linkAxis = firstLinkAxis;
    while ( linkAxis ) {
		LinkAxis* deleteLinkAxis	= linkAxis;
		linkAxis					= linkAxis->nextLinkAxis;
		delete deleteLinkAxis; }; };

bool
	LinkAxisCommand::execute( Machine* machine ) {

    // Machine coordinate space is subordinate to program coordinate space
    subordinateAxis      = machine->findAxis(
		axisNameC,
		NULL );
    if ( ! subordinateAxis )
		return
			true;	// error

	// update the CTM for this axis
	subordinateAxis->deleteSuperiorLinks();

	LinkAxis* linkAxis  = firstLinkAxis;
	while ( linkAxis ) {
		Axis* superiorAxis  = machine->findAxis(
			linkAxis->axisNameC,
			NULL );

		if ( superiorAxis && linkAxis->scale != 0.0 ) {
			Link* link	= new Link(
				superiorAxis,
				subordinateAxis,
				linkAxis->scale );

			link->nextSuperiorLink              = subordinateAxis->firstSuperiorLink;
			subordinateAxis->firstSuperiorLink  = link;

			link->nextSubordinateLink           = superiorAxis->firstSubordinateLink;
			superiorAxis->firstSubordinateLink  = link; };

		linkAxis  = linkAxis->nextLinkAxis; };

    return true; };

void
	LinkAxisCommand::report(
		Machine*	machine,
		String*		msgPtr ) {

	*msgPtr
		+=	" Link "
		+	String( axisNameC );

	LinkAxis*  linkAxis = firstLinkAxis;
	while ( linkAxis ) {
		linkAxis->report( msgPtr );
		linkAxis  = linkAxis->nextLinkAxis; };

	*msgPtr
		+=	"\n"; };
