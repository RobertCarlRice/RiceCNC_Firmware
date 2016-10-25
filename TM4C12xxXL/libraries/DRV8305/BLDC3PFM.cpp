
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

#include <BLDC3PFM.h>
#include <StepCounter.h>

#include <InputPin.h>
#include <OutputPin.h>
#include <SoftPwmOutputPin.h>
#include <AnalogPin.h>

#include <Timers.h>
#include <DRV8305Axis.h>
#include <MachineCommands.h>
#include <HallFeedback.h>

#include <inc/hw_gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>

//#define DebugDelete 1

BLDC3PFM::BLDC3PFM(
	char*			data,
	String*			msgPtr,
	DRV8305Axis*	aaxis )

	:	BLDC3(
			data,
			msgPtr,
			aaxis ) {

	phaseApin		= NULL;
	phaseBpin		= NULL;
	phaseCpin		= NULL;

	char	phaseApinString		[8];
	char	phaseBpinString		[8];
	char	phaseCpinString		[8];
	
	sscanf( data + parseLength,
		" %s %s %s",
		phaseApinString,
		phaseBpinString,
		phaseCpinString );

	*msgPtr		+= "\n inHA - Phase A";
	phaseApin	= new SoftPwmOutputPin(
		phaseApinString,
		msgPtr );
	if ( phaseApin->valid() )
		phaseApin->fmtPin( msgPtr );
	else {
		*msgPtr		+= " invalid";
		return; };
	phaseApin->addSoftPWM();

	*msgPtr		+= "\n inHB - Phase B";
	phaseBpin	= new SoftPwmOutputPin(
		phaseBpinString,
		msgPtr );
	if ( phaseBpin->valid() )
		phaseBpin->fmtPin( msgPtr );
	else {
		*msgPtr		+= " invalid";
		return; };
	phaseBpin->addSoftPWM();

	*msgPtr		+= "\n inHC - Phase C";
	phaseCpin	= new SoftPwmOutputPin(
		phaseCpinString,
		msgPtr );
	if ( phaseCpin->valid() )
		phaseCpin->fmtPin( msgPtr );
	else {
		*msgPtr		+= " invalid";
		return; };
	phaseCpin->addSoftPWM();

//	stepTorqueScaling	= 1<<17;	// Max torque 100% = 128
};

bool
	BLDC3PFM::valid() {
	
	return
		phaseApin
	&&	phaseBpin
	&&	phaseCpin; };


BLDC3PFM::~BLDC3PFM() {

#ifdef DebugDelete
	Serial.println( " delete BLDC3PFM" );
#endif

	if ( phaseApin )
		delete phaseApin;

	if ( phaseBpin )
		delete phaseBpin;

	if ( phaseCpin )
		delete phaseCpin; };

void
	BLDC3PFM::activeFreewheel() {

	phaseApin->compareValue = 0;
	phaseBpin->compareValue = 0;
	phaseCpin->compareValue = 0; };

void
	BLDC3PFM::assertTorque(
		uint32_t	torqueA,
		uint32_t	torqueB,
		uint32_t	torqueC ) {

	phaseApin->compareValue = torqueA;
	phaseBpin->compareValue = torqueB;
	phaseCpin->compareValue = torqueC; };
