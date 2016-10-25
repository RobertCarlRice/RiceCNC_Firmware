
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

#include <Energia.h>


/*
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <strings.h>
#include <stdio.h>      // printf, fgets
#include <stdlib.h>     // atoi

#include <inc/hw_types.h>
#include <inc/hw_watchdog.h>
#include <inc/hw_ints.h>
#include <inc/hw_timer.h>
#include <inc/hw_gpio.h>
#include <inc/hw_ssi.h>
#include "inc/hw_uart.h"
#include <inc/hw_memmap.h>
#include <inc/hw_sysctl.h>
#include "inc/hw_pwm.h"

#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/gpio.h>
#include <driverlib/pwm.h>
#include <driverlib/fpu.h>
#include <driverlib/SysTick.h>
#include <driverlib/timer.h>
#include <driverlib/ssi.h>
#include <driverlib/debug.h>
#include "driverlib/watchdog.h"
#include "driverlib/rom_map.h"
#include <driverlib/uart.h>
#include "driverlib/rom.h"
*/

// include one header from eacxh library subdirectory to make the library available

// This ifdef should work but doesn't. Energia seems to ignore the ifdef and include the ethernet.h regardless
// Thes two includes must be commented out when compiling for the TM4C123GXL LaunchPad
#if defined( PART_TM4C1294NCPDT )         // Connected LaunchPad
  #include <Ethernet.h>
  #include <MyEthernet.h>
#endif

#include <SplineCommand.h>      // SplineInterpolation
#include <MachineCommands.h>    // Machine
#include <SetSpindleSpeed.h>    // MachineCodeCommands
#include <Probe.h>              // Inputs
#include <Spindle.h>            // Outputs
#include <StepDirectionAxis.h>  // Axes

#include <Axis.h>               // Axes

#include <DSpinBuffer.h>        // dSpin
#include <DRV8711Axis.h>        // DRV8711
#include <DRV8305Axis.h>        // DRV8305
#include <PowerStepAxis.h>      // PowerStep01

#include <SSI.h>                // SPI
#include <GPIO.h>               // GPIO
#include <StepCounter.h>        // ClosedLoop

#include <RiceCNC_Main.h>       // MainCommands
#include <MainCommands.h>       // MainCommands
#include <SysTick.h>            // Timers

void  setup() {
  RiceCNC_Main::setup(); };
 
void  loop() {
  RiceCNC_Main::loop(); };
