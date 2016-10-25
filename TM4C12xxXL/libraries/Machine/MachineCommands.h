
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

#ifndef MachineCommands_h
#define MachineCommands_h

#include "Energia.h"

#define INTERPOLATION_CONTROL		'C'
// INTERPOLATION_CONTROL out-of-band
#define		PAUSE_INTERPOLATION			'P'
#define		CONTINUE_INTERPOLATION		'C'
#define		ABORT_INTERPOLATION			'S'

// INTERPOLATION_CONTROL in-band
#define		PAUSE_COMMAND				'Z'


#define FEEDRATE_CONTROL			'F'
// FEEDRATE_CONTROL out-of-band
#define		FEEDRATE_OVERRIDE			'F'

// FEEDRATE_CONTROL in-band
#define		SET_FEEDRATE				'f'
#define		FEEDRATE_CHANGED			'f'

#define		MAX_INTERPOLATION_SPEED		't'
#define		DWELL						'd'


#define	SPINDLE_CONTROL				'M'
// SPINDLE_CONTROL out-of-band
#define		CONFIGURE_SPINDLE			'm'
#define		SPINDLE_SPEED_OVERRIDE		'S'
#define		SPINDLE_RUN_OVERRIDE		'R'
#define		SPINDLE_DIRECTION_OVERRIDE	'D'

// SPINDLE_CONTROL in-band
#define		SET_SPINDLE_SPEED			's'
#define		SPINDLE_SPEED_CHANGED		's'

#define		SPINDLE_START_STOP			'r'
#define		SPINDLE_RUN_CHANGED			'r'

#define		SET_SPINDLE_DIRECTION		'd'
#define		SPINDLE_DIRECTION_CHANGED	'd'

#define	POSITION_CONTROL			'P'
// Interpolated move in program coordinate space
#define		CREATE_SPLINE				'i'
#define		CREATE_PROBING_SPLINE		'p'
#define		SPLINE_COMPLETED			'X'
#define		ADJUST_PROGRAM_POSITION		'A'	// Not used
#define		TRACK_PAD_JOG				'j'	// Jog in program coordinate space
// Interpolated move in machine coordinate space
#define		TRAVERSE_MACHINE			't'
#define		TRACK_PAD_JOG_MACHINE		'u'	// Jog in machine coordinate space

#define		SET_AXIS_VELOCITY			'r'
#define		ENTER_SLAVE_MODE			'S'


#define	AXIS_CONTROL				'A'
#define		REMOVE_AXIS_COMMAND			'k'
#define		CONFIGURE_AXIS				'l'
#define			AXIS_TYPE_STEP_DIR			'S'
#define			AXIS_TYPE_DRV8711			'8'
#define			AXIS_TYPE_DRV8711_SPLIT_DC	'D'
#define			AXIS_TYPE_DRV8305			'B'
#define			AXIS_TYPE_dSPIN				'd'
#define			AXIS_TYPE_POWER_STEP		'P'
#define			AXIS_TYPE_DISCRETE			'D'
#define			AXIS_TYPE_QUADRATURE		'Q'
#define			AXIS_TYPE_THREE_PHASE		'3'
#define			AXIS_TYPE_TORCH				'T'
#define		ADD_LIMIT_SWITCH			's'
#define		ADD_SLAVE_INPUT				'S'
#define			COUNTER_TYPE_STEP_DIRECTION	'S'
#define			COUNTER_TYPE_QUADRATURE		'Q'
#define			COUNTER_TYPE_HALL			'H'
#define		ADD_CLOSED_LOOP				'c'
#define			FEEDBACK_TYPE_QUADRATURE_4X	'Q'
#define			FEEDBACK_TYPE_QUADRATURE_1X	'q'
#define			FEEDBACK_TYPE_HALL			'H'
#define			FEEDBACK_TYPE_VOLTAGE_SENSE	'V'
#define			FEEDBACK_TYPE_CURRENT_SENSE	'C'
#define			FEEDBACK_TYPE_MOTOR_VOLTAGE	'M'
#define		ADD_MOTOR					'm'
#define			MOTOR_TYPE_BLDC3PWM			'w'
#define			MOTOR_TYPE_BLDC3PFM			'f'
#define		UPDATE_AXIS					'g'
#define		ADJUST_AXIS_POSITION		'a'  // doesn't run motor
#define		CANCEL_TRANSFORMS			'C'
#define		LINK_AXIS					'L'
#define		SET_ROTATIONAL_DIAMETER		'R'
#define		SET_HALL_PHASING			'H'
#define		SYNCHRONIZE_QUADRATURE_CTR	'Q'


