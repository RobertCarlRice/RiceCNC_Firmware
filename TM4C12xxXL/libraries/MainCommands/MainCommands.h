
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

#ifndef MainCommands_h
#define MainCommands_h

#include "Energia.h"

#if defined( PART_TM4C1294NCPDT )  // Connected LaunchPad
	#include <MyEEProm.h>
#endif


#define	INTERPOLATOR_NAME		 'I'
#define		RESET_COMMAND	         'R'
#define		CONFIGURE_INTERPOLATOR   'i'
#define		CREATE_MACHINE			 'm'
#define		CONFIGURE_SSI			 's'
#define			SSI_TYPE_ST				'S'
#define			SSI_TYPE_8711			'T'
#define			SSI_TYPE_8305			'B'
#define		GET_VERSION				 'v'
#define		WATCHDOG                 'w'
#define		SET_ETHERNET_ADDRESSES   'e'

#if defined( PART_TM4C1294NCPDT )  // Connected LaunchPad
  #define firmware_version        61

#elif defined( PART_LM4F120H5QR )  // LaunchPad
  #define firmware_version        61

#endif

class MainCommands {
public:
	static uint32_t		interpolationInterruptRate;
	static uint32_t		hostTimeout;
	static uint32_t		hardwareOvesampling;
    static uint32_t		periodicReportingInterval;     // micro-seconds

	static float	 stopFactor;
	static float	 targetLead;

#if defined( PART_TM4C1294NCPDT )  // Connected LaunchPad
	static void
		softwareReset( MyEEProm::SoftResetReason softResetReason );
#else
	static void
		softwareReset();
#endif

	static void
		configureInterpolator(
			char*	data,
			String* msgPtr );

	static void
		configureSSI(
			char*	data,
			String* msgPtr );

	static void
		createMachine(
			char*	data,
			String* msgPtr );

	static void
		deleteMachine(
			char*	data,
			String* msgPtr );

	static void
		signOnMsg(
			String* msg );

	static void
		processCommand(
			char*	commandPtr,
			String* msgPtr );

	static void
		processInterpolatorCommand(
			char*	iCommandPtr,
			String* msgPtr );
};

#endif
