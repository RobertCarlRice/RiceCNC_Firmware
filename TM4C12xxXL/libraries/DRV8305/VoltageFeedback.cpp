
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

#include <VoltageFeedback.h>
#include <AnalogPin.h>

#include <inc/hw_gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>

//#define DebugDelete 1

VoltageFeedback::VoltageFeedback(
	char*		data,
	String*		msgPtr ) {

	vsa			= NULL;
	vsb			= NULL;
	vsc			= NULL;
		
	char	vsaPinString	[8];
	char	vsbPinString	[8];
	char	vscPinString	[8];

	sscanf( data,
		" %s %s %s",
		vsaPinString,
		vsbPinString,
		vscPinString );

	vsa		= new AnalogPin(
		vsaPinString,
		msgPtr );
	if ( vsa->valid() ) {
		*msgPtr	+= "\n vsa";
		vsa->fmtPin( msgPtr ); }
	else {
		delete vsa;
		vsa = NULL; };

	vsb		= new AnalogPin(
		vsbPinString,
		msgPtr );
	if ( vsb->valid() ) {
		*msgPtr	+= "\n vsb";
		vsb->fmtPin( msgPtr ); }
	else {
		delete vsb;
		vsb = NULL; };

	vsc		= new AnalogPin(
		vscPinString,
		msgPtr );
	if ( vsc->valid() ) {
		*msgPtr	+= "\n vsc";
		vsc->fmtPin( msgPtr ); }
	else {
		delete vsc;
		vsc = NULL; };
	};

bool
	VoltageFeedback::valid() {
	
	return
		vsa
	&&	vsb
	&&	vsc; };

VoltageFeedback::~VoltageFeedback() {

#ifdef DebugDelete
	Serial.println( " delete VoltageFeedback" );
#endif

	if ( vsa )
		delete vsa;
	if ( vsb )
		delete vsb;
	if ( vsc )
		delete vsc; };
