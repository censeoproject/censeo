//Bluetooth library
#include <ArduinoBLE.h>




//Pin definitions
#define SENSORPIN 2
#define ConnectedLED 3
#define DispensedLED 4
#define ResetLED 5
#define buttonPin 6



// variables will change :
int buttonState;
int lastButtonState = LOW;
bool buttonPressed = false;
int sensorState = 0, lastState=0;  
bool dispensed = false;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Add characteristics for bluetooth
BLEService PillService("19B10003-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
// BLE Dispensed Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic DispensedCharacteristic("19b10004-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify | BLEWrite);
BLEByteCharacteristic QuantityCharacteristic("19b10006-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify | BLEWrite);
BLEByteCharacteristic PillResetCharacteristic("19b10005-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify | BLEWrite);


void setup() {
  //Serial start:
  Serial.begin(9600);

  //Pin modes & initial digital writes:
	pinMode(SENSORPIN, INPUT_PULLUP);
  pinMode(ConnectedLED, OUTPUT);
  pinMode(DispensedLED, OUTPUT);
  pinMode(ResetLED, OUTPUT);

	digitalWrite(SENSORPIN, HIGH); // turn on the pullup

  //Low energy Peripheral Bluetooth device start
	BLEDevice peripheral = BLE.available();
  BLE.begin();
  //set advertised local name and service UUID:
  BLE.setLocalName("Pill Dispenser");
  BLE.setAdvertisedService(PillService);
  // add the characteristic to the service
  PillService.addCharacteristic(DispensedCharacteristic);
  PillService.addCharacteristic(QuantityCharacteristic);
  PillService.addCharacteristic(PillResetCharacteristic);
  
  // add service
  BLE.addService(PillService);
  // start advertising
  BLE.advertise();
  Serial.println("BLE Pill Dispenser Peripheral, waiting for connections....");
  Serial.setTimeout(50);

}

void loop(){

  BLEDevice central = BLE.central(); 
	// read the state of the pushbutton value:
	sensorState = digitalRead(SENSORPIN);

	// check if the pushbutton is pressed
	// if it is, the buttonState is HIGH
	 int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        buttonPressed = true;
        Serial.println("11Button Pressed!");
      }
    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;


  if (sensorState && !lastState) {
    Serial.println("Unbroken");
    dispensed = false;
    
    byte quantity = 0;
    QuantityCharacteristic.readValue(quantity);
    Serial.print("quantity = ");
    Serial.println(quantity);

    if (quantity < 40) {
      digitalWrite(ResetLED, HIGH);
    }
    else{
      digitalWrite(ResetLED, LOW);
    }
  } 
  if (!sensorState && lastState) {
    Serial.print("Broken, ");
    dispensed = true;
  }
 
  lastState = sensorState;


  if (central && central.connected()) {

    digitalWrite(ConnectedLED, HIGH);
    

    if (dispensed) {
      DispensedCharacteristic.writeValue((byte)0x01);
      dispensed = false;
    }

    if(buttonPressed) {
      PillResetCharacteristic.writeValue((byte)0x01);
      buttonPressed = false;
    }
    
  }
}