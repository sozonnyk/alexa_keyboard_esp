#include "alexa_keyboard_esp.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include "fauxmoESP.h"
#include "credentials.h"

//Required libraries:
//ESP8266WiFi
//ESPAsyncTCP
//ESPAsyncWebServer
//FauxMoESP
//Hash
//SoftwareSerial

#define DEBUG
#ifdef DEBUG
	#define DEBUG_SPEED 115200
	#define D(input) {Serial.print(input); Serial.flush();}
	#define Dln(input) {Serial.println(input); Serial.flush();}
#else
	#define D(input)
	#define Dln(input)
#endif

#if !defined(WIFI_SSID) && !defined(WIFI_PASS)
	#error "You must define WIFI_SSID and WIFI_PASS in credentials.h""
#endif

#define DEVICE_NAME "TV"
#define ON_COMMAND "B1" //Esc
#define OFF_COMMAND "80+D4" //Ctrl+Del

#define SOFT_RX D5
#define SOFT_TX D6
#define SOFT_SPEED 57600

SoftwareSerial serial(SOFT_RX, SOFT_TX, false, 256);
fauxmoESP fauxmo;

void setup() {
#ifdef DEBUG
	Serial.begin(DEBUG_SPEED);
#endif
	serial.begin(SOFT_SPEED);
	serial.println("Begin");

	D("Connecting to ");
	Dln(WIFI_SSID);

	WiFi.begin(WIFI_SSID, WIFI_PASS);
	while (WiFi.status() != WL_CONNECTED) {
		delay(200);
		D(".");
	}

	Dln("WiFi connected");
	D("IP address: ");
	Dln(WiFi.localIP());

	fauxmo.addDevice(DEVICE_NAME);
    fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
        Dln(printf("[MAIN] Device #%d (%s) state: %s", device_id, device_name, state ? "ON" : "OFF"));
        serial.println(state ? ON_COMMAND : OFF_COMMAND);
    });
}

void loop() {
	fauxmo.handle();
}
