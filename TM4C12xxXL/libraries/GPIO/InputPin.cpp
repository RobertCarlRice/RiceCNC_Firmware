
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

#include <InputPin.h>
#include <Timers.h>
#include <ADC.h>
#include <Spindle.h>
#include <StepCounter.h>

// derived classes
#include <LimitSwitch.h>

#include "inc/hw_adc.h"
#include "driverlib/adc.h"
#include <inc/hw_gpio.h>
#include "driverlib/interrupt.h"


InputPin*	registeredPin[ BaseTableLength ][ 8 ];

void    portInterruptHandler( uint vector ) {
	gpioBaseStruct* baseStruct	= &GPIOBaseVector[ vector ];

	uint32_t im		= HWREG( baseStruct->base + GPIO_O_RIS );	// or GPIO_O_MIS
	HWREG( baseStruct->base + GPIO_O_ICR )	= im; // clear immediately

	// Make this fast
	InputPin* inputPin;

	if ( im & 0x01 ) {
		inputPin	= registeredPin[ vector ][ 0 ];
		if ( inputPin )
			inputPin->pinISR(); };
	
	if ( im & 0x02 ) {
		inputPin	= registeredPin[ vector ][ 1 ];
		if ( inputPin )
			inputPin->pinISR(); };
	
	if ( im & 0x04 ) {
		inputPin	= registeredPin[ vector ][ 2 ];
		if ( inputPin )
			inputPin->pinISR(); };
	
	if ( im & 0x08 ) {
		inputPin	= registeredPin[ vector ][ 3 ];
		if ( inputPin )
			inputPin->pinISR(); };
	
	if ( im & 0x10 ) {
		inputPin	= registeredPin[ vector ][ 4 ];
		if ( inputPin )
			inputPin->pinISR(); };
	
	if ( im & 0x20 ) {
		inputPin	= registeredPin[ vector ][ 5 ];
		if ( inputPin )
			inputPin->pinISR(); };
	
	if ( im & 0x40 ) {
		inputPin	= registeredPin[ vector ][ 6 ];
		if ( inputPin )
			inputPin->pinISR(); };
	
	if ( im & 0x80 ) {
		inputPin	= registeredPin[ vector ][ 7 ];
		if ( inputPin )
			inputPin->pinISR(); }; };

// Need a separate interrupt handler for each GPIO Port
void    portA_InterruptHandler( void ) {
	portInterruptHandler( 0 ); };

void    portB_InterruptHandler( void ) {
	portInterruptHandler( 1 ); };

void    portC_InterruptHandler( void ) {
	portInterruptHandler( 2 ); };

void    portD_InterruptHandler( void ) {
	portInterruptHandler( 3 ); };

void    portE_InterruptHandler( void ) {
	portInterruptHandler( 4 ); };

void    portF_InterruptHandler( void ) {
	portInterruptHandler( 5 ); };

#ifdef PART_TM4C1294NCPDT
	void    portG_InterruptHandler( void ) {
		portInterruptHandler( 6 ); };

	void    portH_InterruptHandler( void ) {
		portInterruptHandler( 7 ); };

	void    portJ_InterruptHandler( void ) {
		portInterruptHandler( 9 ); };

	void    portK_InterruptHandler( void ) {
		portInterruptHandler( 10 ); };

	void    portL_InterruptHandler( void ) {
		portInterruptHandler( 11 ); };

	void    portM_InterruptHandler( void ) {
		portInterruptHandler( 12 ); };

	void    portN_InterruptHandler( void ) {
		portInterruptHandler( 13 ); };

	void    portP_InterruptHandler( void ) {
		portInterruptHandler( 15 ); };

	void    portQ_InterruptHandler( void ) {
		portInterruptHandler( 16 ); };
#endif

void (*intHandler[])(void) = {
	portA_InterruptHandler,
	portB_InterruptHandler,
	portC_InterruptHandler,
	portD_InterruptHandler,
	portE_InterruptHandler,
	portF_InterruptHandler,

#ifdef PART_TM4C1294NCPDT
	portG_InterruptHandler,
	portH_InterruptHandler,
	NULL,
	portJ_InterruptHandler,
	portK_InterruptHandler,
	portL_InterruptHandler,
	portM_InterruptHandler,
	portN_InterruptHandler,
	NULL,
	portP_InterruptHandler,
	portQ_InterruptHandler
#endif
};


