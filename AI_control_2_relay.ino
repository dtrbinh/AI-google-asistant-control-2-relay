
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "Hoang Huy"
#define WIFI_PASS "0905547819"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "dotranbinh"
#define MQTT_PASS "aio_GvxM8025ZmnaGGlaKmUdLG7ChhLL"

//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe num1 = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/đèn 1");
Adafruit_MQTT_Subscribe num2 = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/đèn 2");

void setup()
{
  Serial.begin(115200);
  pinMode(D1, OUTPUT);pinMode(D2, OUTPUT);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  //Connect to WiFi
  Serial.print("\n\nKết nối Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  Serial.println("OK!");

  //Subscribe to the onoff feed
  mqtt.subscribe(&num1);  mqtt.subscribe(&num2);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  MQTT_connect();
  
  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &num1)
    {
      //Print the new value to the serial monitor
      Serial.print("Trạng thái 1: ");
      Serial.println((char*) num1.lastread);
      
      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) num1.lastread, "ON"))
      {
        //Active low logic
        digitalWrite(D1, LOW);
      }
      else
      {
        digitalWrite(D1, HIGH);
      }
    }
    if (subscription == &num2)
    {
      //Print the new value to the serial monitor
      Serial.print("Trạng thái 2: ");
      Serial.println((char*) num2.lastread);
      
      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) num2.lastread, "ON"))
      {
        //Active low logic
        digitalWrite(D2, LOW);
      }
      else
      {
        digitalWrite(D2, HIGH);
      }
    }
  }

  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}

void MQTT_connect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Kết nối lại trong 5s...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");digitalWrite(LED_BUILTIN, LOW);
}
