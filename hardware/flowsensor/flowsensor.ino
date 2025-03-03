// ESP32 Water Flow Meter Test
// For YF-S201 or similar hall effect flow sensor

#define FLOW_SENSOR_PIN 34 // GPIO pin connected to the flow sensor

// Variables
volatile long pulseCount = 0;
float calibrationFactor = 7.5; // This varies by sensor model (pulses per liter)
float flowRate;
unsigned long oldTime;

void IRAM_ATTR pulseCounter() {
  // Increment the pulse counter
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  
  // Initialize the flow sensor pin as input
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  
  // Attach an interrupt to the ISR
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);
  
  // Initialize timing variables
  oldTime = millis();
  
  Serial.println("ESP32 Water Flow Meter Test");
  Serial.println("Flow rate will be displayed in liters per minute (L/min)");
}

void loop() {
  // Every second, calculate and display flow rate
  if ((millis() - oldTime) > 1000) {
    // Disable the interrupt while calculating flow rate
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    
    // Calculate flow rate (L/min)
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Reset timing variables
    oldTime = millis();
    
    // Print the flow rate for this second
    Serial.print("Flow rate: ");
    Serial.print(flowRate, 2);  // Print with 2 decimal places
    Serial.println(" L/min");
    
    // Reset the pulse counter
    pulseCount = 0;
    
    // Enable the interrupt again
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);
  }
}