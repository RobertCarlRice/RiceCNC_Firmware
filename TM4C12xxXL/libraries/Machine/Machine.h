
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

#ifndef Machine_h
#define Machine_h

#include "Energia.h"
#include "driverlib/interrupt.h"

class	SSI;
class	Spindle;
class	Probe;
class	Coolant;
class	DoorAjar;
class	MachineOK;
class	Axis;
class	Command;

class	OutputPin;
class	InputPin;
class	AnalogPin;


#define			AuxOutputs             6 // includes two coolants

// sendFlags bits
#define			SendAxisUnconfigured   0x1

typedef void	(Axis::*BroadcastToAxes)();
typedef bool	(Axis::*Detect)();
typedef void    (Axis::*CollectMessage)( String* );

typedef struct {
	float		minimum;
	float		maximum; } Progress;

typedef struct {
	uint32_t	number;
	float		t;
	char		status;
	Progress	progress; } StepControl;

class Machine {

	public:
    Machine(
		char*		data,
		String*		msgPtr );
    ~Machine();

	static Machine* firstMachine;

	static void
		deleteAllAxesUsingSsi(
			SSI* ssi );

    void			background(
		String* );
    void			processCommand(
		char*,
		String* );
    
    void			interpolationISR();     // Interrput Service Call
    void			deleteSSI(
		SSI* );

  // Modal variables
    float			sqFeedrateIPS;          // IPS squared for efficiency
  // Non-modal variables
    float			maxInterpolationSpeed;  // 1 / time specification

    Spindle*		spindle;
    Probe*			probe;
	DoorAjar*		doorAjar;
    MachineOK*		machineOK;

	void			adjustStepSize(
		StepControl* stepControl );

	void			minAndMaxProgress(
		Progress* progress ); // Check progress on current interpolation

	void			reportInterpolationStatus(
		StepControl*	stepControl,
		String*			msgPtr );

	void			reportSplineCompleted(
		StepControl*	stepControl,
		String*			msgPtr );

    bool			isSeekingPosition();

    void			cancelTransforms();

    Axis*			findAxis(
		char		axisName,
		String*		msgPtr );

    Coolant*		coolants[
		AuxOutputs ]; // 0..1 are coolants, 2..6 are aux outputs
    
    void			setSendFlag(
		uint32_t flag );
      
    bool			detect(
		Detect method );
	
    void			broadcastToAxes(
		BroadcastToAxes method );

    void			collectMessage(
		CollectMessage	method,
		String*			msgPtr );

    void			reportInterpolationStatus(
		String* );
    void			reportAxisStatus(
		String* );
	void			stallDetected();

	float			scaledTargetSpeedSquared();


	protected:


	private:
    Command*		firstCommandInQueue;
    Command*		currentCommand;        // Execution queue pointer
  
    Axis*			firstAxis;
    AnalogPin*		firstAnalog;           // Analog inputs
    InputPin*		firstDigital;          // Digital inputs
  
    OutputPin*		motorPowerPin;
	OutputPin*		alarmPin;
	OutputPin*		watchdogPin;

    volatile uint32_t  sendFlags;

    // interface periodic
    uint32_t		statusReportingInterval;     // micro-seconds
    uint32_t		lastStatusTime;              // micro-seconds
	bool			interpolationBusy;
	float			stepSize;

  // Destructors
    void			clearAnalog();
    void			clearDigital();
    void			removeAxis(
		char,
		String* );
    void			removeLinksForAxis( Axis* );
    
    // Out-of-band Commands
    void			clearAnalogCommand(
		String* );
    void			clearDigitalCommand(
		String* );
    void			pause();
    void			continueInterpolation();
    void			abort();

	void			insertAtTopBG(
		Command* command ); // Insert in-band at top-of-queue

	void			insertAtTop(
		Command* command ); // Insert in-band at top-of-queue

	void			feedrateControl(
		char*,
		String* );
    void			feedrateOverride(
		char*,
		String* );

	void			spindleControl(
		char*,
		String* );
    void			spindleSpeedOverride(
		char*,
		String* );
    void			spindleRunOverride(
		char*,
		String* );
    void			spindleDirectionOverride(
		char*,
		String* );
    
	void			torchControl(
		char*,
		String* );
	void			auxilaryControl(
		char*,
		String* );
	void			axisControl(
		char*,
		String* );
	void			positionControl(
		char*,
		String* );
	void			handshakeControl(
		char*,
		String* );
	void			interpolationControl(
		char*,
		String* );

    // In-band commands
    void			configureSpindle(
		char*,
		String* );
    void			configureProbe(
		char*,
		String* );
	void			configureDoorAjar(
		char*,
		String* );
	void			configureAlarm(
		char*,
		String* );
    void			configureCoolant(
		char*,
		String* );
    void			configureWatchdog(
		char*,
		String* );
    void			configureMachineOK(
		char*,
		String* );
	void			enterSlaveMode(
		char*,
		String* );
    void			createSpline(
		char*,
		String* );
	void			createProbingSpline(
		char*,
		String* );
    void			createMachineSpline(
		char*,
		String* );
    void			trackPadJog(
		char*,
		String* );
    void			trackPadJogMachine(
		char*,
		String* );
    void			configureAxis(
		char*,
		String* );
	void			updateAxis(
		char*,
		String* );
    void			createAnalog(
		char*,
		String* );
    void			createDigital(
		char*,
		String* );
    void			removeAxisCommand(
		char*,
		String* );
    void			setVelocity(
		char*,
		String* );
    
    void			reportAnalog(
		String* );
    void			reportDigital(
		String* );
    
    void			periodicReporting(
		String* );
    void			queueCommand(
		Command* );
    void			createSplineAxis(
		char* );
    
    bool			checkSendFlag(
		uint32_t );

	void			checkDoorAjar();
	void			checkMachineOK();
	void			checkLimits();

	void			addLimitSwitch(
		char*,
		String* );
	void			addSlaveInput(
		char*,
		String* );
	void			addClosedLoop(
		char*,
		String* );
	void			addMotor(
		char*,
		String* );
};

#endif
