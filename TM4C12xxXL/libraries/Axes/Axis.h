
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

#ifndef Axis_h
#define Axis_h

#include <Energia.h>
#include <Machine.h>

#include <driverlib/interrupt.h>

class	SSI;
class	Link;
class	LimitSwitch;
class	StepCounter;
class	ClosedLoopCounter;


#define  MinArmatureOffset	0.5
#define  MaxArmatureOffset	1.5

// flags
#define  ANGULAR			1
#define  STATUS_CHANGED		4
#define  VELOCITY_CHANGED	8

class Axis {
	static Axis*	firstMotorStepAxis;

	public:

	Axis(
		char*		data,
		String*		msgPtr );
	uint32_t		parseLength;

    virtual			~Axis(); // Called automatically after derived class destructor

	enum AxisState {
		UNCONFIGURED,
		IDLE,
		SEEKING_POSITION,
		SEEKING_VELOCITY,
		SWITCH_LIMIT_HIGH,
		SWITCH_LIMIT_LOW,
		PROGRAM_FAULT};

	// Motor states are aligned with dSpin motor states
	enum MotorState {	// not really needed
		HOLDING,		// mvStepsPerSecF			== 0.0
		ACCELERATING,	// mvStepsPerSecF			!= targetStepsPerSecondF
		DECELERATING,	// targetStepsPerSecondF	== 0.0
		CONSTANT_SPEED,	// mvStepsPerSecF			== targetStepsPerSecondF
		DISABLED };

    Axis*			nextAxis;

	static void
		addMotorStepInterrupt( Axis* thisAxis );

	static void
		removeMotorStepInterrupt( Axis* thisAxis );

	static void
		motorStepInterrupt();

	void
		addMotorStepInterrupt();

    char			axisNameC;
	char			axisFunctionC;

	volatile float	mvStepsPerSecF;			// motor velocity
	float			maxStepsPerSecF;		// for efficiency

	float			stepsPerUnitDistanceF;
    float			microStepsPerUnitDistanceF;	// Inches, mm or degrees - used to scale for feedrate
    float			rotationalRadiusF;			// used to include angular axis in feedrate calculation

	// Current interpolated position before machine transformations
	int32_t			uStepProgramPositionI;
	float			uStepProgramPositionF;

	float			interpolationMicrostepBuffer;
    volatile float	indexerOffsetF;			// microstep error

    Link*			firstSuperiorLink;
    Link*			firstSubordinateLink;

	StepCounter*		slaveStepCounter;
	ClosedLoopCounter*	closedLoopStepCounter;

	// Need 64 bits to maintain microstep accuracy
	// otherwise will overflow 23 bit float precision
	volatile int32_t	microStepTargetOffsetI;	// micro-steps

    // virtual functions can be overriden by derived class
	virtual bool	isTorchAxis();
    virtual bool	isConfigured();
    virtual bool	is8305();
    virtual bool	usesSSI( SSI* ssi );

    virtual void	updateAxis(
		char*		data,
		String*		msgPtr );

    virtual bool	setTargetVelocityWithData(
		char*		data,
		String*		msgPtr );

	bool
		setTargetVelocity(
			float		newTargetStepsPerSecondF );

	// Axis timer interrupt called at 15kHz rate
    virtual void	motorStepIsr() {};

	// Interpolation timer interrupt start and end
	// called at 2kHz rate
    virtual void	interpolationIsr(
		Machine*	machine );

    virtual void	interpolationIsrEnd();

    virtual void	reportAxisStatus(
		String*		msgPtr );

    virtual void	reportAnalog(
		String*		msgPtr );
	
    bool			angular();
    bool			isSwitchLimited();
    bool			isSeekingPosition();
    bool			velocityChanged();
    bool			statusChanged();

// called on initialization
    void			adjustProgramPosition(
		float		pos );

// called on fixture or tool change
    void			adjustMachinePosition(
		float		offset );
	
