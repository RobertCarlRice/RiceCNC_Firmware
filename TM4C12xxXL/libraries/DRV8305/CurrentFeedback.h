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

#ifndef CurrentFeedback_h
#define CurrentFeedback_h

#include "Energia.h"
#include <Axis.h>

class AnalogPin;
class BLDC3;

class CurrentFeedback {
	public:
		CurrentFeedback(
			char*		data,
			String*		msgPtr );

		~CurrentFeedback();

		AnalogPin*	csa;
		AnalogPin*	csb;
		AnalogPin*	csc;

		// Additional data not stored in 8305 registers
		union {
			struct {
				uint8_t holdCurrent;
				uint8_t constantCurrent;
				uint8_t acceleratingCurrent;
				uint8_t deceleratingCurrent; } st;
			uint32_t	powerLevels; } powerLevelsU;

		uint32_t
			maxCurrentScale;

		uint32_t
			maxCurrentCompare;

		bool
			valid();

		bool
			currentLimitExceeded( BLDC3* motor );

		uint32_t
			motorCurrent( BLDC3* motor );

		void
			motorStateChanged( Axis::MotorState motorState );
	};

#endif
