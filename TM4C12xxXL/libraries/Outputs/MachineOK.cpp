
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

#include <MachineOK.h>
#include <MachineCommands.h>
#include <SysTick.h>
#include <InputPin.h>


bool
	MachineOK::validate( char* data, String* msgPtr ) {

	pin  = new InputPin( data, msgPtr );
	if ( pin->valid() ) {
		pinState        = pin->readPin();
		timeout			= false;
		statusChange	= true;
		lastPulseTime	= SysTick::microSecondTimer;

		*msgPtr			+= " Machine OK pin ";
		pin->fmtPin( msgPtr );

		return
			true; };
 
	delete pin;
	pin    = NULL;

	return
		false; };


bool
	MachineOK::checkMachineNotOk() {

	if ( pinState != pin->readPin() ) {
		pinState		= ! pinState;
		lastPulseTime	= SysTick::microSecondTimer;
		if ( timeout ) {
			timeout			= false;
			statusChange	= true; }; }

	else if ( ( SysTick::microSecondTimer - lastPulseTime ) > 1000000 ) {
		if ( ! timeout ) {
			timeout			= true;
			statusChange	= true; }; };

	return
		timeout; };

void
	MachineOK::reportDigital( String* msgPtr ) {

	if ( statusChange ) {
		char  data[8];
		snprintf( data, 8,
			" %c%d %c",
			pin->portC,
			pin->pin,
			timeout ?
					'1'	// Alarm
				:	'0' );
		*msgPtr			+= String( data );
		statusChange	= false; }; };
