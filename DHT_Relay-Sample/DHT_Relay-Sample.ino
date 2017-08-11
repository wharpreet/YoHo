// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11

// Replace with your network details
const char* ssid = "<SSID>";
const char* password = "<PASSWORD>";

const char* mqtt_server = "<Server IP>";

// DHT Sensor
const int DHTPin = D5;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

const int ledGPIO5 = 5;
const int ledGPIO4 = 4;

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if(topic=="esp8266/4"){
      if(messageTemp == "0"){
        digitalWrite(ledGPIO4, HIGH);
        client.publish("esp8266/4/reply", "4-on");
      }
      else if(messageTemp == "1"){
        digitalWrite(ledGPIO4, LOW);
        client.publish("esp8266/4/reply", "4-off");
      }
  }
  if(topic=="esp8266/5"){
      if(messageTemp == "0"){
        digitalWrite(ledGPIO5, HIGH);
        client.publish("esp8266/5/reply", "5-on");

      }
      else if(messageTemp == "1"){
        digitalWrite(ledGPIO5, LOW);
         client.publish("esp8266/5/reply", "5-off");
      }
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      client.subscribe("esp8266/4");
      client.subscribe("esp8266/5");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// only runs once on boot
void setup() {
  pinMode(ledGPIO4, OUTPUT);
  pinMode(ledGPIO5, OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  dht.begin();
}

// runs over and over again
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");
    delay(10000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float hic = dht.computeHeatIndex(t, h, false);
  dtostrf(hic, 6, 2, celsiusTemp);
  float hif = dht.computeHeatIndex(f, h);
  dtostrf(hif, 6, 2, fahrenheitTemp);
  dtostrf(h, 6, 2, humidityTemp);
  client.publish("DHT11/1",celsiusTemp);
  client.publish("DHT11/1",fahrenheitTemp);
  client.publish("DHT11/1",humidityTemp);
}
