#include <ArduinoBLE.h>




#define LEDPIN 10
// Pin 13: Arduino has an LED connected on pin 13
// Pin 11: Teensy 2.0 has the LED on pin 11
// Pin  6: Teensy++ 2.0 has the LED on pin 6
// Pin 13: Teensy 3.0 has the LED on pin 13

#define SENSORPIN 7



const int ledPin = 5;
// variables will change :
const int buttonPin = 6;
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
BLEByteCharacteristic PillResetCharacteristic("19b10005-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify | BLEWrite);


void setup() {
 
	// initialize the LED pin as an output:
	pinMode(LEDPIN, OUTPUT);
	// initialize the sensor pin as an input:
	pinMode(SENSORPIN, INPUT);
	// initialize the digital pin 12 as an output.
	pinMode(12, OUTPUT);
	// initialize the LED pin as an output
	pinMode(ledPin, OUTPUT);
	// initialize the pushbutton pin as an input

  pinMode (buttonPin,INPUT);

	digitalWrite(SENSORPIN, HIGH); // turn on the pullup

	BLEDevice peripheral = BLE.available();
  Serial.begin(9600);
  BLE.begin();
  //set advertised local name and service UUID:
  BLE.setLocalName("Pill Dispenser");
  BLE.setAdvertisedService(PillService);
  // add the characteristic to the service
  PillService.addCharacteristic(DispensedCharacteristic);
  PillService.addCharacteristic(PillResetCharacteristic);
  //PillService.addCharacteristic(PillResetCharacteristic);
  // add service
  BLE.addService(PillService);
  // start advertising
  BLE.advertise();
  Serial.println("BLE Pill Dispenser Peripheral, waiting for connections....");
  Serial.setTimeout(50);

}

void loop()
{
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
        Serial.println("11Button Pressed!");
        buttonPressed = true;
      }
    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;


  if (sensorState && !lastState) {
    Serial.println("Unbroken");
    dispensed = false;
  } 
  if (!sensorState && lastState) {
    Serial.println("Broken");
    dispensed = true;
  }
  lastState = sensorState;

  if (central && central.connected()) {
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