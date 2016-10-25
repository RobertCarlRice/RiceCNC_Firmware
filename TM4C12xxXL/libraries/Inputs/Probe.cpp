
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

#include <Probe.h>
#include  <InputPin.h>


Probe::~Probe() {
	if ( pin )
		delete pin; };

Probe::Probe(
	char*		data,
	String*		msgPtr ) {

	analogAverage	= 0;

	transition		= (*data) == '+';

	pin		= new InputPin(
		data + 1,
		msgPtr );

	if ( pin->valid() ) {
		state		= debounce();

		*msgPtr		+= " Probe input pin ";
		pin->fmtPin( msgPtr );

		if ( transition )
			*msgPtr    += " On transition"; }

	else {
		delete pin;
		pin    = NULL; }; };

bool
	Probe::isValid() {

	return
		pin		!= NULL; };
	
bool
	Probe::probeStop() {

	bool priorState		= state;
	state               = debounce();
	return
		(	state
		&&	( ! transition || ! priorState ) ); };

void
	Probe::reportDigital( String* msgPtr ) {

	pin->reportPinChange( msgPtr ); };

bool
	Probe::debounce() {
	
	analogAverage	-= analogAverage >> 8;
	if ( pin->readPin() )
		analogAverage	+= 0x100;

	return
		analogAverage	>= 0x8000; };

	
