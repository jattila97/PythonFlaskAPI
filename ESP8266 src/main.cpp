#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
const char* ssid="***";
const char* password = "***";
const char* ssid2="***";
const char* password2 = "***";
#define DHTPIN 2
DHT dht(DHTPIN, DHT11);
#define stopPin 14


const char* mqtt_server1 = "192.168.0.58";
const char* mqtt_server2 = "192.168.1.184";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


const char* mqttUser = "***";
const char* mqttPassword = "***";




const long utcOffsetInSeconds = 7200;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);




void setup() 
{
  pinMode(stopPin,INPUT);

  pinMode(DHTPIN,INPUT);
  Serial.begin(9600);
  dht.begin();
  delay(10);
       
  Serial.println("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  
  while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
        if (millis()>12000)
        {
          break;
        }
    }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println();
    WiFi.begin(ssid2, password2);
    Serial.println("Connecting to ");
    Serial.println(ssid2);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
  }
  
  

  client.setServer(mqtt_server1, 1883);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      
      if (millis()>5000)
        {
          break;
        }

      delay(2000);
    }
  }

  client.setServer(mqtt_server2, 1883);

while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
   client.publish("workshop/esp8266/Temperature", "Initial message");


   timeClient.begin();
}


void loop() {

  timeClient.update();
  String day = daysOfTheWeek[timeClient.getDay()];
  Serial.print(day);
  Serial.print(", ");
  int hour = timeClient.getHours();
  Serial.print(hour);
  Serial.print(":");
  int minutes = timeClient.getMinutes();
  Serial.print(minutes);
  Serial.print(":");
  int seconds = timeClient.getSeconds();
  Serial.println(seconds);
  String formattedTime = timeClient.getFormattedTime();
  Serial.println(formattedTime);


  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
      
  if (isnan(hum) || isnan(temp)) 
     {
       Serial.println("Failed to read from DHT sensor!");
       delay(1000);
       return;
     }
  else 
      {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.println(" degrees Celcius,");
      Serial.print("Humidity: ");
      Serial.println(hum);
      }

 
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoderHumidity = JSONbuffer.createObject();
  JsonObject& JSONencoderTemperature = JSONbuffer.createObject();
 
  /* JSONencoder["device"] = "ESP32";
  JSONencoder["sensorType"] = "Temperature";
  JsonArray& values = JSONencoder.createNestedArray("values");
 
  values.add(hum); */

  JSONencoderHumidity["type"] = "Humidity";
  JSONencoderHumidity["value"] = hum;
  JSONencoderHumidity["time"] = formattedTime;
  JSONencoderTemperature["type"] = "Temperature";
  JSONencoderTemperature["value"] = temp;
  JSONencoderTemperature["time"] = formattedTime;
 
  char JSONmessageBufferHumidity[100];
  JSONencoderHumidity.printTo(JSONmessageBufferHumidity, sizeof(JSONmessageBufferHumidity));
  Serial.println("Sending humidity message to MQTT topic..");
  Serial.println(JSONmessageBufferHumidity);
 
  if (client.publish("workshop/esp8266/Humidity", JSONmessageBufferHumidity) == true) {
    Serial.println("Success sending humidity message");
  } else {
    Serial.println("Error sending humidity message");
  }

  char JSONmessageBufferTemperature[100];
  JSONencoderTemperature.printTo(JSONmessageBufferTemperature, sizeof(JSONmessageBufferTemperature));
  Serial.println("Sending temperature message to MQTT topic..");
  Serial.println(JSONmessageBufferTemperature);
 
  if (client.publish("workshop/esp8266/Temperature", JSONmessageBufferTemperature) == true) {
    Serial.println("Success sending temperature message");
  } else {
    Serial.println("Error sending temperature message");
  }

 
  client.loop();
  Serial.println("-------------");

  delay(5000);
}