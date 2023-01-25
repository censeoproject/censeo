#include <WiFiNINA.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "iPhone"
#define STAPSK  "r3m3mb3r"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// UDP
WiFiUDP UDP;
char packet[255];
char reply[] = "Packet received!";

IPAddress remoteIP(172,20,10,5);
const char* phoneIp = "172.20.10.5";
const unsigned int phonePort = 7777;
unsigned int remotePort = 5556;
WiFiUDP udp;

void setup(){
  Serial.begin(115200);
  Serial.println("****************");
	// Connect to WiFi network
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

	Serial.print("connected.");
	Serial.print("IP:");
	Serial.println(WiFi.localIP());

  //Serial.println("Puerto Serial OK");
  //Ethernet.begin(mac,ip);
  //Serial.print("IP : ");
  //Serial.println(Ethernet.localIP());
  udp.begin(3456);
}
void loop() {
  int pingResult = WiFi.ping(phoneIp);
  WiFiClient client;
  client.connect("172.20.10.5", 7777);
  client.println("hello!");
  Serial.println("hello!");
  client.flush();
  client.stop();
  //Serial.println("pingResult:");
  //Serial.println(pingResult);
	//Serial.println("beginPacket:"); 
  //Serial.println(udp.beginPacket(phoneIp, phonePort));
  //udp.write("dispense");
	//udp.println("DispenserA decrement");
  //Serial.println("Message sent");
  //int retval = udp.endPacket();
	//Serial.println("endPacket");
  //Serial.println(retval);
  delay(5000);
}