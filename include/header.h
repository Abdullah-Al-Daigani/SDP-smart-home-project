
// Blynk ID
#define BLYNK_TEMPLATE_ID "TMPL6XMgI0VSM"
#define BLYNK_TEMPLATE_NAME "SDP main"
#define BLYNK_AUTH_TOKEN "gPoPXkBMeM_3oGP4xFw7K0lLi8Rnkz3y"

// libraries:

#include <Arduino.h> // include the arduino platform
#include <string>    // include the String library

// include necessary libraries for the modules to work
#include <SPI.h>              // the protocol used to communicate to the OLED display and the power monitor
#include <Wire.h>             // required for the Adafruit_SSD1306 and
#include <Adafruit_GFX.h>     // also required for the Adafruit_SSD1306 and Adafruit_INA219 libraries
#include <Adafruit_SSD1306.h> // the library used to control the OLED display
#include <Adafruit_INA219.h>  // the library used to get readings from the power monitor

#include <ArduinoJson.h>   // used to conveniently make and modify JSON files
#include <ArduinoJson.hpp> //

#include <BlynkSimpleEsp32.h> // used to communicate with Blynk server

#include <WiFi.h>         // the library used to turn on and use WiFi
#include <PubSubClient.h> // the library managing the MQTT protocol to communicate with the Host over WiFi

// defining constants and declarations:

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin for the OLED display

int ledPin = 13;                         // the pin # the LED is connected to
int potPin = 34;                         // the pin # the potentiometer is connected to
const char *content = "no messages yet"; // declares and initializes a pointer to store the latest message recieved from Telegram
String message = "No messages yet";      // a variable used to store the incoming telegram message
unsigned long lastMsg = millis();        // a variable used to set the frequency of the data reports
double ina_power;
double ina_current;
double ina_voltage;

void reconnect();                                                        // declaration of the function used to reconnect to the MQTT broker
void displayLoop(float ina_power, float ina_current, float ina_voltage); // declaration of the display loop

// other wifi cred
const char *ssid = "W";            // WiFi ssid
const char *password = "12121212"; // WiFi password
// const char *mqtt_server = "broker.mqttdashboard.com";

// const char *ssid = "Nutmeg-2.4GHz";       // WiFi ssid
// const char *password = "Qw121212";        // WiFi password

// objects generation:

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // the display object used to control the OLED display
Adafruit_INA219 ina219;                                                   // the object used to communicate with the power monitor module
WiFiClient espClient;                                                     // initialization of the onboard WiFi client
PubSubClient client(espClient);                                           // the initialization of the PubSubClient used for the MQTT protocol
BlynkTimer timer;                                                         // an object used for timing the messages

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif