#define LED_4 D4
#define LED_2 D2

void setup()
{          
  pinMode(LED_2, OUTPUT);
  pinMode(LED_4, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  digitalWrite(LED_2, LOW); 
  digitalWrite(LED_4, LOW); 
  Serial.println("low"); 
  delay(1000);

  digitalWrite(LED_2, HIGH);
  digitalWrite(LED_4, HIGH); 
  Serial.println("high");
  delay(1000);  
}