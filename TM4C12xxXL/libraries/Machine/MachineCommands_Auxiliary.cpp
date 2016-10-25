
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

#include <MachineCommands.h>
#include <Machine.h>
#include <SetCoolant.h>
#include <Coolants.h>
#include <Probe.h>
#include <AnalogPin.h>
#include <InputPin.h>
#include <DoorAjar.h>


void
	Machine::auxilaryControl(
		char*	commandPtr,
		String* msgPtr ) {

	char* data	= commandPtr + 1;

	switch ( *commandPtr ) {

// in-band - acknowledge immediately
		case COOLANT_ON_OFF:
		queueCommand(
			new SetCoolant( data ) );
		*msgPtr
			+=	String( AUXILARY_CONTROL )
			+	String( COOLANT_ON_OFF );
		break;

		case CLAMP_ON_OFF:
		queueCommand(
			new SetCoolant( data ) );
		*msgPtr
			+=	String( AUXILARY_CONTROL )
			+	String( CLAMP_ON_OFF );
		break;

		case AUX_ON_OFF:
		queueCommand(
			new SetCoolant( data ) );
		*msgPtr
			+=	String( AUXILARY_CONTROL )
			+	String( AUX_ON_OFF );
		break;

		case CONFIGURE_COOLANT :
		configureCoolant(
			data,
			msgPtr );
		break;

		case CONFIGURE_PROBE :
		configureProbe(
			data,
			msgPtr );
		break;

		case CONFIGURE_DOOR_AJAR :
		configureDoorAjar(
			data,
			msgPtr );
		break;

		case CLEAR_ANALOG_COMMAND :
		clearAnalogCommand(
			msgPtr );
		break;

		case CREATE_ANALOG :
		createAnalog(
			data,
			msgPtr );
		break;

		case CLEAR_DIGITAL_COMMAND :
		clearDigitalCommand(
			msgPtr );
		break;

		case CREATE_DIGITAL :
		createDigital(
			data,
			msgPtr );
		break;

		default :
		*msgPtr
			+=	*commandPtr
			+	"E Invalid Machine Command - Ignored"; }; };


void
	Machine::clearAnalogCommand(  String* msgPtr ) {

	*msgPtr
		+=	String( AUXILARY_CONTROL )
		+	String( CLEAR_ANALOG_COMMAND );

	clearAnalog(); };


void
	Machine::clearDigitalCommand( String* msgPtr ) {

	*msgPtr
		+=	String( AUXILARY_CONTROL )
		+	String( CLEAR_DIGITAL_COMMAND )
		+	"\n";

	clearDigital(); };


void
	Machine::configureCoolant(
		char*		data,
		String*		msgPtr ) { // This is also used for aux outputs

	*msgPtr
		+=	String( AUXILARY_CONTROL )
		+	String( CONFIGURE_COOLANT );

	uint32_t outputNumber = 0;
	sscanf( data,
		"%lu",
		&outputNumber );
  
	int index = outputNumber - 1;
	if ( index < 0 || index >= AuxOutputs ) {
		*msgPtr		+= " Invalid index";
		return; };

	if ( coolants[ index ] ) {
		delete coolants[ index ];
		coolants[ index ] = NULL; };
  
	Coolant* newCoolant = new Coolant();
	if ( ! newCoolant->validate( data, msgPtr ) ) {
		delete newCoolant;
		snprintf( data, 40,
			" Output %lu Unassigned",
			outputNumber );
		*msgPtr    += String( data ); }

	else {
		coolants[ index ] = newCoolant; }; };


void
	Machine::configureDoorAjar(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( AUXILARY_CONTROL )
		+	String( CONFIGURE_DOOR_AJAR );

	if ( doorAjar )
		delete doorAjar;
  
	doorAjar	= new DoorAjar();
	if ( ! doorAjar->validate( data, msgPtr ) ) {
		delete doorAjar;
		doorAjar	= NULL;
		*msgPtr		+= " No Door Ajar"; }; };


void
	Machine::configureProbe(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( AUXILARY_CONTROL )
		+	String( CONFIGURE_PROBE );

	if ( probe )
		delete probe;
  
	probe = new Probe( data, msgPtr );
	if ( ! probe->isValid() ) {
		delete probe;
		probe		= NULL;
		*msgPtr		+= " No Probe"; }; };


void
	Machine::createAnalog(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( AUXILARY_CONTROL )
		+	String( CREATE_ANALOG );

	AnalogPin* newAnalog	= new AnalogPin( data, msgPtr );
	if ( newAnalog->pin ) {
		uint16_t newID	= newAnalog->id();
		// Now add to chain or replace existing analog with the same pin number
		AnalogPin** analogPtrPtr	= &firstAnalog;
		while ( *analogPtrPtr ) {
			AnalogPin*  analogPtr	= *analogPtrPtr;
			if ( analogPtr->id()  == newID ) {
				newAnalog->nextAnalogPin  = analogPtr->nextAnalogPin;
				delete analogPtr;
				break; }; // replace
			analogPtrPtr     = &analogPtr->nextAnalogPin; };
		*analogPtrPtr    = newAnalog; }

	else
		delete newAnalog; };


void
	Machine::createDigital(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( AUXILARY_CONTROL )
		+	String( CREATE_DIGITAL );

	InputPin* newInputPin     = new InputPin( data, msgPtr );

	if ( newInputPin->valid() ) {
		uint16_t newID = newInputPin->id();
		// Now add to chain or replace existing pin with the same pin number
		InputPin** inputPinPtrPtr   = &firstDigital;
		while ( *inputPinPtrPtr ) {
			InputPin*  inputPinPtr    = *inputPinPtrPtr;
			if ( inputPinPtr->id() == newID ) {
				newInputPin->nextInputPin  = inputPinPtr->nextInputPin;
				delete inputPinPtr;
				break; }; // replace
			inputPinPtrPtr    = &inputPinPtr->nextInputPin; };
		*inputPinPtrPtr   = newInputPin; }

	else
		delete newInputPin; };