#define	AUXILARY_CONTROL			'X'
#define		CONFIGURE_DOOR_AJAR			'j'

#define		CONFIGURE_COOLANT			'n'
#define		COOLANT_ON_OFF				'k'
#define		CLAMP_ON_OFF				'q'
#define		AUX_ON_OFF					'z'
#define		CONFIGURE_PROBE				'o'

#define		CLEAR_ANALOG_COMMAND		'b'
#define		CREATE_ANALOG				'c'
#define		ANALOG_REPORT				'a'

#define		CLEAR_DIGITAL_COMMAND		'e'
#define		CREATE_DIGITAL				'f'
#define		DIGITAL_REPORT				'd'


#define	TORCH_CONTROL				'T'
#define		SET_ARC_VECTOR				'v'
#define		START_PLASMA				't'


#define INTERPOLATOR_STATUS_REPORT	'R'
#define		INTERPOLATOR_IDLE			'i'
#define		INTERPOLATOR_RUNNING		'r'
#define		INTERPOLATOR_PAUSED			'p'
#define		INTERPOLATOR_DWELLING		'd'
#define		INTERPOLATOR_STOPPED		's'
#define		INTERPOLATOR_DOOR_AJAR		'j'
#define		INTERPOLATOR_ABORTING		'a'
#define		INTERPOLATOR_LIMITED		'l'
#define		INTERPOLATOR_NOT_OK			'n'
#define		INTERPOLATOR_SLAVE			'x'
#define		INTERPOLATOR_STALLED		'S'
#define		INTERPOLATOR_CHECKING_HALL_SENSORS				'H'
#define		INTERPOLATOR_SYNCHRONIZING_QUADRATURE_COUNTER	'Q'


#define	STATUS_REPORT				'S'
#define		VELOCITY					'v'
#define		MACHINE_POSITION_CHANGE		'q'
#define		PROGRAM_POSITION_CHANGE		'p'
#define		MICROSTEP_MODE_CHANGE		'm'
#define		DRV8711_STATUS				's'
#define		POT_ANALOG					'a'
#define		DRV8305_STATUS				's'
#define		DRV8305_VOLTAGE				'V'
#define		DRV8305_STATOR_ANGLE		'<'
#define		DRV8305_LEAD_ANGLE			'L'
#define		DRV8305_CURRENT				'c'
#define		DRV8305_HALL_ERROR			'e'
#define		DRV8305_INTEGRAL_ERROR		'E'
#define		DRV8305_COUNTER_ADJUSTMENT	'X'
#define		DRV8305_FEEDBACK_TORQUE		't'
#define		DRV8305_WATCHDOG_REGISTER	'w'
#define		DRV8305_OVERVOLTAGE_FAULTS	'o'
#define		DRV8305_IC_FAULTS			'i'
#define		DRV8305_GATE_DRIVER_FAULTS	'g'
// Axis States
#define		AXIS_UNCONFIGURED			'u'
#define		AXIS_IDLE					'i'
#define		AXIS_SEEKING_POSITION		'p'
#define		AXIS_SEEKING_VELOCITY		'v'
#define		AXIS_SWITCH_LIMITED_HIGH	'h'
#define		AXIS_SWITCH_LIMITED_LOW		'l'
#define		AXIS_PROGRAM_FAULT			'F'
// 8305 Axis States
#define		AXIS_RUNNING				'r'
#define		AXIS_VOLTAGE_LIMITED		'l'
#define		AXIS_CURRENT_LIMITED		'c'
#define		AXIS_STALLED				'0'
#define		AXIS_HALL_UNITIALIZED		'U'
#define		AXIS_QUADRATURE_UNITIALIZED	'Q'
#define		AXIS_ACCELERATION_LIMITED	'a'
#define		AXIS_DECELERATION_LIMITED	'd'
#define		AXIS_POWER_SUPPLY_LIMITED	'p'
// Motor States
#define		MOTOR_HOLDING				'H'
#define		MOTOR_ACCELERATING			'A'
#define		MOTOR_DECELERATING			'D'
#define		MOTOR_CONSTANT_SPEED		'C'


// machine handshaking
#define HANDSHAKE_CONTROL			'W'
#define		SET_ALARM					'a'
#define		SET_WATCHDOG				's'
#define		SET_MACHINE_OK				'y'


#endif
