#include <ArduinoBLE.h>

unsigned long counter = 0; 
int aState;
int aLastState;  

#define outputA 2
#define outputB 3


BLEService RotationService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedLongCharacteristic RotationCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify | BLEWrite);

void setup() {
pinMode (outputA,INPUT);
pinMode (outputB,INPUT);
   
 Serial.begin(9600);
 
  // begin initialization
  BLE.begin();
  // set advertised local name and service UUID:
  BLE.setLocalName("Bandage Dispenser");
  BLE.setAdvertisedService(RotationService);
  // add the characteristic to the service
  RotationService.addCharacteristic(RotationCharacteristic);
  // add service
  BLE.addService(RotationService);
  // start advertising
  BLE.advertise();
  Serial.println("BLE Rotation Peripheral, waiting for connections....");
  Serial.setTimeout(50);
}
void loop() {
  
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();{ 
   aState = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aState) { 
       counter ++;
     } else {
       counter --;
     }
     Serial.print("Position: ");
     Serial.println(counter);
   } 
   aLastState = aState; // Updates the previous state of the outputA with the current state
 }
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // while the central is still connected to peripheral:
    while (central.connected()) {
   aState = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aState) { 
       counter ++;
       RotationCharacteristic.writeValue((unsigned long)counter);
     } else {
       counter --;
       RotationCharacteristic.writeValue((unsigned long)counter);
     }
     Serial.print("Position: ");
     Serial.println(counter);
   } 
   aLastState = aState; // Updates the previous state of the outputA with the current state
    }
  
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}