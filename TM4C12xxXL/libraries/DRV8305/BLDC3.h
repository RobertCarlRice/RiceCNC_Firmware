
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

#ifndef BLDC3_h
#define BLDC3_h

#include "Energia.h"
#include <Axis.h>


class SoftPwmOutputPin;
class DRV8305Axis;
class HallFeedback;


class BLDC3 {
//	static const float		sineTable64[];
//	static const float		tanVector64[];
//	static const uint16_t	commutationVector64[];

	static const float		sineTable50[];
	static const float		tanVector50[];
	static const uint16_t	commutationVector50[];

	virtual void
		assertTorque(
			uint32_t	torqueA,
			uint32_t	torqueB,
			uint32_t	torqueC ) {};

	protected:
		float			lrRatio;
		char			motorType;
		uint32_t		parseLength;

	public:
		BLDC3(
			char*			data,
			String*			msgPtr,
			DRV8305Axis*	aaxis );
		virtual ~BLDC3();

		enum  MagneticAngle50 {	// Use to match 600 ppr encoder
			MS50_0			= 0,
//				MS50_30		= 25,
//			MS50_60			= 50,
				MS50_90		= 75,
			MS50_120		= 100,
//				MS50_150	= 125,
			MS50_180		= 150,
//				MS50_210	= 175,
			MS50_240		= 200,
//				MS50_270	= 225,
//			MS50_300		= 250,
//				MS50_330	= 275,
			MS50_360		= 300 };

		DRV8305Axis*	axis;
	
		volatile uint32_t	armMagneticAngle;
		volatile int32_t	speedLeadAngle;
		volatile int32_t	assertedLeadAngle;
		volatile uint32_t	assertedVoltage;
				
		// Additional data not stored in 8305 registers
		union {
			struct {
				uint8_t holdVoltage;
				uint8_t constantVoltage;
				uint8_t acceleratingVoltage;
				uint8_t deceleratingVoltage; } st;
			uint32_t	powerLevels; } powerLevelsU;

		uint32_t	polePairs;
		uint32_t	holdTorque;
		uint32_t	stepTorque;
		uint32_t	minimumPulseWidth;

		bool
			configured();
		virtual bool
			valid();

		void
			interpolationIsr(
				Machine*		machine );	// 2 kHz

		void
			stepArmature(
				int32_t steps );

		virtual void
			activeFreewheel() {};

		void
			assertTorque();

		void
			motorStateChanged(
				Axis::MotorState motorState );

		int
			currentPhase(); // return the first phase at base ( 0 duty cycle )
	};

#endif
