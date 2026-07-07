#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "IT_iotLab";
const char* password = "iot12345678";

const char* mqtt_server = "192.168.107.237";
const int mqtt_port = 1883;

const char* mqtt_user = "esp32_user01";
const char* mqtt_pass = "12345678";

WiFiClient espClient;
PubSubClient client(espClient);

#define LED_PIN 22

void callback(char* topic, byte* payload, unsigned int length) {

  String msg = "";

  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  Serial.println(msg);

  if (msg == "ON") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
  }
  else if (msg == "OFF") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
  }
}

void reconnect() {

  while (!client.connected()) {

    Serial.print("Connecting MQTT...");

    String clientId = "ESP32S3-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(),
                       mqtt_user,
                       mqtt_pass)) {

      Serial.println(" Connected");

      client.subscribe("esp32/led");

      Serial.println("Subscribed: esp32/led");

    } else {

      Serial.print(" Failed rc=");
      Serial.println(client.state());

      delay(5000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}