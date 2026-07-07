#include <WiFi.h>
#include "PubSubClient.h"


// 1. ตั้งค่า Wi-Fi และ MQTT Broker
const char* ssid       = "IT_iotLab"; 
const char* password   = "iot12345678";
const char* mqtt_server = "192.168.107.236"; // เปลี่ยนเป็น IP คอมพิวเตอร์ที่รัน Mosquitto

const int ledPin = 2; // ESP32 ส่วนใหญ่มี LED บนบอร์ดที่ขา 2

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 
}

// ฟังก์ชันรับส่งข้อมูล (เมื่อมีข้อความมาจาก Node-RED)
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // เช็คเงื่อนไขเปิด-ปิดไฟ
  if (String(topic) == "esp32/led") {
    if (message == "ON") {
      digitalWrite(ledPin, HIGH); // ไฟติด
    } else if (message == "OFF") {
      digitalWrite(ledPin, LOW);  // ไฟดับ
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // สุ่มชื่อ Client ID
    if (client.connect("ESP32_Classroom_Client")) {
      Serial.println("connected");
      client.subscribe("esp32/led"); // สมัครรับข้อมูลช่องนี้
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}