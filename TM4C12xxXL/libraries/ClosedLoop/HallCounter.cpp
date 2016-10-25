
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

#include <HallCounter.h>
#include <StepCounter.h>
#include <InputPin.h>
#include <Timers.h>

#include <inc/hw_gpio.h>
#include "driverlib/interrupt.h"


HallCounter::HallCounter(
	InputPin*		pin1,
	InputPin*		pin2,
	InputPin*		pin3,
	float			ascale ) {

	hallA			= pin1;
	hallB			= pin2;
	hallC			= pin3;
	scale			= ascale;

	pinsU.allPins	= 0;

	hallA->enableBothEdgeInterrupt();
	hallB->enableBothEdgeInterrupt();
	hallC->enableBothEdgeInterrupt();

	hallA->stepCounter = this;
	hallB->stepCounter = this;
	hallC->stepCounter = this; };


HallCounter::~HallCounter() {

#ifdef DebugDelete
	Serial.println( " delete HallCounter" );
#endif

	delete hallA;
	delete hallB;
	delete hallC; };

void
	HallCounter::pinISR( InputPin* pin ) {

	if		( pin == hallA ) {
		pinsU.st.hallA	= hallA->readPin();

		switch ( pinsU.allPins ) {
			case 5 :
			case 2 :
			adjust( 1 );
//			stepCount++;
			break;

			case 4 :
			case 3 :
			adjust( -1 );
//			stepCount--;
			}; }

	else if ( pin == hallB ) {
		pinsU.st.hallB	= hallB->readPin();

		switch ( pinsU.allPins ) {
			case 4 :
			case 3 :
			adjust( 1 );
//			stepCount++;
			break;
			
			case 1 :
			case 6 :
			adjust( -1 );
//			stepCount--;
		}; }

	else {
		pinsU.st.hallC	= hallC->readPin();

		switch ( pinsU.allPins ) {
			case 6 :
			case 1 :
			adjust( 1 );
//			stepCount++;
			break;
			
			case 5 :
			case 2 :
			adjust( -1 );
//			stepCount--;
			}; }; };
