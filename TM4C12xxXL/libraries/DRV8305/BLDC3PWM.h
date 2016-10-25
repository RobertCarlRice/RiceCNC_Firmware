
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

#ifndef BLDC3PWM_h
#define BLDC3PWM_h

#include "Energia.h"
#include <BLDC3.h>


class PwmOutputPin;
class DRV8305Axis;


class BLDC3PWM : public BLDC3 {
	PwmOutputPin*	phaseApin;
	PwmOutputPin*	phaseBpin;
	PwmOutputPin*	phaseCpin;

	bool
		configurePin(
			PwmOutputPin*	phasePin,
			String*			msgPtr,
			uint32_t		period );

	bool
		valid();

	void
		assertTorque(
			uint32_t	torqueA,
			uint32_t	torqueB,
			uint32_t	torqueC );

	void
		activeFreewheel();

	public:
		BLDC3PWM(
			char*			data,
			String*			msgPtr,
			DRV8305Axis*	aaxis );

		~BLDC3PWM();
	};

#endif
