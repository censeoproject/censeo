#include <Arduino.h>
#include <WiFiNINA.h>

#ifndef STASSID
#define STASSID "iPhone"
#define STAPSK "Janner06"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const char *inventoryServerIp = "172.20.10.2";
const unsigned int inventoryServerPort = 5555;

#define LEDPIN 13
// Pin 13: Arduino has an LED connected on pin 13
// Pin 11: Teensy 2.0 has the LED on pin 11
// Pin  6: Teensy++ 2.0 has the LED on pin 6
// Pin 13: Teensy 3.0 has the LED on pin 13

#define SENSORPIN 4

// variables will change:
int sensorState = 0, lastState = 0; // variable for reading the pushbutton status

// include the library code:

// initialize the library with the numbers of the interface pins

WiFiClient client;

void setup()
{
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);
  // initialize the digital pin 12 as an output.
  pinMode(12, OUTPUT);

  digitalWrite(SENSORPIN, HIGH); // turn on the pullup

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected to Wifi.");
  Serial.print("IP:");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // read the state of the pushbutton value:
  sensorState = digitalRead(SENSORPIN);

  if (sensorState == LOW)
  {
    // beam was broken
  }

  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == LOW)
  {
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);
    digitalWrite(11, HIGH);
  }
  else
  {
    // turn LED off:
    digitalWrite(LEDPIN, LOW);
  }

  if (sensorState && !lastState)
  {
    Serial.println("Unbroken");
  }
  if (!sensorState && lastState)
  {
    client.connect(inventoryServerIp, inventoryServerPort);
    Serial.println("Connected to inventory system.");
    Serial.println("dispensed");
    client.println("1001");
    client.println("dispensed");
    client.flush();
    client.stop();
    Serial.print("Disconnected from inventory system.");
  }

  lastState = sensorState;
}