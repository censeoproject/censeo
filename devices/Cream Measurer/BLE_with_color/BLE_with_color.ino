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

unsigned long netDistance = 0; 
unsigned long prevNetDistance = 0;

BLEService CreamService("19B10006-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedLongCharacteristic DistanceCharacteristic("19b10007-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify | BLEWrite);

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

  // if a central is connected to peripheral:
  if (central && central.connected()) {
    if (prevNetDistance != netDistance) {
        DistanceCharacteristic.writeValue(netDistance);
        prevNetDistance = netDistance;
    }
  }
}
