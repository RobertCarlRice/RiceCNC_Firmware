
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

#include <SoftPwmOutputPin.h>
#include <Timers.h>
#include <ADC.h>
#include <Spindle.h>
#include <AxisTimer.h>

#include <inc/hw_adc.h>
#include <driverlib/adc.h>
#include <inc/hw_gpio.h>
#include <driverlib/timer.h>


SoftPwmOutputPin*	SoftPwmOutputPin::firstSoftPWM	= NULL;
// Synchronization reduces beat frequency oscillation noise
// It also limits switching frequency converting to PWM
volatile uint32_t	SoftPwmOutputPin::syncCounter	= 0;

void
	SoftPwmOutputPin::addSoftPWM( SoftPwmOutputPin* pwmOutputPin ) {
	
	// prevent duplicates
	SoftPwmOutputPin* softPWM = SoftPwmOutputPin::firstSoftPWM;
	while( softPWM ) {
		if ( softPWM == pwmOutputPin )
			return;	// already added
		softPWM = softPWM->nextSoftPWM; };
	
	// Add as start of chain
	pwmOutputPin->nextSoftPWM	= SoftPwmOutputPin::firstSoftPWM;
	SoftPwmOutputPin::firstSoftPWM	= pwmOutputPin; };

void
	SoftPwmOutputPin::removeSoftPWM( SoftPwmOutputPin* pwmOutputPin ) {
	
	SoftPwmOutputPin** softPWMptr = &SoftPwmOutputPin::firstSoftPWM;
	while( *softPWMptr ) {
		SoftPwmOutputPin* softPWM = *softPWMptr;
		if ( softPWM == pwmOutputPin ) {
			*softPWMptr	= pwmOutputPin->nextSoftPWM;
			return; };
		softPWMptr = &softPWM->nextSoftPWM; }; };

void
	SoftPwmOutputPin::pwmInterrupt() { // every 1 µS
	
	SoftPwmOutputPin* softPWM = SoftPwmOutputPin::firstSoftPWM;
	while( softPWM ) {
		softPWM->pwmInt();
		softPWM = softPWM->nextSoftPWM; };
	syncCounter++; };

SoftPwmOutputPin::SoftPwmOutputPin(
	char*		data,
	String*		msgPtr )

	:	OutputPin(
			data,
			msgPtr ) {

	high			= false;
	analogOutput	= 0;
	compareValue	= 0; };


SoftPwmOutputPin::~SoftPwmOutputPin( void ) {
#ifdef DebugDelete
	Serial.println( " delete SoftPwmOutputPin" );
#endif
	SoftPwmOutputPin::removeSoftPWM( this ); };

void
	SoftPwmOutputPin::addSoftPWM() {
	
	SoftPwmOutputPin::addSoftPWM( this ); };

void
	SoftPwmOutputPin::pwmInt() { // called at high freq > 100 KHz

	analogOutput	-= analogOutput >> 12;
	if ( high )
		analogOutput	+= 0x1000; // 4096

	if (	analogOutput < compareValue	) {	// 50% = 0x800000
		if (	! high
			&&	( SoftPwmOutputPin::syncCounter & 0x7 ) == 0  ) {

			HWREG( pinSelectMask ) = 0xFF;
			high	= true; }; }

	else if ( high ) {
			HWREG( pinSelectMask ) = 0;
			high	= false; }; };