InputPin::InputPin(
	char*		data,
	String*		msgPtr )

	:	GpioPin(
			data,
			msgPtr ) {

	nextInputPin	= NULL;
	stepCounter		= NULL;

	if ( valid() ) {
		ROM_GPIOPinTypeGPIOInput(
			gpioBase,
			mask );

		uint32_t type = GPIO_PIN_TYPE_STD;
		
		switch ( driveCurrent ) {
			case 'u' :
			type = GPIO_PIN_TYPE_STD_WPU; // Weak Pull Up
			break;

			case 'd' :
			type = GPIO_PIN_TYPE_STD_WPD; }; // Weak Pull Down

		ROM_GPIOPadConfigSet(
			gpioBase,
			mask,
			GPIO_STRENGTH_2MA,			// Output current irrelevant
			type );	

		reportedPinState	= ! readPin(); }; };

InputPin::~InputPin( void ) {

#ifdef DebugDelete
	Serial.println( " delete InputPin" );
#endif

	registeredPin[ portC - 'A' ][ pin ] = NULL;
	HWREG( gpioBase + GPIO_O_IM )	&= ~mask; };

void
	InputPin::fmtPin( String* msgPtr ) {

	char data[10];
	snprintf( data, 10,
		" P%c%d%c%c",
		portC,
		pin,
		activeMode,
		driveCurrent );

	*msgPtr		+= String( data ); };

void
	InputPin::reportPinChange( String* msgPtr ) {

	if ( valid() ) {
		bool pinState	= readPin();
		if ( pinState	!= reportedPinState ) {
			reportedPinState	= pinState;

			char data[10];
			snprintf( data, 10,
				" %c%d%c",
				portC,
				pin,
				pinState ?
					'1' :
					'0' );
			*msgPtr  += String( data ); }; }; };

bool
	InputPin::readPin() {

	return
		valid() ?
			activeLow ?
					( HWREG( pinSelectMask ) == 0 )
				:	( HWREG( pinSelectMask ) != 0 )
		:	false; };

void
	InputPin::enableActiveEdgeInterrupt() {		// used for step/direction input
	
	HWREG( gpioBase + GPIO_O_IM )	&= ~mask;	// disable interrupt
	HWREG( gpioBase + GPIO_O_IBE )	&= ~mask;	// not both edges
	HWREG( gpioBase + GPIO_O_IS )	&= ~mask;	// not level sense

	if ( activeLow )
		HWREG( gpioBase + GPIO_O_IEV )	&= ~mask;	// falling event trigger
	else
		HWREG( gpioBase + GPIO_O_IEV )	|= mask;	// rising event trigger

	enableInterrupt(); };

void
	InputPin::enableBothEdgeInterrupt() {		// used for quadrature input
	
	HWREG( gpioBase + GPIO_O_IM )	&= ~mask;	// disable interrupt
	HWREG( gpioBase + GPIO_O_IBE )	|= mask;	// both edges
	HWREG( gpioBase + GPIO_O_IS )	&= ~mask;	// not level sense
	HWREG( gpioBase + GPIO_O_IEV )	|= mask;	// rising event trigger
	enableInterrupt(); };

void
	InputPin::enableInterrupt() {

	if ( HWREG( gpioBase + GPIO_O_IM ) == 0 ) {
		IntPrioritySet(
			baseStruct->interruptVectorNumber,
			GPIO_PriorityLevel );

		GPIOIntRegister(
			gpioBase,
			intHandler[ portC - 'A' ] ); };		// enable

	registeredPin[ portC - 'A' ][ pin ]	= this;
	HWREG( gpioBase + GPIO_O_IM )	|= mask; };	// unmask (enable) interrupt

void
	InputPin::pinISR() {
	
	if ( stepCounter )
		stepCounter->pinISR( this ); };
