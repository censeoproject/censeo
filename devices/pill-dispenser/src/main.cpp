#include <Arduino.h>


#define LEDPIN 13
  // Pin 13: Arduino has an LED connected on pin 13
  // Pin 11: Teensy 2.0 has the LED on pin 11
  // Pin  6: Teensy++ 2.0 has the LED on pin 6
  // Pin 13: Teensy 3.0 has the LED on pin 13

#define SENSORPIN 4

// variables will change:
int sensorState = 0, lastState=0;         // variable for reading the pushbutton status
int pills = 50;

// include the library code:

// initialize the library with the numbers of the interface pins

  
void setup2() {
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);      
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);     
  // initialize the digital pin 12 as an output.
  pinMode(12, OUTPUT);
  
  
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup
  
  Serial.begin(115200);
}

void loop2(){
  // read the state of the pushbutton value:
  sensorState = digitalRead(SENSORPIN);
 
  if (sensorState == LOW) {
    // beam was broken

  }

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == LOW) {     
    // turn LED on:
    digitalWrite(LEDPIN, HIGH); 
    digitalWrite(16, HIGH);
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
  }
  
  if (sensorState && !lastState) {
    Serial.println("Unbroken");
  } 
  if (!sensorState && lastState) {
    Serial.println("Broken");
    pills = pills - 1;
    Serial.print("pills = ");
    Serial.println(pills);
    
  }
  lastState = sensorState;
}