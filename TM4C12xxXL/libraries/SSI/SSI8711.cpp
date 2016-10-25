
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

#include <SSI8711.h>
#include <SsiDeviceQueue.h>
#include <Machine.h>
#include <AxisTimer.h>
#include <OutputPin.h>
#include <Timers.h>

#include <inc/hw_ssi.h>
#include <inc/hw_gpio.h>
#include <driverlib/ssi.h>


SSI8711::SSI8711(
	char*		data,
	String*		msgPtr )

	:	SSI(
			data,
			msgPtr ) {

	resetPin		= NULL;
	sleepPin		= NULL;
	directionPin	= NULL;
	stepPin			= NULL;

	if ( ! configured )
		return;

	char    sleepPinString		[8];
	char    resetPinString      [8];
	char    directionPinString	[8];
	char    stepPinString		[8];

	sscanf( data + parseLength + 1,
		" %s %s %s %s",
		sleepPinString,
		resetPinString,
		directionPinString,
		stepPinString );

	*msgPtr			+= "\n Reset";
	resetPin		= new OutputPin(
		resetPinString,
		msgPtr );
	if ( ! resetPin->valid() )
		return;
	resetPin->fmtPin( msgPtr );
	resetPin->setPin( true );

	*msgPtr			+= "\n Sleep";
	sleepPin		= new OutputPin(
		sleepPinString,
		msgPtr );
	if ( ! sleepPin->valid() )
		return;
	sleepPin->fmtPin( msgPtr );
	sleepPin->setPin( false );

	*msgPtr			+= "\n Dir";
	directionPin	= new OutputPin(
		directionPinString,
		msgPtr );
	if ( ! directionPin->valid() )
		return;
	directionPin->fmtPin( msgPtr );
	directionPin->setPin( false );

	*msgPtr			+= "\n Step";
	stepPin			= new OutputPin(
		stepPinString,
		msgPtr );
	if ( ! stepPin->valid() )
		return;
	stepPin->fmtPin( msgPtr );
	stepPin->setPin( false );

	configureSSI( TI_8711_Protocol );
  
	resetPin->setPin( false ); // release reset

	snprintf( data, 40,
		"\n SSI %lu configured for BOOST-DRV8711\n",
		number );
	*msgPtr     += String( data );

	configured	= true; };

SSI8711::~SSI8711() {
	if ( resetPin )
		delete resetPin;

	if ( sleepPin )
		delete sleepPin;

	if ( directionPin )
		delete directionPin;

	if ( stepPin )
		delete stepPin; };
