// Define the GPIO pin for the relay control
#define RELAY_PIN 12  // Change this to your chosen GPIO pin

void setup() {
  // Initialize the relay control pin as output
  pinMode(RELAY_PIN, OUTPUT);
  
  // Start the serial communication to monitor the status
  Serial.begin(115200);
}

void loop() {
  // Turn relay on (close the switch)
  digitalWrite(RELAY_PIN, HIGH);  
  Serial.println("Relay ON");
  
  // Wait for 2 seconds
  delay(2000);

  // Turn relay off (open the switch)
  digitalWrite(RELAY_PIN, LOW);  
  Serial.println("Relay OFF");

  // Wait for 2 seconds
  delay(2000);
}
