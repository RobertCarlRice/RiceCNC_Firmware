
// © RiceMotion ( Robert Carl Rice ) 2012-2015 - All rights reserved


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

#include <QEI4X.h>
#include <StepCounter.h>
#include <InputPin.h>
#include <Timers.h>

#include <inc/hw_gpio.h>
#include "driverlib/interrupt.h"
#include <driverlib/pin_map.h>
#include <inc/hw_qei.h>
#include "driverlib/qei.h"


QEI4X::QEI4X(
	float		ascale ) {

	scale		= ascale;

	SysCtlPeripheralEnable( SYSCTL_PERIPH_QEI0 );
//	SysCtlPeripheralReset( SYSCTL_PERIPH_QEI0 );

#if defined( PART_TM4C1294NCPDT )         // Connected LaunchPad
	GPIOPinConfigure( GPIO_PL1_PHA0 );
	GPIOPinConfigure( GPIO_PL2_PHB0 );
#else
	GPIOPinConfigure( 0x00031806 );	// GPIO_PD6_PHA0
	GPIOPinConfigure( 0x00031C06 );	// GPIO_PD7_PHB0
#endif

	GPIOPinTypeQEI(
		GPIO_PORTL_BASE,
		GPIO_PIN_1 | GPIO_PIN_2 );

/*
	QEIVelocityConfigure(
		QEI0_BASE,
		QEI_VELDIV_1,
		SysCtlClockGet() ); // Divide by clock speed to get counts/sec
*/
    // Set the maximum position to 32 bit counter
	HWREG( QEI0_BASE + QEI_O_MAXPOS )	= 0xFFFFFFFF;
	qCount		= HWREG( QEI0_BASE + QEI_O_POS );

	QEIunion qei;
	qei.qeiCtl				= HWREG( QEI0_BASE + QEI_O_CTL );

	qei.st.enable			= 1;
	qei.st.stepDir			= 0;
	qei.st.capture4X		= 1;
	qei.st.indexReset		= 0;
	qei.st.captureVelocity	= 0;

	qei.st.filterPreScale	= 3;
	qei.st.filterEnable		= 1;

	HWREG( QEI0_BASE + QEI_O_CTL )	= qei.qeiCtl; };


QEI4X::~QEI4X() {

#ifdef DebugDelete
	Serial.println( " delete QEI4X" );
#endif

	HWREG( QEI0_BASE + QEI_O_CTL ) &= ~QEI_CTL_ENABLE; };

float
	QEI4X::readScaledCount() {

	uint32_t pos	= HWREG( QEI0_BASE + QEI_O_POS );
	uint32_t delta	= pos - qCount;
	qCount			= pos;

	return
		scale
	*	(int32_t)delta; };

void
	QEI4X::adjust( int32_t adjustment ) {
	
	qCount	-= adjustment; };
