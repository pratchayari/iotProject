#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(115200);
  Serial.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("QR Scanner Ready...");
}

void loop() {
  if (Serial2.available()){
    String qrData = Serial2.readStringUntil('\n');
    Serial.print("QR Code: ");
    Serial.println(qrData);
  }
}