#include <WiFi.h>
#include <PubSubClient.h>

//=========================
// WiFi
//=========================
const char* ssid = "IT_iotLab";
const char* password = "iot12345678";

//=========================
// MQTT
//=========================
const char* mqtt_server = "192.168.107.237";
const int mqtt_port = 1883;

const char* mqtt_user = "esp32_user01";
const char* mqtt_pass = "12345678";

// Topic
const char* mqtt_cmd_topic = "esp32/led";
const char* mqtt_status_topic = "esp32/led/status";

//=========================
// GPIO
//=========================
#define LED_PIN 22

WiFiClient espClient;
PubSubClient client(espClient);

//=========================
// Callback
//=========================
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

//=========================
// Connect MQTT
//=========================
void reconnect() {

  while (!client.connected()) {

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

//=========================
// Setup
//=========================
void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println();
  Serial.println("Connecting WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("======================");
  Serial.println("WiFi Connected");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

//=========================
// Loop
//=========================
void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}