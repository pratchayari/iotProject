#define PIN_D4 D2 //GPIO2 = 2
#define PIN_D1 D1

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_D4, OUTPUT);
  pinMode(PIN_D1, OUTPUT);
  pinMode(2, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PIN_D4, LOW);
  digitalWrite(PIN_D1, LOW);
  digitalWrite(2, HIGH);
  delay(1000);

  digitalWrite(PIN_D1, HIGH);
  digitalWrite(PIN_D4, HIGH);
  digitalWrite(2, LOW);
  delay(1000);
}
