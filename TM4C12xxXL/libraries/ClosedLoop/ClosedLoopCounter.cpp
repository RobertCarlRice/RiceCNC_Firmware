
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

#include <ClosedLoopCounter.h>
#include <StepCounter.h>
#include <QuadratureCounter4X.h>
#include <StepDirectionCounter.h>

#include <InputPin.h>
#include <Timers.h>
#include <Machine.h>
#include <AxisTimer.h>
#include <Axis.h>

#include <inc/hw_gpio.h>
#include "driverlib/interrupt.h"


ClosedLoopCounter::ClosedLoopCounter(
	StepCounter*		aStepCounter,
	float				aAdjustmentFactor ) {

	stepCounter			= aStepCounter;
	adjustmentFactor	= aAdjustmentFactor;
	stepped				= 0.0; };

ClosedLoopCounter::~ClosedLoopCounter() {
#ifdef DebugDelete
	Serial.println( " delete ClosedLoopCounter" );
#endif
	if ( stepCounter )
		delete stepCounter;
};

void
	ClosedLoopCounter::interpolationIsr(
		Machine*	machine,
		Axis*		axis ) {

	// Need to wait for more than one count to prevent hunting
	// and allow axis to go idle
	if ( abs( stepCounter->stepCount ) > 1 ) {
		float scaledCount	=
			stepCounter->readScaledCount();
		if ( scaledCount ) {
			stepped			-= scaledCount;

			float maxCorrection	=
					fabs( scaledCount )
				*	adjustmentFactor;
			float correction	= stepped;

			if ( correction ) {
				if ( correction > maxCorrection )
					correction	= maxCorrection;

				else if ( correction < -maxCorrection ) {
					correction	= -maxCorrection; };

				AxisTimer::disableAxisInterrupts();
					axis->moveMachinePositionAndTarget( -correction );
				AxisTimer::enableAxisInterrupts(); }; }; };
	
	if ( fabs( stepped / stepCounter->scale ) > 10.0 ) {
		machine->stallDetected();

		AxisTimer::disableAxisInterrupts();
			axis->moveMachinePositionAndTarget( -stepped );
		AxisTimer::enableAxisInterrupts(); }; };
