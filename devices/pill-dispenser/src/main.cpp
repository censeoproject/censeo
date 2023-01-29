#include <Arduino.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "shouji"
#define STAPSK "r3m3mb3r"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const char *inventoryServerIp = "172.20.10.2";
const unsigned int inventoryServerPort = 5555;
WiFiUDP Udp;

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
	// initialize the digital pin 3 as an output.
	pinMode(REFILLPIN, OUTPUT);
	// initialize the LED pin as an output
	pinMode(ledPin, OUTPUT);
	// initialize the pushbutton pin as an input
	pinMode(REFILLBUTTON, INPUT_PULLUP);

	digitalWrite(SENSORPIN, HIGH); // turn on the pullup
	digitalWrite(REFILLPIN, LOW);	 // turn off the refill led
	digitalWrite(REFILLBUTTON, HIGH);
	Serial.begin(9600);
	while (!Serial)
		; // only works on 33 IOT

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
	Udp.begin(5555);
}

void loop()
{
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
		Udp.beginPacket(inventoryServerIp, inventoryServerPort);
		Serial.println("Begin packet");
		Serial.println("1001|refilled|0");
		Udp.println("1001|refilled|0");
		Udp.endPacket();
		previousButtonState = 0;
		while (!Udp.parsePacket())
		{
			Serial.print('.');
			delay(100);
		}
		String msg = Udp.readString();
		Serial.print("msg = ");
		Serial.println(msg);
		int newQuantity = msg.toInt();
		if (newQuantity <= 5)
		{
			digitalWrite(REFILLPIN, HIGH);
		}
		else
		{
			digitalWrite(REFILLPIN, LOW);
		}
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
		Udp.beginPacket(inventoryServerIp, inventoryServerPort);
		Serial.println("Begin dispense packet");
		Serial.println("1001|dispensed|0");
		Udp.println("1001|dispensed|0");
		Udp.endPacket();
		while (!Udp.parsePacket())
		{
			Serial.print('.');
			delay(100);
		}
		String msg = Udp.readStringUntil('\n');
		Serial.print("msg = ");
		Serial.println(msg);
		int newQuantity = msg.toInt();
		if (newQuantity <= 5)
		{
			digitalWrite(REFILLPIN, HIGH);
		}
		else
		{
			digitalWrite(REFILLPIN, LOW);
		}
		// Udp.stop();
		// Serial.print("Udp.stop()");
	}

	lastState = sensorState;
}