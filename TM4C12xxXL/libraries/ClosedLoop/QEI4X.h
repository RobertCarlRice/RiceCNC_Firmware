
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

#ifndef QEI4X_h
#define QEI4X_h

#include "Energia.h"
#include <StepCounter.h>

class InputPin;


class QEI4X : public StepCounter {
	uint32_t qCount;

	typedef union {
		struct {
			uint	enable			: 1;
			uint	swap			: 1;
			uint	stepDir			: 1;
			uint	capture4X		: 1;
			uint	indexReset		: 1;
			uint	captureVelocity	: 1;
			uint	velocityPreDiv	: 3;
			uint	invertA			: 1;
			uint	invertB			: 1;
			uint	invertIndx		: 1;
			uint	stallEnable		: 1;
			uint	filterEnable	: 1;
			uint	reserved		: 2;
			uint	filterPreScale	: 4; } st;
		uint32_t	qeiCtl; } QEIunion;
	
	public:
		QEI4X(
			float		scale );

		~QEI4X();

		virtual float	readScaledCount();
		virtual void	adjust( int32_t adjustment );
};

#endif
