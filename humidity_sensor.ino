// Author: Heather Cox
// Date: 11/09/2021
// Purpose: Humidity and Temperature Sensor
//          Monitor temperature and humidity of entomological specimen environment
//          and send data and alerts to external device via WiFi

// Import DHT11 Sensor library and construct new sensor object
#include <SimpleDHT.h>
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

void setup() {
  Serial.begin(9600);
}

void loop() {
  // initialise variables
  byte temperature = 0;
  byte humidity = 0;
  
  if (dht11.read(&temperature, &humidity, NULL)) {
    Serial.print("Read DHT11 failed");
    return;
  }

  //Output to Serial Terminal
  Serial.print("Temperature: ");Serial.print((int)temperature); Serial.println(" *C"); 
  Serial.print("Humidity: ");Serial.print((int)humidity); Serial.println(" %");
  
  // DHT11 sampling rate is 1HZ.
  delay(1500);


}
