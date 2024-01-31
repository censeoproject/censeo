#include <ArduinoBLE.h>




#define LEDPIN 10
// Pin 13: Arduino has an LED connected on pin 13
// Pin 11: Teensy 2.0 has the LED on pin 11
// Pin  6: Teensy++ 2.0 has the LED on pin 6
// Pin 13: Teensy 3.0 has the LED on pin 13

#define SENSORPIN 7
#define REFILLPIN 8
#define buttonPin 9

const int REFILLBUTTON = 2; // the number of the pushbutton pin
const int ledPin = 5;
// variables will change :
int buttonState = 0;
int sensorState = 0, lastState=0;  // variable for reading the pushbutton status
int previousButtonState = 1;
int newQuantity = 4;
bool dispensed = false;

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
	// initialize the digital pin 3 as an output.
	pinMode(REFILLPIN, OUTPUT);
	// initialize the LED pin as an output
	pinMode(ledPin, OUTPUT);
	// initialize the pushbutton pin as an input
	pinMode(REFILLBUTTON, INPUT_PULLUP);

	digitalWrite(SENSORPIN, HIGH); // turn on the pullup
	digitalWrite(REFILLPIN, LOW);	 // turn off the refill led
	digitalWrite(REFILLBUTTON, HIGH);
	BLEDevice peripheral = BLE.available();
  Serial.begin(9600);
  BLE.begin();
  //set advertised local name and service UUID:
  BLE.setLocalName("Pill Dispenser");
  BLE.setAdvertisedService(PillService);
  // add the characteristic to the service
  PillService.addCharacteristic(DispensedCharacteristic);
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
	// read the state of the pushbutton value
	buttonState = digitalRead(REFILLBUTTON);
	// 0 represents button down
	// 1 means button up

	// check if the pushbutton is pressed
	// if it is, the buttonState is HIGH
	if (buttonState == HIGH && previousButtonState == 1)
	{
		// turn LED on:
		digitalWrite(ledPin, HIGH);
		Serial.println("1001|refilled|0");
		
		previousButtonState = 0;
	
		//if (newQuantity <= 5)
		//{
		//	digitalWrite(REFILLPIN, HIGH);
		//}
		//else
		//{
		//	digitalWrite(REFILLPIN, LOW);
		//}
	}
	else if (buttonState == LOW && previousButtonState == 0)
	{
		previousButtonState = 1;

	}
	else
	{
		// turn LED off:
		digitalWrite(ledPin, LOW);
	}

	// check if the sensor beam is broken
	// if it is, the sensorState is LOW:
	
  // read the state of the pushbutton value:
  sensorState = digitalRead(SENSORPIN);

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == LOW) {     
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
  }
  
  if (sensorState && !lastState) {
    Serial.println("Unbroken");
    dispensed = false;
  } 
  if (!sensorState && lastState) {
    Serial.println("Broken");
    dispensed = true;
  }
  lastState = sensorState;

  Serial.println(central);
  Serial.println(central)
  if (central && central.connected()) {
    if (dispensed) {
        DispensedCharacteristic.writeValue((byte)0x01);
        Serial.println("test");
        dispensed = false;
    }
  }
}