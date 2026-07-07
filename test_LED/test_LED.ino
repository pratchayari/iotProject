#define LED_D 8
void setup() {
  pinMode(LED_D, OUTPUT);

}

void loop() {
  digitalWrite(LED_D, HIGH);   // สั่งให้ LED เปิดสว่าง (ON)
  delay(2000);                  // หน่วงเวลา 2 วินาที (2000 มิลลิวินาที)
  
  digitalWrite(LED_D, LOW);    // สั่งให้ LED ดับ (OFF)
  delay(2000); 

}
