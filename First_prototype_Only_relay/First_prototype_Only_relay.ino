#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "<SSID>";
const char* password = "<PASSWORD>";

const char* mqtt_server = "<Server IP>";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledGPIO5 = 5;
const int ledGPIO4 = 4;
const int ledGPIO12 = 12;
const int ledGPIO14 = 14;

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
        client.publish("esp8266/4/reply", "4-off");
      }
      else if(messageTemp == "1"){
        digitalWrite(ledGPIO4, LOW);
        client.publish("esp8266/4/reply", "4-on");
      }
  }
  if(topic=="esp8266/5"){
      if(messageTemp == "0"){
        digitalWrite(ledGPIO5, HIGH);
        client.publish("esp8266/5/reply", "5-off");
      }
      else if(messageTemp == "1"){
        digitalWrite(ledGPIO5, LOW);
        client.publish("esp8266/5/reply", "5-on");
      }
  }
  if(topic=="esp8266/12"){
      if(messageTemp == "0"){
        digitalWrite(ledGPIO12, HIGH);
        client.publish("esp8266/12/reply", "12-off");
      }
      else if(messageTemp == "1"){
        digitalWrite(ledGPIO12, LOW);
        client.publish("esp8266/12/reply", "12-on");
      }
  }
  if(topic=="esp8266/14"){
      if(messageTemp == "0"){
        digitalWrite(ledGPIO14, HIGH);
        client.publish("esp8266/14/reply", "14-off");
      }
      else if(messageTemp == "1"){
        digitalWrite(ledGPIO14, LOW);
        client.publish("esp8266/14/reply", "14-on");
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
      client.subscribe("esp8266/12");
      client.subscribe("esp8266/14");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledGPIO4, OUTPUT);
  pinMode(ledGPIO5, OUTPUT);
  pinMode(ledGPIO12, OUTPUT);
  pinMode(ledGPIO14, OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");
}
