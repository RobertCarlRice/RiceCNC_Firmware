
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

#ifndef HallFeedback_h
#define HallFeedback_h

#include "Energia.h"
#include <StepCounter.h>

class InputPin;
class BLDC3;
class DRV8305Axis;
class Machine;

class HallFeedback : public StepCounter {
	DRV8305Axis*	axis;
	BLDC3*			bldc3;

	InputPin*		hallA;
	InputPin*		hallB;
	InputPin*		hallC;
	
	enum	HallState {
		CW_A1,
		CW_C0,
		CW_B1,
		CW_A0,
		CW_C1,
		CW_B0,
		CCW_A1,
		CCW_B0,
		CCW_C1,
		CCW_A0,
		CCW_B1,
		CCW_C0,
		HallStart,
		S_A1,
		S_B1,
		S_C1,
		S_A0,
		S_B0,
		S_C0 };

	uint		hallState;

	void
		calculateFeedback();

	public:
		HallFeedback(
			char*			data,
			String*			msgPtr,
			DRV8305Axis*	axis );

		~HallFeedback();

		bool		synchronized;
		uint		expirationCounter;

		// vector is direction, sensor, transition
		uint32_t	phaseTransition[ 12 ];
		uint32_t	stallDetectVoltage;	// 100% = 4096

		int32_t		error;
		int32_t		priorError;
		int32_t		integralError;

	// Angle PID parameter settings
		float		angleDerivative;
		float		angleProportion;
		float		angleIntegral;

	// Voltage PID parameter settings
		float		voltageIntegral;

		float		speedProportion;

		int32_t		feedbackAngle;
		int32_t		feedbackTorque;

		void
			initPhaseTransitionTable();

		void
			reportHallPinChange( String* msgPtr );

		void
			readHallPhasing(
				char*		data,
				String*		msgPtr );

		uint
			initCount();

		bool
			valid();

		virtual void
			pinISR( InputPin* );

		bool
			calculateHallFeedback(
				Machine*	machine );
	};

#endif
