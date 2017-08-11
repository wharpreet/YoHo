#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "<SSID>";
const char* password = "<PASSWORD>";

const char* mqtt_server = "<Server IP>";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledGPIO5 = 5;
const int ledGPIO4 = 4;

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
      Serial.print("Changing GPIO 4 to ");
      if(messageTemp == "0"){
        digitalWrite(ledGPIO4, HIGH);
        Serial.print("On");
        client.publish("esp8266/4/reply", "4-on");
      }
      else if(messageTemp == "1"){
        digitalWrite(ledGPIO4, LOW);
        Serial.print("Off");
        client.publish("esp8266/4/reply", "4-off");
      }
  }
  if(topic=="esp8266/5"){
      Serial.print("Changing GPIO 5 to ");
      if(messageTemp == "0"){
        digitalWrite(ledGPIO5, HIGH);
        Serial.print("On");
        client.publish("esp8266/5/reply", "5-on");

      }
      else if(messageTemp == "1"){
        digitalWrite(ledGPIO5, LOW);
        Serial.print("Off");
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

void setup() {
  pinMode(ledGPIO4, OUTPUT);
  pinMode(ledGPIO5, OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

const int sensorIn = A0;
int mVperAmp = 66; // use 185 for 5A Module, 100 for 20A Module and 66 for 30A Module
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");

 Voltage = getVPP();
 VRMS = (Voltage/2.0) *0.707; 
 AmpsRMS = ((VRMS * 1000)/mVperAmp);
 String stringOne =  String(AmpsRMS, DEC);          // using an int and a base
 client.publish("ampServer/1", stringOne.c_str());
 Serial.print(AmpsRMS);
 Serial.println(" Amps RMS");
}

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
