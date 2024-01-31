#include <ArduinoBLE.h>

unsigned long counter = 0; 
int aState;
int aLastState;  
// pin definitions for rotary encoder:
#define outputA 2
#define outputB 3
//pin definitions for reset buttton
const int buttonPin = 5;
int buttonState;
int lastButtonState = LOW;
bool buttonPressed = false;
bool rotated = false;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

BLEService BandageService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedLongCharacteristic RotationCharacteristic("19b10001-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify | BLEWrite);
BLEByteCharacteristic ResetCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify | BLEWrite);

void setup() {  
  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);
  pinMode (buttonPin,INPUT);

  Serial.begin(9600);
 
  // begin initialization
  BLE.begin();
  // set advertised local name and service UUID:
  BLE.setLocalName("Bandage Dispenser");
  BLE.setAdvertisedService(BandageService);
  // add the characteristic to the service
  BandageService.addCharacteristic(RotationCharacteristic);
  BandageService.addCharacteristic(ResetCharacteristic);
  // add service
  BLE.addService(BandageService);
  // start advertising
  BLE.advertise();
  Serial.println("BLE Rotation Peripheral, waiting for connections....");
  Serial.setTimeout(50);
}

void loop() {  
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central(); 
  aState = digitalRead(outputA); // Reads the "current" state of the outputA
  // If the previous and the current state of the outputA are different, that means a Pulse has occured
  if (aState != aLastState) {     
    // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    rotated = true;
    if (digitalRead(outputB) != aState) { 
      counter ++;
    } else {
      counter --;
    }
    Serial.print("Position1: ");
    Serial.println(counter);
  }
  
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

  aLastState = aState; // Updates the previous state of the outputA with the current state

  // if a central is connected to peripheral:
  if (central && central.connected()) {
    if (buttonPressed) {
        ResetCharacteristic.writeValue((byte)0x01);
        buttonPressed = false;
        counter = 0;
    }
    if (rotated) {
      RotationCharacteristic.writeValue((unsigned long)counter);
      rotated = false;
    }
    /*aState = digitalRead(outputA); // Reads the "current" state of the outputA
    // If the previous and the current state of the outputA are different, that means a Pulse has occured
    int reading = digialRead(buttonPin);

    if (aState != aLastState) {     
      // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
      if (digitalRead(outputB) != aState) { 
        counter ++;
        RotationCharacteristic.writeValue((unsigned long)counter);
      } else {
        counter --;
        RotationCharacteristic.writeValue((unsigned long)counter);
         
      }
      Serial.print("Position2: ");
      Serial.println(counter);
      
    }*/
  }
}
