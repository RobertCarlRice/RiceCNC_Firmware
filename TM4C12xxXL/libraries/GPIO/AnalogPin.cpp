
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

#include <AnalogPin.h>
#include <Timers.h>
#include <ADC.h>
#include <Spindle.h>
#include <MainCommands.h>

// derived classes

#include "inc/hw_adc.h"
#include "driverlib/adc.h"
#include <inc/hw_gpio.h>

//#define DebugDelete 1

AnalogPin::AnalogPin (
	char*		data,
	String*		msgPtr )

	:	GpioPin(
		data,
		msgPtr ) {

	nextAnalogPin		= NULL;
	aid					= NULL;
	reportedAnalogValue	= 0;
	softwareOvesampling	= activeMode - '0';
	enabled				= true;

	if ( valid() ) {
		fmtPin( msgPtr );

		for (	int index = 0;
				index < AnalogTableLength;
				index++ ) {

			AnalogChannelPins* tableAid = &AnalogChannel[ index ];
			if (	tableAid->port	== portC
				&&	tableAid->pin	== pin ) {
				
				aid		= tableAid;
				break; }; };

		if ( aid ) { // Analog
			ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC0 );
			ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC1 );

			ROM_GPIOPinTypeADC( gpioBase, mask );
			ADC::addAnalogPin( this );

			*msgPtr
				+= " ok"; }

		else
			*msgPtr
				+= " invalid"; }; };


AnalogPin::~AnalogPin() {

#ifdef DebugDelete
	Serial.println( " delete AnalogPin" );
#endif

	if ( aid ) // Analog
		ADC::removeAnalogPin( this ); };

void
	AnalogPin::fmtPin( String* msgPtr ) {

	char data[30];
	snprintf( data, 30,
		" P%c%d sw over-sample %d",
		portC,
		pin,
		1 << softwareOvesampling );

	*msgPtr += String( data ); };

void
	AnalogPin::reportAnalog( String* msgPtr ) {

	if ( aid ) {
		uint32_t currentValue = analogValue();
		if ( currentValue != reportedAnalogValue ) {
			reportedAnalogValue = currentValue;

			char data[8];
			snprintf( data, 8,
				" %c%d %d",
				portC,
				pin,
				analogValue() );
			*msgPtr  += String( data ); }; }; };

void
	AnalogPin::averageSample(	uint32_t sample ) {

	if ( enabled ) {
		analogAverage	-= analogAverage >> softwareOvesampling;
		analogAverage	+= sample << 10; }; }; // sample is 12 bits

uint32_t
	AnalogPin::analogValue() {

	return
		analogAverage >> softwareOvesampling + 10; };
