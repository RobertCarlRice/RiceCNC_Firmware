
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

#ifndef PwmOutputPin_h
#define PwmOutputPin_h

#include <Energia.h>
#include <OutputPin.h>

#include <driverlib/sysctl.h>

class PwmOutputPin : public OutputPin {

	typedef struct {
		char		portC;
		uint8_t		pin;
		uint32_t	timerBase;
		uint16_t	mask;
		uint32_t	pinConfigure; } GpioTimerConfigure;

	static uint16_t assignedTimers;

	const GpioTimerConfigure*	configureBase;

	public:
		PwmOutputPin(
			char*		data,
			String*		msgPtr );
		~PwmOutputPin();

		uint32_t	period;

		bool
			isPWM();

		void
			enable();
	
		void
			disable();

		bool
			configureTimerBase( String*	msgPtr );

		void
			setPeriod( uint32_t period );

		void
			setOnTime( uint32_t onTime );
};

#endif
