
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

#include <RiceCNC_Main.h>
#include <MainCommands.h>
//#include <SysTick.h>
#include <Machine.h>
#include <MySerial.h>
#include <SSI.h>
#include <Timers.h>
//#include <AxisTimer.h>
//#include <InterpolationTimer.h>
#include <ADC.h>

#if defined( PART_TM4C1294NCPDT )  // Connected LaunchPad
	#include <MyEEProm.h>
#endif

#include <driverlib/sysctl.h>
#include <driverlib/fpu.h>

#if defined( PART_TM4C1294NCPDT )         // Connected LaunchPad
	#include <Ethernet.h>
	#include <MyEthernet.h>
	#include <MyEthernetUdp.h>
	#include <Retransmit.h>

	#include <inc/tm4c1294ncpdt.h>
  
#elif defined( PART_LM4F120H5QR )  // LaunchPad
	#include <inc/tm4c123gh6pm.h>

#endif

uint32_t   loopLedCount  = 0L; // Counter for D3 LED flasher x11-64

#ifdef PART_TM4C1294NCPDT
    MyEthernet* myEthernet = new MyEthernet();
#endif

void
	RiceCNC_Main::setup() {

	Machine::firstMachine  = NULL;
  
	MainCommands::interpolationInterruptRate  = 1500;

	// FPU
	FPUEnable();
//	FPUFlushToZeroModeSet( FPU_FLUSH_TO_ZERO_DIS ); // Default - Don't adjust very small numbers to zero
	FPULazyStackingEnable();                       // Reserve space on stack to save fpu context. FPU is avail in isrs.
 
	Timers::enableTimers();

#ifdef PART_TM4C1294NCPDT
 	MyEEProm::readEEProm();
#endif

	MySerial::setup();

	ADC::configure();

#ifdef PART_TM4C1294NCPDT
    myEthernet->startEthernet();
#endif
};


void
	RiceCNC_Main::loop() {

#ifdef PART_TM4C1294NCPDT
    Timers::setLoopLED( ++loopLedCount >> 10 );

#elif defined( PART_LM4F120H5QR )
    Timers::setLoopLED( ++loopLedCount >> 13 );

#endif

	MySerial::loop(); // Accept commands via serial even if ethernet is running

#ifdef PART_TM4C1294NCPDT
    myEthernet->loop();

// prefer sending background messages via ethernet
    if ( myEthernet->background() )
		return;
#endif

	MySerial::background(); };


bool
	RiceCNC_Main::isBusy() { // Used to adapt interpolation interrupt

#ifdef PART_TM4C1294NCPDT
    return myEthernet->isBusy() || MySerial::isBusy();
#else
	return MySerial::isBusy();
#endif
};

