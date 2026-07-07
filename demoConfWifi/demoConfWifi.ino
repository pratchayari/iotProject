#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

WebServer server(80);
Preferences prefs;

String saved_ssid;
String saved_pass;

const char* ap_ssid = "ESP32-Setup";
const char* ap_pass = "12345678";

#define CONFIG_BUTTON 4

void handleRoot() {
  String html = "";
  html += "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>ESP32 WiFi Setup</title>";
  html += "</head><body>";
  html += "<h2>ESP32 WiFi Setup</h2>";
  html += "<form action='/save' method='POST'>";
  html += "WiFi SSID:<br>";
  html += "<input type='text' name='ssid'><br><br>";
  html += "WiFi Password:<br>";
  html += "<input type='password' name='pass'><br><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form><br>";
  html += "<a href='/reset'>Reset WiFi</a>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleSave() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  if (ssid.length() > 0) {
    prefs.begin("wifi", false);
    prefs.putString("ssid", ssid);
    prefs.putString("pass", pass);
    prefs.end();

    server.send(200, "text/html",
                "<h2>Saved WiFi</h2><p>ESP32 will restart...</p>");

    delay(2000);
    ESP.restart();
  } else {
    server.send(200, "text/html",
                "<h2>Error</h2><p>SSID is empty</p><a href='/'>Back</a>");
  }
}

void handleReset() {
  prefs.begin("wifi", false);
  prefs.clear();
  prefs.end();

  server.send(200, "text/html",
              "<h2>WiFi Cleared</h2><p>ESP32 will restart...</p>");

  delay(2000);
  ESP.restart();
}

void startAPMode() {
  Serial.println("Start AP Mode");

  WiFi.disconnect(true);
  delay(500);

  WiFi.mode(WIFI_AP);
  delay(200);

  WiFi.softAP(ap_ssid, ap_pass);

  Serial.print("AP SSID: ");
  Serial.println(ap_ssid);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/reset", handleReset);

  server.begin();

  Serial.println("Web Server Started");
}

bool connectSavedWiFi() {
  prefs.begin("wifi", true);
  saved_ssid = prefs.getString("ssid", "");
  saved_pass = prefs.getString("pass", "");
  prefs.end();

  if (saved_ssid == "") {
    Serial.println("No saved WiFi");
    return false;
  }

  Serial.print("Connecting to WiFi: ");
  Serial.println(saved_ssid);

  WiFi.mode(WIFI_STA);
  delay(200);

  WiFi.begin(saved_ssid.c_str(), saved_pass.c_str());

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println("WiFi Failed");
  return false;
}

bool configButtonPressed5Sec() {

  Serial.println("Hold CONFIG button...");

  if (digitalRead(CONFIG_BUTTON) == HIGH) {
    return false;
  }

  unsigned long pressTime = millis();

  while (digitalRead(CONFIG_BUTTON) == LOW) {

    if (millis() - pressTime >= 5000) {
      Serial.println("Factory Reset");
      return true;
    }

    delay(10);
  }

  Serial.println("Button released");

  return false;
}

void clearWiFiConfig() {
  prefs.begin("wifi", false);
  prefs.clear();
  prefs.end();

  Serial.println("WiFi Config Cleared");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(CONFIG_BUTTON, INPUT_PULLUP);

  if (configButtonPressed5Sec()) {
    clearWiFiConfig();
    startAPMode();
    return;
  }

  bool wifiOK = connectSavedWiFi();

  if (!wifiOK) {
    startAPMode();
  }
}

void loop() {
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    // ใส่ MQTT หรือโค้ดควบคุมอุปกรณ์ตรงนี้
  }
}