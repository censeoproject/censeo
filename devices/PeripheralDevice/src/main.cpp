#include <ArduinoBLE.h>

#define LEDPIN 13
// Pin 13: Arduino has an LED connected on pin 13
// Pin 11: Teensy 2.0 has the LED on pin 11
// Pin  6: Teensy++ 2.0 has the LED on pin 6
// Pin 13: Teensy 3.0 has the LED on pin 13

#define SENSORPIN 4
#define REFILLPIN 3
#define buttonPin 2

const int REFILLBUTTON = 2; // the number of the pushbutton pin
const int ledPin = 5;
// variables will change :
int buttonState = 0;
int sensorState = 0, lastState = 0; // variable for reading the pushbutton status
int previousButtonState = 1;

// initialize the library with the numbers of the interface pins
//Define Bluetooth Uuid
BLEService PillcountService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic PillcountCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify | BLEWrite);






void setup()
{
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

	//digitalWrite(SENSORPIN, HIGH); // turn on the pullup
	//digitalWrite(REFILLPIN, LOW);	 // turn off the refill led
	//digitalWrite(REFILLBUTTON, HIGH);
	Serial.begin(9600);
	pinMode(buttonPin, INPUT_PULLUP);
	BLE.begin();
	BLE.setLocalName("Pill Dispenser");
	BLE.setAdvertisedService(PillcountService);
	PillcountService.addCharacteristic(PillcountCharacteristic);
	BLE.addService(PillcountService);
	BLE.advertise();
	Serial.println("Pill dispenser Peripheral, waiting for connections...");
	//{
	//	delay(500);
		//Serial.print(".");
//	}
}
void loop()
{
	// read the state of the pushbutton value:
	//sensorState = digitalRead(SENSORPIN);
	// read the state of the pushbutton value
	//buttonState = digitalRead(REFILLBUTTON);
	// 0 represents button down
	// 1 means button up
    BLEDevice central = BLE.central();
	if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // while the central is still connected to peripheral:
    //while (central.connected()) {

   //   int buttonState = digitalRead(buttonPin);

    //  if (buttonState == LOW) {
     //   PilldispenserSwitch = !ledSwitch;
    //    delay(500);

    //    if (ledSwitch) {
    //      Serial.println("ON");
    //      LEDCharacteristic.writeValue((byte)0x01);
     //   }
    //    else {
    //      LEDCharacteristic.writeValue((byte)0x00);
    //      Serial.println("OFF");
    //    }
    //  }
   // }
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}