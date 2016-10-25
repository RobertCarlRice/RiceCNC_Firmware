
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

#ifndef PowerStepAxis_h
#define PowerStepAxis_h

#include "Energia.h"
#include <Axis.h>
#include <PowerStepDefines.h>

class	PowerStepBuffer;
class	SsiDeviceQueue;
class	OutputPin;


class	PowerStepAxis : public Axis {
	public:

    PowerStepAxis(
		char*		data,
		String*		msgPtr );

    virtual		~PowerStepAxis();

	uint16_t	slewRates[ 4 ]  = { 320, 75, 110, 260 };

    SSI*				ssi;
    SsiDeviceQueue*     ssiDeviceQueue;


	protected: // overridden virtual functions
	
//    virtual bool		isWaitingForMotor();
    virtual bool		usesSSI( SSI* aSsi );
    virtual bool		isConfigured();

    virtual bool
		setTargetVelocityWithData(
			char*		data,
			String*		msgPtr );

    virtual void		interpolationIsrEnd();
    virtual void		reportAxisStatus(	String* msgPtr );
	virtual void		calculateStopVelocity();

	int32_t				microStepStopPointI();


	private:

	PowerStepConfigReg		powerStepConfigReg;
	PowerStepGateConfig1	powerStepGateConfig1;
	PowerStepGateConfig2	powerStepGateConfig2;
	PowerStepAlarmEnable	powerStepAlarmEnable;

    PowerStepBuffer*	gpBuf;
    PowerStepBuffer*	getStatusCR;
    PowerStepBuffer*	getSpeedCR;
    PowerStepBuffer*	getPosCR;
    PowerStepBuffer*	targetSpeedCR;

    void		configure_dSpin( uint32_t, OutputPin*, OutputPin* );
    bool		initialize(  String*  msg );

    // Callbacks
    void		gpDone(      uint32_t data );
    void		newPosition( uint32_t newPosU );
    void		newStatus(   uint32_t newStatus );
    void		newSpeed(    uint32_t newSpeed );
  
    uint32_t	deviceNum;
    uint32_t	runPower;
    uint32_t	accelPower;
    uint32_t	decelPower;
    uint32_t	holdPower;
    uint32_t	overCurrent;
    uint32_t	overCurrentEnable;
    uint32_t	stallDetect;
    uint32_t	lastPosU;
    uint32_t	slewRate;

    uint32_t	configurationFlags;

    int32_t		positionChangeI;
    
    uint32_t	lastReportedSpeed;
    uint16_t	reportedStatus;
    
    bool		configured;

#ifdef DEBUG
// TODO: 
uint32_t	debugTimer;
uint32_t	debugTimer2;
#endif

	void		setTargetVelocityForSeekedPosition();
    void		seekVelocity( float velocity );

	union {
		struct {
			uint datum					: 8;
			uint param					: 8; } st;
		uint16_t	updateReg; } updateRegU;

	PowerStepStatusReg powerStepStatusReg;
};

#endif
