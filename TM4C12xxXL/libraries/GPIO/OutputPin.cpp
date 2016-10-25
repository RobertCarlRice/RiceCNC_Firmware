
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

#include <OutputPin.h>
#include <Timers.h>
#include <ADC.h>
#include <Spindle.h>

// derived classes
#include <PwmOutputPin.h>

#include "inc/hw_adc.h"
#include "driverlib/adc.h"
#include <inc/hw_gpio.h>


OutputPin::OutputPin(
	char*		data,
	String*		msgPtr )

	:	GpioPin(
		data,
		msgPtr ) {

	if ( valid() ) {
		ROM_GPIOPinTypeGPIOOutput(
			gpioBase,
			mask );
  
		uint32_t type		= GPIO_PIN_TYPE_STD;
		uint32_t strength	= GPIO_STRENGTH_2MA;

		switch ( driveCurrent ) {
			case '0' :
			type		= GPIO_PIN_TYPE_OD;
			break;

			case '1' :	// 2ma
			break;

			case '2' :
			strength	= GPIO_STRENGTH_4MA;
			break;

			case '3' :
			strength	= GPIO_STRENGTH_8MA; };

		ROM_GPIOPadConfigSet(
			gpioBase,
			mask,
			strength,
			type ); }; };


OutputPin::~OutputPin( void ) {
#ifdef DebugDelete
	Serial.println( " delete OutputPin" );
#endif
};

void
	OutputPin::setPin( bool state ) { // Can be called at interrupt level

	if ( valid() ) {

		uint8_t level	= state ?
			0xFF :
			0;

		if ( activeLow )
			level	= ~level;
 
		HWREG( pinSelectMask ) = level; }; };

void
	OutputPin::fmtPin( String* msgPtr ) {

	char data[8];
	snprintf( data, 8,
		" P%c%d%c%c",
		portC,
		pin,
		activeMode,
		driveCurrent );

	*msgPtr += String( data ); };

void
	OutputPin::togglePin() {

	if ( valid() )
		// TivaWare doesn't have a toggle command
		HWREG( pinSelectMask ) = ~HWREG( pinSelectMask ); };
