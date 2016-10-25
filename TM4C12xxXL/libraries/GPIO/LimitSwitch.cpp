
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

#include <LimitSwitch.h>
#include <Timers.h>
#include <ADC.h>
#include <Spindle.h>

#include "inc/hw_adc.h"
#include "driverlib/adc.h"
#include <inc/hw_gpio.h>


LimitSwitch::LimitSwitch(
	char*		data,
	String*		msgPtr )

	:	InputPin(
		data,
		msgPtr ) {

	analogAverage	= 0;
	char highChar	= ' ';

	sscanf( data + parseLength + 1,
		" %c",
		&highChar );

	high		= highChar == 'h'; };

LimitSwitch::~LimitSwitch( void ) {
#ifdef DebugDelete
	Serial.println( " delete LimitSwitch" );
#endif
};

void
	LimitSwitch::fmtPin( String* msgPtr ) {

	char data[10];
	snprintf( data, 10,
		" P%c%d%c%c%c",
		portC,
		pin,
		activeMode,
		driveCurrent,
		high ?
			'h'
		:	'l' );

	*msgPtr		+= String( data ); };

bool
	LimitSwitch::readPin() {

	// Debounce logic
	analogAverage	-= analogAverage >> 8;
	if ( InputPin::readPin() )
		analogAverage	+= 0x100;
	
	return
		analogAverage	>= 0x8000; };
