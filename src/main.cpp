#include <header.h>

BLYNK_WRITE(V3)
{
  int slider = param.asInt();
  Serial.println(slider);
}

BLYNK_CONNECTED()
{
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
}

void BlynkMsgUp()
{
  Blynk.virtualWrite(V0, (double)ina_voltage);
  Blynk.virtualWrite(V1, (double)ina_current);
  Blynk.virtualWrite(V2, (double)ina_power);
}

BLYNK_WRITE(2)
{
  int value = param.asInt();
  digitalWrite(2, value);
}

void callback(char *topic, byte *payload, unsigned int length) // the call back function which gets called with incoming MQTT messages
{
  // prints the incoming message topic to serial for debugging
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  StaticJsonDocument<256> payload_JSON;           // make a new JSON document to put the data into
  deserializeJson(payload_JSON, payload, length); // convert the character array  to a JSON document
  content = payload_JSON["content"];              // stores the content parameter from the JSON document to the address *content
  Serial.println(content);                        // prints the content to serial for debugging
  message = content;                              // store the content of the telegram message in the variable

  // if statement to check the content value and respond to it
  if (strcmp(content, "Toggle") == 0)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  else if (strcmp(content, "/off") == 0)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (strcmp(content, "/on") == 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (strcmp(content, "Blink") == 0)
  {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(100);
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(100);
    }
  }
}

void setup() // the setup function runs only once after startup
{

  delay(100);         // a delay to make sure the generation is complete before doing anything else
  Serial.begin(9600); // start the serial interface to report back through the USB for debugging
  while (!Serial)     // wait until serial is active before continuing
    ;
  while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) // wait until the display object is active before continuing
    ;                                                //
  while (!ina219.begin())                            // wait until the power monitor object is active before continuing
    ;                                                //

  pinMode(ledPin, OUTPUT); // set LED pin to output pin
  pinMode(potPin, INPUT);  // set potentiometer pin to input pin
  pinMode(2, OUTPUT);      // set embedded LED pin to output pin

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  while (!Blynk.connected())
    ;

  Serial.print("Connecting to "); // send to serial that WiFi connection is being attempted
  Serial.println(ssid);           // show ssid of the network you're trying to connect to
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);           // initiate the WiFi connection
  // while (WiFi.status() != WL_CONNECTED) // wait until WiFi is connected
  //   ;

  Serial.println("WiFi connected"); // prints that the WiFi is connected to serial for debugging
  Serial.println("IP address: ");   //
  Serial.println(WiFi.localIP());   // prints the current IP address to serial for debugging
                                    //
  // IPAddress mqtt_server(192, 168, 133, 179);
  IPAddress mqtt_server(192, 168, WiFi.localIP()[2], 179);

  client.setServer(mqtt_server, 1883); // set the IP address and port of the MQTT broker
  client.setCallback(callback);        // set the callback function

  display.display(); // start the OLED display
  delay(500);        // pause to make sure the display is ready

  display.clearDisplay();      // clear the display
  display.setTextSize(2);      // set the font size to x2
  display.setTextColor(WHITE); // set font color to white
  display.setCursor(10, 0);    // position curser in x = 10 and y = 0
  display.print("working!");   // print working in the display
  display.display();           // send the previous commands to the display
  delay(1000);                 // wait to make sure the display is displaying correctly

  timer.setInterval(100L, BlynkMsgUp);
}

void loop() // the loop function runs continuously
{

  if (!client.connected()) // make sure the MQTT client is active
  {                        //
    reconnect();           // reconnect to the MQTT client
  }

  float potin = analogRead(potPin);         // get a reading from the potentiometer
  float potin_per = potin * 100 / 4095;     // calculate the reading as a percentage
  float brightness = potin_per * 255 / 100; // calculate the brightness equivalent

  ina_power = ina219.getPower_mW();
  ina_current = ina219.getCurrent_mA();
  ina_voltage = ina219.getBusVoltage_V();
  analogWrite(ledPin, brightness);                  // output the brightness value as PWM to the LED
  displayLoop(ina_power, ina_current, ina_voltage); // display the percentage on the OLED display

  unsigned long now = millis();         // the current time in ms
  if (now - lastMsg > 200)              // compare the current time with the time of the last message
  {                                     //
    lastMsg = now;                      // set the last message time to now
    StaticJsonDocument<256> PubJSON;    // make a new JSON document to put the data into
    char buffer[256];                   // make a buffer variable to store the data to temporarily
    PubJSON["potin_per"] = potin_per;   // add the data to the JSON document
    PubJSON["brightness"] = brightness; // add the data to the JSON document
    PubJSON["ina_power"] = ina_power;
    PubJSON["ina_current"] = ina_current;
    PubJSON["ina_voltage"] = ina_voltage;
    size_t n = serializeJson(PubJSON, buffer); // convert the JSON document to a character array to send over the MQTT protocol
    // Serial.println(buffer);                     // prints the output to serial for debugging
    client.publish("/home/sensors", buffer, n); // sends the data to the host through the MQTT protocol
    // Serial.println(ina_power);
  }
  client.loop(); // required for the MQTT client to function correctly
  Blynk.run();
  timer.run();
}

void reconnect() // reconnects to the MQTT client
{

  while (!client.connected()) // check whether the client is connected
  {
    Serial.print("Attempting MQTT connection..."); // prints to serial for debugging
    // WiFi.mode(WIFI_STA);
    if (client.connect("ESP32-1"))               // Attempt to connect
    {                                            //
      Serial.println("connected");               // prints to serial for debugging
      client.publish("outTopic", "hello world"); // once connected, publish an announcement
      client.subscribe("/test/ESP1");            // resubscribe
    }
    else
    {
      display.setTextSize(1);                   //
      display.clearDisplay();                   //
      display.setCursor(0, 0);                  // display the error and the server ip for debugging
      display.println("failed to connect to:"); //
      display.setTextSize(2);                   //

      Serial.print("failed, rc=");               // prints to serial for debugging
      Serial.print(client.state());              // prints the error to serial for debugging
      Serial.println(" try again in 5 seconds"); // prints to serial for debugging
      delay(5000);                               // Wait 5 seconds before retrying
    }
  }
}

void displayLoop(float ina_power, float ina_current, float ina_voltage) // prints the current PWM percentage and the last message to the OLED
{
  display.clearDisplay();     //
  display.setCursor(30, 0);   //
  display.println("Power:");  //
  display.setCursor(30, 16);  //
  display.print(ina_power);   //
  display.print("mW");        //
  display.setCursor(30, 32);  //
  display.print(ina_current); //
  display.print("mA");        //
  display.setCursor(30, 48);  //
  display.print(ina_voltage); //
  display.print("V");         //
  display.setTextSize(2);     //
  display.display();          //
}
