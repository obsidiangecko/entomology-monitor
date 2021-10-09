// Author: Heather Cox
// Version: 0.3
// Date: 03/10/2021
// Purpose: Humidity and Temperature Sensor
//          Monitor temperature and humidity of entomological specimen environment
//          and send data and alerts to external device via WiFi

// Import DHT11 Sensor library and construct new sensor object
#include <DHT.h>
#include <ESP8266WiFi.h>
#include "ThingsBoard.h" //Version 0.2.0 - not compatible with 0.4.0 as at 3/10/21

#define WIFI_AP "WIRELESS_AP"
#define WIFI_PASSWORD "PASSWORD"

#define TOKEN "oPL81uLk9wv9bFQwHigC"
#define THINGSBOARD_SERVER "demo.thingsboard.io"


//DHT
#define DHTPIN 2 //int pinDHT11 = 2; equivalent SimpleDHT library
#define DHTTYPE DHT11 

WiFiClient wifiClient;

PubSubClient client(wifiClient);

//Initialise sensor
DHT dht(DHTPIN, DHTTYPE); //SimpleDHT11 dht11(pinDHT11); equivalent SimpleDHT library

ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSent;

void setup() {
  Serial.begin(115200); //higher rate seems to be necessary for wireless or dashboards
  dht.begin();
  delay(10); //delays are allowed with ThingsBoard, but not with Blynk
  InitWiFi();
  lastSent = 0;
}

void loop() {
  if (!tb.connected()) {
    reconnect();
  }
  if (millis() - lastSent > 1000) { //update and send on
    getAndSend();
    lastSent = millis();
  }

  tb.loop();
  delay(500); //allow update to Thingsboard to complete prior to sleep
  // ESP8266 Deep Sleep Mode enabled - 100 seconds = 
  ESP.deepSleep(10e7);
}

void getAndSend(){
  Serial.println("Collecting temperature data.");

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check for failure
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Read DHT11 failed");
    return;
  }
  
  //Output
  Serial.println("Sending to Thingsboard:");
  Serial.print("Temperature: ");Serial.print((int)temperature); Serial.println(" *C"); 
  Serial.print("Humidity: ");Serial.print((int)humidity); Serial.println(" %");

  tb.sendTelemetryFloat("temperature", temperature);
  tb.sendTelemetryFloat("humidity", humidity);
}

//suggested code from Thingsboard  
void InitWiFi(){
  Serial.println("Connecting to Wireless ..."); 

  //attempt connection to WiFi network
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wireless Network");
}

//suggested code from Thingsboard
void reconnect() {
   while (!tb.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to Wireless Network");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    if ( tb.connect(THINGSBOARD_SERVER, TOKEN) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
