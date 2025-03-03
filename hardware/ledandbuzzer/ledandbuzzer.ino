// ESP32 LEDs and Buzzer Test

#define GREEN_LED_PIN 2  // Green LED
#define RED_LED_PIN 4    // Red LED
#define BUZZER_PIN 15    // Buzzer

void setup() {
  Serial.begin(115200);
  
  // Initialize LED pins as outputs
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println("ESP32 LEDs and Buzzer Test");
}

void loop() {
  // Test Green LED
  Serial.println("Testing Green LED");
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  delay(1000);
  
  // Test Red LED
  Serial.println("Testing Red LED");
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  delay(1000);
  
  // Test Buzzer
  Serial.println("Testing Buzzer");
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  delay(500);
  
  // Test all together
  Serial.println("Testing all together");
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  delay(500);
}