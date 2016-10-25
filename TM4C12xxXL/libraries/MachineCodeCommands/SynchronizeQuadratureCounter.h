
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

#ifndef SynchronizeQuadratureCounter_h
#define SynchronizeQuadratureCounter_h

#include "Energia.h"
#include <Pause.h>

class DRV8305Axis;
class QuadratureFeedback;
class BLDC3;

class SynchronizeQuadratureCounter : public Pause {
	static const char myName[29];

	char				axisName;

	uint32_t			phase;
	uint32_t			torque;
	uint32_t			delay;

	bool				success;
	DRV8305Axis*		axis8305;
	BLDC3*				motor;
	QuadratureFeedback*	quadratureFeedback;
	uint32_t			startTime;

	enum SetHallState {
		SyncQuadratureINIT,
		SyncQuadratureWait,
		SyncQuadratureEND } syncQuadratureState;

	void
		restart();

	public:
	SynchronizeQuadratureCounter(
		char*	data );
//	~SynchronizeQuadratureCounter();

	virtual const char*
		name();

	bool
		execute(
			Machine*	machine );
    
	void
		report(
			Machine*	machine,
			String*		msgPtr );

	void
		reportInterpolationStatus(
			Machine*	machine,
			String*		msgPtr );
};

#endif