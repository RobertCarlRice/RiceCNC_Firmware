
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

#include <TorchHeightControl.h>
#include <OutputPin.h>
#include <ADC.h>
#include <Machine.h>
#include <AnalogPin.h>
#include <InputPin.h>


TorchHeightControl::TorchHeightControl(
	char*		data,
	String*		msgPtr,
	Machine*	machine )

	:	Axis(
			data,
			msgPtr ) {
	
	char        plasmaStartString[8];
	char        arcTransferString[8];
	char        arcVoltageString[8];

	char		xAxisName				= 'X';
	char		yAxisName				= 'Y';
	char		zAxisName				= 'Z';
	char		torchRotationAxisName	= 'N';
	char		torchTiltAxisName		= 'N';

	proportionalComponent		= 0.0;
	integralComponent			= 0.0;

	xyArcLength					= 0.0;
	float headRotationAngle		= 90.0;

	zArcOffset					= 0.0;

	sscanf( data+3,
		"%s %s %s %c %c %c %c %c %f %f %f %f %f",
		plasmaStartString,
		arcTransferString,
		arcVoltageString,
		&xAxisName,
		&yAxisName,
		&zAxisName,
		&torchRotationAxisName,
		&torchTiltAxisName,
		&proportionalComponent,
		&integralComponent,
		&xyArcLength,
		&headRotationAngle,
		&zArcOffset );
	
	if ( zArcOffset == 0.0 ) {
		*msgPtr		+= " No Z offset for arc vector";
		return; };

	// validate
	xAxis			= machine->findAxis( xAxisName, msgPtr );
	yAxis			= machine->findAxis( yAxisName, msgPtr );
	zAxis			= machine->findAxis( zAxisName, msgPtr );

	if ( ! xAxis || ! yAxis || ! zAxis )
		return;
	
	if ( torchRotationAxisName == 'N' )
		torchRotationAxis	= NULL;
	else
		torchRotationAxis	= machine->findAxis(
			torchRotationAxisName,
			msgPtr );

	plasmaStart     = new OutputPin( plasmaStartString, msgPtr );
	arcTransfer     = new InputPin( arcTransferString, msgPtr );
	arcVoltage		= new AnalogPin( arcVoltageString, msgPtr );
 
	if (	! plasmaStart->valid()
		||	! arcTransfer->valid()
		||	! arcVoltage->valid() )	{

		delete plasmaStart;
		plasmaStart		= NULL;
      
		delete arcTransfer;
		arcTransfer		= NULL;
      
		delete arcVoltage;
		arcVoltage		= NULL;
		
		return; };
	
	plasmaStart->setPin( false);

	*msgPtr		+= " Start";
	plasmaStart->fmtPin( msgPtr );

	*msgPtr		+= " Transfer";
	arcTransfer->fmtPin( msgPtr );

	*msgPtr		+= " Voltage";
	arcVoltage->fmtPin( msgPtr );
	
	ADC::addAnalogPin( arcVoltage );

	headRotationAngleRad	=
		headRotationAngle
		* M_PI
		/ 180.0;
	headRotationSin			= sin( headRotationAngleRad );
	headRotationCos			= cos( headRotationAngleRad );
	xArcOffset				= xyArcLength * headRotationSin;
	yArcOffset				= xyArcLength * headRotationCos;
	
	integral				= 0.0;
	setVoltage				= 0.0;
	cuttingAngleRad			= 0.0; };


TorchHeightControl::~TorchHeightControl() {
	// Called automatically after derived class destructor
	if ( plasmaStart )
		delete plasmaStart;

	if ( arcTransfer )
		delete arcTransfer;

	if ( arcVoltage )
		delete arcVoltage; };

void
	TorchHeightControl::setArcVector(
		float		axyArcLength,
		float		aHeadRotationAngleRad,
		float		azArcOffset ) {

	xyArcLength				= axyArcLength;
	headRotationAngleRad	= aHeadRotationAngleRad;
	headRotationSin			= sin( headRotationAngleRad );
	headRotationCos			= cos( headRotationAngleRad );
// Move to new position
//	xArcOffset				= xyArcLength * headRotationSin;
//	yArcOffset				= xyArcLength * headRotationCos;
	};

bool
	TorchHeightControl::isTorchAxis() {

	return true; };

void
	TorchHeightControl::startPlasma( bool on ) {

	if ( plasmaStart )
		plasmaStart->setPin( on ); };

bool
	TorchHeightControl::arcEstablished() {
	
	return arcTransfer ?
		arcTransfer->readPin() :
		false; };

void
	TorchHeightControl::interpolationIsr(
		Machine*	machine ) {

	bool	arcLengthChaged		= false;
	bool	headAngleChanged	= false;
	
	if ( arcEstablished() ) {
		float pV			= arcVoltage->analogValue();
		float error			= setVoltage - pV;
		integral			+= error;
		
		float arcLengthScale	=
			error				* proportionalComponent +
			integral			* integralComponent;

		if ( arcLengthScale != 1.0 ) {
			xyArcLength			*= arcLengthScale;
			arcLengthChaged	= true;

			float newZArcOffset		= zArcOffset * arcLengthScale;
			zAxis->interpolationMicrostepBuffer	+=
				newZArcOffset - zArcOffset;
			zArcOffset			= newZArcOffset; }; };

	float	deltaX			= xAxis->interpolationMicrostepBuffer;
	float	deltaY			= yAxis->interpolationMicrostepBuffer;

	if ( deltaX || deltaY ) {
		float newCuttingAngleRad	= atan2( deltaY, deltaX );
		float deltaAngleRad			= newCuttingAngleRad - cuttingAngleRad;
		if ( deltaAngleRad ) {
			if ( torchRotationAxis ) // rotate the head
				torchRotationAxis->interpolationMicrostepBuffer +=
					deltaAngleRad
					* torchRotationAxis->microStepsPerUnitDistanceF;
			cuttingAngleRad			=  newCuttingAngleRad;

			headRotationAngleRad	+= deltaAngleRad;
			headAngleChanged		= true;
			headRotationSin			= sin( headRotationAngleRad );
			headRotationCos			= cos( headRotationAngleRad ); }; };

	if ( arcLengthChaged || headAngleChanged ) {
		float newxArcOffset		= xyArcLength * headRotationSin;
		xAxis->interpolationMicrostepBuffer	+=
			newxArcOffset - xArcOffset;
		xArcOffset				= newxArcOffset;

		float newyArcOffset		= xyArcLength * headRotationCos;
		yAxis->interpolationMicrostepBuffer	+=
			newyArcOffset - yArcOffset;
		yArcOffset				= newyArcOffset; }; };

