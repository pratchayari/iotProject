#include <ChipNorm_DHT11.h>
 
 dht11 DHT11; // Initialisierung des DHT11
 #define DHT11PIN 5 //D5
 
 void setup() {
  Serial.begin(9600); // Serielle kommunikation starten
 }
 
 void loop() {
  DHT11.read(DHT11PIN); // Sensor-Daten auslesen
 
  Serial.print((float)DHT11.humidity, 2);
  Serial.print("%  ");
  Serial.print((float)DHT11.temperature, 2);
  Serial.println("°C");
 
  delay(2000); // 2 Sekunden Pause
 }
 
 
