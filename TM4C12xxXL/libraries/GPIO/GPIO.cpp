
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

#include <GPIO.h>
#include <Timers.h>
#include <ADC.h>
#include <Spindle.h>

// derived classes
#include <AnalogPin.h>
#include <InputPin.h>
#include	<LimitSwitch.h>
#include <OutputPin.h>
#include	<PwmOutputPin.h>

#include "inc/hw_adc.h"
#include "driverlib/adc.h"
#include <inc/hw_gpio.h>


GpioPin::GpioPin(
	char*		data,
	String*		msgPtr ) {

	gpioBase		= 0;
	portC			= 'A';
	pin				= 0;
	pinSelectMask	= 0;

	char header		= 'X';
	activeMode		= 'h';
	driveCurrent	= '3'; // 8ma

	parseLength		= 0;
	sscanf( data,
		" %c%c%c%c%c%ln",
		&header,
		&portC,
		&pin,
		&activeMode,
		&driveCurrent,
		&parseLength );

	if ( header != 'P' )
		return;

	pin				-= '0';
	mask			= 1 << pin;
	if ( ! mask ) {
		*msgPtr		+= "\nE Invalid pin number ";
		*msgPtr		+= String( data );
		return; };

	uint8_t baseOffset	= portC - 'A';
	baseStruct			= NULL;

	if ( baseOffset < BaseTableLength ) {
		baseStruct	= &GPIOBaseVector[ baseOffset ];
		gpioBase	= baseStruct->base; };

	if ( ! gpioBase ) {
		*msgPtr		+= "\nE Invalid port ";
		*msgPtr		+= String( data );
		return; };

	ROM_SysCtlPeripheralEnable( baseStruct->sysCtl ); // Enable GPIO port

	pinSelectMask	=
			gpioBase
		+	GPIO_O_DATA
		+	( mask << 2 );  // address bit map

	activeLow	= activeMode == 'l';

	Timers::checkPinConflict( this ); };


GpioPin::~GpioPin( void ) {
#ifdef DebugDelete
	Serial.println( " delete GpioPin" );
#endif

	if ( valid() )
		ROM_GPIOPinTypeGPIOInput( gpioBase, mask ); };

uint16_t
	GpioPin::id() {

	return
		portC << 8 + pin; };

bool
	GpioPin::valid() {

	return
		pinSelectMask != 0; };

bool
	GpioPin::readPin() {
	
	return
		false; };
