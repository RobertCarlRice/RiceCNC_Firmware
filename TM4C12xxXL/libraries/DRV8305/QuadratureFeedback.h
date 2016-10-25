
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

#ifndef QuadratureFeedback_h
#define QuadratureFeedback_h

#include "Energia.h"
#include <Axis.h>

class DRV8305Axis;
class BLDC3;
class StepCounter;
class Machine;

class QuadratureFeedback {
	DRV8305Axis*		drv8305axis;
	BLDC3*				bldc3;

	public:
		QuadratureFeedback(
			char*			data,
			String*			msgPtr,
			DRV8305Axis*	drv8305axis,
			char			counterType );

		~QuadratureFeedback();

		StepCounter*	stepCounter;

		uint32_t	ppr;	// pulses per revolution per phase
		uint32_t	adaptAngle;
		uint32_t	stallDetectVoltage;	// 100% = 4096
		float		voltageProportion;
		float		voltageIntegral;
		float		voltageDerivative;

		float		priorError;
		float		integralError;
		bool		synchronized;
		float		fractionalCount;
	
		bool		leading;
		int32_t		stepAdjustment;

		bool
			valid();

		bool	// called on interpolation interrupt
			calculateFeedback(
				Machine*	machine );
	};

#endif
