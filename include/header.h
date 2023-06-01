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

void reconnect();                  // declaration of the function used to reconnect to the MQTT broker
void displayLoop(float potin_per); // declaration of the display loop

// other wifi cred
const char *ssid = "W";                     // WiFi ssid
const char *password = "12121212";          // WiFi password
const char *mqtt_server = "192.168.72.179"; // MQTT broker ip address

// const char *ssid = "Nutmeg-2.4GHz";       // WiFi ssid
// const char *password = "Qw121212";        // WiFi password
// const char *mqtt_server = "192.168.1.12"; // MQTT broker ip address

// objects generation:

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // the display object used to control the OLED display
Adafruit_INA219 ina219;                                                   // the object used to communicate with the power monitor module
WiFiClient espClient;                                                     // initialization of the onboard WiFi client
PubSubClient client(espClient);                                           // the initialization of the PubSubClient used for the MQTT protocol

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

Adafruit_NeoPixel pixels(8, 26, NEO_GRB + NEO_KHZ800);
unsigned long lastMsg2 = millis(); // a variable used to set the frequency of the data reports
int i = 0;