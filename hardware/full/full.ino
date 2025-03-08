#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "I have no life";
const char* password = "123456789";

// Endpoint URL
const char* serverUrl = "https://didier.requestcatcher.com/";

// Flow sensor
#define FLOW_SENSOR_PIN 34
volatile long pulseCount = 0;
float calibrationFactor = 7.5; // Pulses per liter
float flowRate;
unsigned long oldTime;

// Global variables for tracking total mL
float totalDispensedmL = 0.0; // Total milliliters dispensed
const float targetmL = 400.0; // Target volume in milliliters
bool pumpRunning = false;     // Track if the pump is running

// Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {17, 16, 14};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Relay
#define RELAY_PIN 12

// LEDs and Buzzer
#define GREEN_LED_PIN 2
#define RED_LED_PIN 4
#define BUZZER_PIN 15

// RFID
MFRC522DriverPinSimple ss_pin(5);
MFRC522DriverSPI driver{ss_pin};
MFRC522 mfrc522{driver};

// Function prototypes
void IRAM_ATTR pulseCounter();
void handleFlowSensor();
void handleKeypad();
void handleOLED();
void handleRelay();
void handleRFID();
void handleLEDsAndBuzzer();
void sendDataToServer(String data);

void setup() {
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize flow sensor
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);
  oldTime = millis();

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();

  // Initialize relay
  pinMode(RELAY_PIN, OUTPUT);

  // Initialize LEDs and buzzer
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize RFID
  mfrc522.PCD_Init();
}

void loop() {
  handleFlowSensor();
  handleKeypad();
  handleOLED();
  handleRelay();
  handleRFID();
  handleLEDsAndBuzzer();
}

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void handleFlowSensor() {
  if ((millis() - oldTime) > 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    pulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);

    Serial.print("Flow rate: ");
    Serial.print(flowRate, 2);
    Serial.println(" L/min");

    // Send flow rate data to server
    sendDataToServer("Flow rate: " + String(flowRate, 2) + " L/min");
  }
}

void handleKeypad() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    // Display key on OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Key: " + String(key));
    display.display();
  }
}

void handleOLED() {
  // Example of displaying static text
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("System Active");
  display.display();
}

void handleRelay() {
  // Calculate the volume dispensed since the last reading
  float volumeDispensed = (flowRate / 60.0) * 1000.0; // Convert L/min to mL/s
  totalDispensedmL += volumeDispensed;

  // Ensure totalDispensedmL does not go below 0
  if (totalDispensedmL < 0) {
    totalDispensedmL = 0;
  }

  // Control the relay based on the total dispensed volume
  if (totalDispensedmL >= targetmL) {
    digitalWrite(RELAY_PIN, LOW); // Turn off the relay
    pumpRunning = false;
    Serial.println("Pump OFF - Target volume reached");
  } else if (!pumpRunning) {
    digitalWrite(RELAY_PIN, HIGH); // Turn on the relay
    pumpRunning = true;
    Serial.println("Pump ON - Dispensing...");
  }

  // Print the total dispensed volume
  Serial.print("Total dispensed: ");
  Serial.print(totalDispensedmL);
  Serial.println(" mL");
}

void resetDispensedVolume() {
  totalDispensedmL = 0.0; // Reset the total dispensed volume
  pumpRunning = false;    // Ensure the pump is off
  digitalWrite(RELAY_PIN, LOW); // Turn off the relay
  Serial.println("Volume reset to 0 mL");
}

void handleRFID() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Card UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Send RFID data to server
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    sendDataToServer("RFID UID: " + uid);
  }
}

void handleLEDsAndBuzzer() {
  // Example of controlling LEDs and buzzer
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  delay(1000);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
}

void sendDataToServer(String data) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST("data=" + data);
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}