	void
		moveMachinePositionAndTarget(
			float microStepsF );

    void			abort();
    void			reportDigital(
		String*		msgPtr );

    void			setFlag(
		uint32_t	flag );

    void			clearFlag(
		uint32_t	flag );

    void			deleteLinks();
    void			deleteSuperiorLinks();
    void			adjustMicroStepTarget(
		float		offset );

	void			updateTimerInterval();

	void			setTargetVelocityForTargetPosition();
	void			calculateTransformedTargets(
		float		microstepDeltaF );

	float			scaledTargetSpeedSquared();
	void			minAndMaxProgress(
		Progress*	progress );

	void			addLimitSwitch(
		char*,		String* );

	void			addSlaveInput(
		char*,		String* );

	void			addSlaveInput();

	void			addClosedLoop(
		char*,		String* );

	virtual void
		addMotor(
			char*		data,
			String*		msgPtr );

	virtual void
		addHallFeedback(
			char*		data,
			String*		msgPtr );

	virtual void
		addQuadratureFeedback(
			char*		data,
			String*		msgPtr,
			char		counterType );

	virtual void
		reportHallPinChange( String* msgPtr ) {};

	virtual void
		addVoltageFeedback(
			char*		data,
			String*		msgPtr );

	virtual void
		addCurrentFeedback(
			char*		data,
			String*		msgPtr );

    void			updateVelocity();

	void			stepped(
		float		microStepsF );

    int32_t			roundToInt(
		float		value );

	protected:
    volatile uint32_t	flags;

	float			stepProgressF;	// ratio of microstep stop distance to target offset

    // ISR variables
    // enums storage size is same as int or 4 bytes on a 32 bit machine. 
    volatile AxisState	axisState;
    volatile MotorState	motorState;
  
    volatile float	targetStepsPerSecondF;	// target motor velocity

    // Configured Dynamics
	uint32_t		microStepU;
	uint32_t		microStepsPerStepU;
	uint32_t		accelStepsPerSecPerSecU;
	uint32_t		decelStepsPerSecPerSecU;
	uint32_t		maxStepsPerSecU;

	// efficiency variables
	float			microStepsPerStepF;
	float			accelStepsPerSecPerMicroSecF;
	float			decelStepsPerSecPerMicroSecF;
	float			decelStepsPerSecPerSecF;

	// Need 64 bits to maintain microstep accuracy
	// otherwise will overflow 23 bit float precision
	volatile float		microStepTargetOffsetF;	// fractional micro-steps

    volatile int32_t	uStepMachinePositionI;	// unreported position delta
    volatile float		uStepMachinePositionF;	// unreported position delta


 	float				microStepStopDisplacement();

    void				setAxisState(
		AxisState	newState );

	void				setMotorState();
    virtual void		motorStateChanged() {};
	virtual void		calculateStopVelocity();

	void				setMicroStep();

	void				runMotor(
		float		stepsPerSecondF );

	void				stopMotor();


	private:

    Axis*				nextMotorStepAxis;

	LimitSwitch*		limitSwLowPin;
    LimitSwitch*		limitSwHighPin;		// ACTIVE_HIGH is limited

	volatile float		accelSpeedStepSizeF;
	volatile float		decelSpeedStepSizeF;
	volatile float		indexerStepSize;
	
	float				decelTime_SecPerStepsPerSecF;

	char				axisStateChar();
	char				motorStateChar();

	bool				anyChange();
	void				switchLimited(
		AxisState );

	void				addToMicroStepTarget(
		float		microStepsOffsetF );

	void				interpolating();
	
	void				moveMachinePosition(
		float		scaledOffset );

	void				moveProgramPosition(
		float		scaledOffset );

	void
		addStepDirectionCounter(
			char*		data,
			String*		msgPtr );

	void
		addQuadratureCounter(
			char*		data,
			String*		msgPtr );

	void
		addHallCounter(
			char*		data,
			String*		msgPtr );
};

#endif
