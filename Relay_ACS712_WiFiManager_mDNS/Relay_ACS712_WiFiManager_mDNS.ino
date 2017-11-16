#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

const char* mqtt_server_host = "<mqtt host>";
const char* mqttuser = "<USERNAME>";
const char* mqttpass = "<PASSWORD>";
byte mac[6];

WiFiClient espClient;
PubSubClient client(espClient);
const int ledGPIO5 = 5;
const int ledGPIO4 = 4;

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.print("Created config portal AP ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
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
  float rawAmps = getVPP();
   String stringOne =  String(rawAmps, DEC);          // using an int and a base
//  Serial.println(stringOne);
  client.publish("esp8266/4/reply", "stringOne");

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
  Serial.println();
}

void reconnect() {
  
  Serial.println(mqtt_server_host);
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqttuser, mqttpass)) {
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

void setup() {
  
  Serial.begin(115200);  
  WiFiManager wifiManager;

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  if(!wifiManager.autoConnect("YoHo")) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  } 
  if (!MDNS.begin("yoho")) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
//  Serial.println("mDNS responder started");
//  Serial.println("TCP server started");
  MDNS.addService("http", "tcp", 80);
//  Serial.println("connected...yeey :)");
  WiFi.macAddress(mac);
  char macAddr[20] = {0};
  
  snprintf(macAddr,20,"%02x%02x%02x%02x%02x%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  Serial.println(macAddr);
  pinMode(ledGPIO4, OUTPUT);
  pinMode(ledGPIO5, OUTPUT);
  client.setServer(mqtt_server_host, 1883);
  client.setCallback(callback);

}

const int sensorIn = A0;
float getVPP()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;    
   return result;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
  {
    Serial.println("attempting mqtt");
    client.connect("ESP8266Client", mqttuser, mqttpass);
  }
}
