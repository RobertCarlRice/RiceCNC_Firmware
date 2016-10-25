
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

#ifndef TorchHeightControl_h
#define TorchHeightControl_h

#include "Energia.h"
#include "Axis.h"

class	OutputPin;
class	InputPin;
class	AnalogPin;
class	Machine;

class	TorchHeightControl : public Axis {
	public:

	TorchHeightControl(
		char*		data,
		String*		msgPtr,
		Machine*	parent );
    virtual     ~TorchHeightControl();
	// Called automatically after derived class destructor

	void
		setArcVector(
			float		xyArcLength,
			float		ayArcOffset,
			float		azArcOffset );
	
	void
		startPlasma( bool on );

	bool
		arcEstablished();

 	virtual void
		interpolationIsr(
			Machine*	machine );

	virtual bool
		isTorchAxis();

	float		setVoltage;

	protected:

	private:
	OutputPin*		plasmaStart;
	InputPin*		arcTransfer;
	AnalogPin*		arcVoltage;

	Axis*		xAxis;
	Axis*		yAxis;
	Axis*		zAxis;
	Axis*		torchRotationAxis;

	float		xyArcLength;
	float		xArcOffset;
	float		yArcOffset;
	float		zArcOffset;

	float		headRotationAngleRad;
	float		headRotationSin;
	float		headRotationCos;

	float		cuttingAngleRad;

	float		integral;
	float		calabration;
	float		proportionalComponent;
	float		integralComponent;
};

#endif
