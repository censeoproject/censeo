#include <ArduinoBLE.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int distance = 0; //The distance travelled by the squeezer
int curr = 3; //Red = 1, Yellow = 2, Green = 3
int movedBack = 0; //How many squares the squeezer has moved backwards
//Start strip with red
int debounce = 60;
int lastCheck = 0;
const int buttonPin = 2;
int buttonState;
int lastButtonState = LOW;
bool buttonPressed = false;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

unsigned long netDistance = 0; 
unsigned long prevNetDistance = 0;



BLEService CreamService("19B10006-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedLongCharacteristic DistanceCharacteristic("19b10007-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify | BLEWrite);
BLEByteCharacteristic ResetCharacteristic("19b10008-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify | BLEWrite);

void setup() {  
  BLEDevice peripheral = BLE.available();

  Serial.begin(9600);
 
  // begin initialization
  BLE.begin();
  // set advertised local name and service UUID:
  BLE.setLocalName("Cream Measurer");
  BLE.setAdvertisedService(CreamService);
  // add the characteristic to the service
  CreamService.addCharacteristic(DistanceCharacteristic);
  CreamService.addCharacteristic(ResetCharacteristic);
  // add service
  BLE.addService(CreamService);
  // start advertising
  BLE.advertise();
  Serial.println("BLE Color Peripheral, waiting for connections....");
  Serial.setTimeout(50);

  if (tcs.begin()) {
    //Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
}

void loop() {  
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central(); 

  float red, green, blue;

  tcs.setInterrupt(false);  // turn on LED

  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  tcs.setInterrupt(false);  // turn on LED
  
  if(millis()-lastCheck > debounce){
    tcs.getRGB(&red, &green, &blue);
    tcs.setInterrupt(true);  // turn off LED
  }

  if (int(red) > 150) {
    if (curr == 3 && movedBack == 0) {
      distance++;
      Serial.print("Distance traveled: ");
      Serial.println(distance);
    }
    else if (curr == 3 && movedBack > 0) {
      movedBack--;
      Serial.print("Moved back: ");
      Serial.println(movedBack);
    }
    else if (curr == 2) {
      movedBack++;
      Serial.print("Moved back: ");
      Serial.println(movedBack);
    }
    curr = 1;
  }
  else if (int(green) > 110) {
    if (curr == 2 && movedBack == 0) {
      distance++;
      Serial.print("Distance traveled: ");
      Serial.println(distance);
    }
    else if (curr == 2 && movedBack > 0) {
      movedBack--;
      Serial.print("Moved back: ");
      Serial.println(movedBack);
    }
    else if (curr == 1) {
      movedBack++;
      Serial.print("Moved back: ");
      Serial.println(movedBack);
    }
    curr = 3;
  }
  else if (int(red) > 110 && int(red) < 130 && int(green) > 90 && int(green) < 100) {
    if (curr == 1 && movedBack == 0) {
      distance++;
      Serial.print("Distance traveled: ");
      Serial.println(distance);
    }
    else if (curr == 1 && movedBack > 0) {
      movedBack--;
      Serial.print("Moved back: ");
      Serial.println(movedBack);
    }
    else if (curr == 3) {
      movedBack++;
      Serial.print("Moved back: ");
      Serial.println(movedBack);
    }
    curr = 2;
  }

  netDistance = distance - movedBack;

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

  // if a central is connected to peripheral:
  if (central && central.connected()) {
    if (prevNetDistance != netDistance) {
        DistanceCharacteristic.writeValue(netDistance);
        prevNetDistance = netDistance;
    }
    if(buttonPressed) {
      ResetCharacteristic.writeValue((byte)0x01);
      buttonPressed = false;
    }
  }
}
