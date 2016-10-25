
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

#ifndef dSpinBuffer_h
#define dSpinBuffer_h

#include <Energia.h>
#include <SsiBuffer.h>

#include <driverlib/interrupt.h>


class dSpinAxis;
class SsiDeviceQueue;

typedef void (dSpinAxis::*dSpinAxisCallback)( uint32_t );  

class DSpinBuffer : public SsiBuffer {
	private:
    dSpinAxis*           target;
    dSpinAxisCallback    callBack;

    void
		getParam(  uint8_t param );

	public:
    // Constructor
    DSpinBuffer (
		SsiDeviceQueue*		aQueue );
    
	// Destructor
	virtual ~DSpinBuffer();

	void
		addCallback (
			dSpinAxis*			aTarget,
			dSpinAxisCallback	aCallBack );

    DSpinBuffer*	queueNext;

    bool			setParam(
		uint8_t		param,
		uint32_t	value,
		String*		msg );

	virtual void
		rxComplete();

    // parameter registers
    void		getMaxSpeed();
    void		getSpeed();
    void		getPosition();
    void		getPhase();
  
    bool
		setMinSpeed(
			float		stepsPerSecF,
			bool		lowSpeedOptimization,
			String*		msgPtr );

    bool
		setMaxSpeed(
			float		stepsPerSecF,
			String*		msgPtr );

    // Full speed is the speed at which the chip stops microstepping and switches to full step (both phases on)
    // to achieve higher motor speeds.
    bool
		setFullSpeed(
			float		stepsPerSecF,
			String*		msgPtr );

    bool
		setAcceleration(
			float		stepsPerSecPerSecF,
			String*		msgPtr );
  
    bool
		setDeceleration(
			float		stepsPerSecPerSecF,
			String*		msgPtr );

    // Power levels use an 8 bit scale, i.e., 255 is max
    bool
		setRunPower(
			uint32_t	powerU,
			String*		msgPtr );

    bool
		setAccelPower(
			uint32_t	powerU,
			String*		msgPtr );

    bool
		setDecelPower(
			uint32_t	powerU,
			String*		msgPtr );

    bool
		setHoldPower(
			uint32_t	powerU,
			String*		msgPtr );

    bool
		setOvercurrentThreshold(
			uint32_t	threshold,
			String*		msgPtr );
    
    bool
		setStallDetectThreshold(
			uint32_t	threshold,
			String*		msgPtr );

    bool
		setMicroStepMode(
			uint32_t	mode,
			String*		msgPtr );

// Fetch and return the 16-bit value in the STATUS register.
    void
		getStatus( bool clear );

    // accelerate the motor to a constant velocity
    void
		setTargetVelocity( float velocity );

    void
		runAtMinSpeed( bool forward );
    
    void
		setPosition( uint32_t position );

    // Bring the motor to a halt using the deceleration curve.
    void
		softStop();

    // Stop the motor immediatly losing position.
    void
		hardStop();

    // Put the bridges in Hi-Z state immediately with no deceleration.
    void
		hardHiZ();

    // Decelerate the motor and put the bridges in Hi-Z state.
    void
		softHiZ();

    // Reset device to power up conditions.
    // Equivalent to toggling the STBY pin or cycling power.
    void
		softReset();  

    // Sets the ABS_POS register to 0, effectively declaring the current
    //  position to be "HOME".
    void
		resetPos();
    /*
      setStepClock() puts the device in external step clocking mode. When active,
      pin 25, STCK, will microstep the device in the requested direction.
      Any motion command (RUN, MOVE, etc) will cause the device
      to exit step clock mode. */
    void
		setStepClock( bool forward );
    /*
      move() will send the motor n_step microsteps (size based on step mode) in the
      direction imposed by dir (FWD or REV constants may be used). The motor
      will accelerate according the acceleration and deceleration curves, and
      will run at MAX_SPEED. Stepping mode will adhere to FS_SPD value, as well. */
    void
		move( int32_t n_step );
    /*
      goTo operates much like MOVE, except it produces absolute motion instead
      of relative motion. The motor will be moved to the indicated position
      in the shortest direction, i.e., ABS_POS register can wrap. */
    void
		goTo( uint32_t pos );
    /*
      goToWithDirection() is same as GOTO, but with user constrained rotational direction. */
    void
		goToWithDirection( int32_t pos );
    /*
      goUntil() will run at requested velocity
      until a falling edge is detected on the SW pin. Depending
      on bit SW_MODE in CONFIG, either a hard stop or a soft stop is
      then performed, and depending on the value of the copy parameter
      either the value in the ABS_POS register is RESET to 0 or
      the ABS_POS register is copied to the MARK register. */
    void
		goUntil( int32_t vel, bool copy );
    /*
      Similar to goUntil, releaseSW() runs at minimum speed (the higher
      of the value in MIN_SPEED or 5 steps/s) until a rising edge
      is detected on the switch input, then a hard stop is performed
      and the ABS_POS register is either copied into MARK or RESET to
      0, depending on the copy parameter. */
    void
		releaseSW( bool forward, bool copy );
    /*
      goHome() is equivalent to goTo(0), but requires less time to send.
      Note that no direction is provided; motion occurs through shortest
      path. If the ABS_POS register may have wraped and a direction is required,
      use GoTo_DIR(). */
    void
		goHome();
    /*
      goMark() is equivalent to GoTo( MARK ), but requires less time to send.
      Note that no direction is provided; motion occurs through shortest
      path. If a direction is required, use GoTo_DIR(). */
    void
		goMark();
 };

#endif
