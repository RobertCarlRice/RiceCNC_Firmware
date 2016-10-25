
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

#include <QuadratureCounter4X.h>
#include <StepCounter.h>
#include <InputPin.h>
#include <Timers.h>

#include <inc/hw_gpio.h>
#include "driverlib/interrupt.h"


QuadratureCounter4X::QuadratureCounter4X(
	InputPin*		aPin1,
	InputPin*		aPin2,
	float			ascale ) {

	pin1			= aPin1;
	pin2			= aPin2;
	scale			= ascale;

	pinsU.bothPins	= 0;

	pin1->enableBothEdgeInterrupt();
	pin2->enableBothEdgeInterrupt();

	pin1->stepCounter = this;
	pin2->stepCounter = this; };


QuadratureCounter4X::~QuadratureCounter4X() {

#ifdef DebugDelete
	Serial.println( " delete QuadratureCounter4X" );
#endif

	delete pin1;
	delete pin2; };

void
	QuadratureCounter4X::pinISR( InputPin* pin ) {

	if ( pin == pin1 ) {
		pinsU.st.pin1	= pin1->readPin();

		switch ( pinsU.bothPins ) {
			case 1 :
			case 2 :
			adjust( 1 );
//			stepCount++;
			break;

			case 0 :
			case 3 :
			adjust( -1 );
//			stepCount--;
		}; }

	else {
		pinsU.st.pin2	= pin2->readPin();

		switch ( pinsU.bothPins ) {
			case 0 :
			case 3 :
			adjust( 1 );
//			stepCount++;
			break;
			
			case 1 :
			case 2 :
			adjust( -1 );
//			stepCount--;
		}; }; };
