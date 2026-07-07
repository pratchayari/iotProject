#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <PubSubClient.h>

// =========================
// Web Server / Preferences
// =========================
WebServer server(80);
Preferences prefs;

String saved_ssid;
String saved_pass;

bool apMode = false;

// =========================
// AP Setup
// =========================
const char* ap_ssid = "ESP32-Setup";
const char* ap_pass = "12345678";

// =========================
// MQTT
// =========================
const char* mqtt_server = "192.168.107.237";
const int mqtt_port = 1883;

const char* mqtt_user = "esp32_user01";
const char* mqtt_pass = "12345678";

const char* mqtt_cmd_topic = "esp32/led";
const char* mqtt_status_topic = "esp32/led/status";

// =========================
// GPIO
// =========================
#define LED_PIN 5
#define CONFIG_BUTTON 4

WiFiClient espClient;
PubSubClient client(espClient);

// =========================
// Web Page
// =========================
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

// =========================
// AP Mode
// =========================
void startAPMode() {
  apMode = true;

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

// =========================
// Connect Saved WiFi
// =========================
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

// =========================
// Factory Reset Button
// =========================
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

// =========================
// MQTT Callback
// =========================
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";

  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.println("-------------------------");
  Serial.print("Topic   : ");
  Serial.println(topic);

  Serial.print("Message : ");
  Serial.println(msg);

  if (msg == "ON") {
    digitalWrite(LED_PIN, HIGH);
    client.publish(mqtt_status_topic, "ON");
    Serial.println("LED ON");
  }
  else if (msg == "OFF") {
    digitalWrite(LED_PIN, LOW);
    client.publish(mqtt_status_topic, "OFF");
    Serial.println("LED OFF");
  }
}

// =========================
// MQTT Reconnect
// =========================
void reconnectMQTT() {
  while (!client.connected() && WiFi.status() == WL_CONNECTED) {
    Serial.print("Connecting MQTT...");

    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println(" Connected");

      client.subscribe(mqtt_cmd_topic);

      Serial.print("Subscribed : ");
      Serial.println(mqtt_cmd_topic);

      client.publish(mqtt_status_topic, "ONLINE");
    } else {
      Serial.print(" Failed, rc=");
      Serial.println(client.state());

      delay(5000);
    }
  }
}

// =========================
// Setup
// =========================
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(CONFIG_BUTTON, INPUT_PULLUP);

  if (configButtonPressed5Sec()) {
    clearWiFiConfig();
    startAPMode();
    return;
  }

  bool wifiOK = connectSavedWiFi();

  if (!wifiOK) {
    startAPMode();
    return;
  }

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// =========================
// Loop
// =========================
void loop() {
  server.handleClient();

  if (apMode) {
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      reconnectMQTT();
    }

    client.loop();
  }
}