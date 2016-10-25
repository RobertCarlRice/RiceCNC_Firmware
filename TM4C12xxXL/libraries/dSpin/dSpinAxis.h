
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

#ifndef dSpinAxis_h
#define dSpinAxis_h

#include "Energia.h"
#include <Axis.h>

class	DSpinBuffer;
class	SsiDeviceQueue;
class	OutputPin;


class	dSpinAxis : public Axis {
	public:

    dSpinAxis( char* data, String* msgPtr );

    virtual		~dSpinAxis();

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

    DSpinBuffer* gpBuf;
    DSpinBuffer* getStatusCR;
    DSpinBuffer* getSpeedCR;
    DSpinBuffer* getPosCR;
    DSpinBuffer* targetSpeedCR;

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

	union {
		struct {
			uint hiZ					: 1;
			uint notBusy				: 1;
			uint switchClosed			: 1;
			uint switchOpenedLatched	: 1;
			uint directionForward		: 1;
			MotorState motorState		: 2;
			uint lastCmdNotPerformed	: 1;
			uint lastCmdInvalid			: 1;
			uint notUVLO				: 1;
			uint notThermalWarning		: 1;
			uint notThermalShutdown		: 1;
			uint notOverCurrentDisabled	: 1;
			uint notStepLossA			: 1;
			uint notStepLossB			: 1;
			uint notStepClockMode		: 1; } st;
		uint16_t	dSpinStatusReg; } dSpinStatusRegU;

};

#endif
