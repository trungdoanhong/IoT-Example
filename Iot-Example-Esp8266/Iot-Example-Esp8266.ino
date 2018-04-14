#include <ESP8266WiFi.h>
#include "PubSubClient.h"

#define MQTT_SERVER "m12.cloudmqtt.com"
#define MQTT_TOPIC_PUB "SensorValue"
#define MQTT_TOPIC_SUB "ControlValue"
#define MQTT_USER "wind2115"
#define MQTT_PASSWORD "841566"

const uint16_t MQTT_PORT = 17387;

WiFiClient EspClient;
PubSubClient Client(EspClient);

String InputString = "";
boolean StringComplete = false;

char Ssid[] = "k82 nguyenluongbang";
char Password[] = "0917691235";

void setup()
{
  Serial.begin(9600);
  ConnectWifi();

  Client.setServer(MQTT_SERVER, MQTT_PORT);
  Client.setCallback(Callback);
}

void Callback(char* topic, byte* payload, unsigned int len)
{
  for (int i = 0; i < len; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void Reconnect()
{
  while (!Client.connected())
  {
    if (Client.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD))
    {
      Client.publish(MQTT_TOPIC_PUB, "ESP_reconnected");
      Client.subscribe(MQTT_TOPIC_SUB);
    }
    else
    {
      delay(5000);
    }
  }
}

void loop()
{
  SerialEvent();
  if (StringComplete)
  {
    char charBuf[50];
    InputString.toCharArray(charBuf, 50);
    Client.publish(MQTT_TOPIC_PUB, charBuf);

    InputString = "";
    StringComplete = false;
  }

  if (!Client.connected())
  {
    Reconnect();
  }
  Client.loop();
}

void SerialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    InputString += inChar;

    if (inChar == '\n')
    {
      StringComplete = true;
    }
  }
}

void ConnectWifi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(Ssid);

  WiFi.begin(Ssid, Password);
}
