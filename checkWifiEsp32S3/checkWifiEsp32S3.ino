#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(3000);

  Serial.println("Start AP Test");

  WiFi.mode(WIFI_AP);
  bool ok = WiFi.softAP("ESP32-Setup", "12345678");

  Serial.print("AP result: ");
  Serial.println(ok ? "OK" : "FAIL");

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
}