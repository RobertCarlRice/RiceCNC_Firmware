
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

#include <QuadratureCounter1X.h>
#include <StepCounter.h>
#include <InputPin.h>
#include <Timers.h>

#include <inc/hw_gpio.h>
#include "driverlib/interrupt.h"


QuadratureCounter1X::QuadratureCounter1X(
	InputPin*		aPin1,
	InputPin*		aPin2,
	float			ascale ) {

	pin1			= aPin1;
	pin2			= aPin2;
	scale			= ascale;

	pin1->enableActiveEdgeInterrupt();
	pin1->stepCounter = this; };


QuadratureCounter1X::~QuadratureCounter1X() {

#ifdef DebugDelete
	Serial.println( " delete QuadratureCounter1X" );
#endif

	delete pin1;
	delete pin2; };

void
	QuadratureCounter1X::pinISR( InputPin* pin ) {

	// pin will always be pin1
	if ( pin2->readPin() )
		adjust( -1 );
//		stepCount--;
	else
		adjust( 1 );
//		stepCount++;
	};
