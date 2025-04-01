#include <Keypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Balance";
const char* password = "balance1234";

// Endpoint URL
const char* serverUrl = "https://eb0a-197-157-187-129.ngrok-free.app/api/status";

// Flow sensor
#define FLOW_SENSOR_PIN 34
volatile long pulseCount = 0;
float calibrationFactor = 7.5; // Pulses per liter
float flowRate;
unsigned long oldTime;

// Global variables for tracking total mL
float totalDispensedmL = 0.0; // Total milliliters dispensed
bool pumpRunning = false;     // Track if the pump is running
String uid = "";
String amountNumber = ""; 
String codeNumber = ""; 

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

void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Connecting to WiFi...");
    display.display();
  }
  Serial.println("Connected to WiFi");
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connected");
  display.display();
  delay(1000);

  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);
  oldTime = millis();

  pinMode(RELAY_PIN, OUTPUT);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  mfrc522.PCD_Init();
}

void loop() {
  selectType();
}

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void selectType() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Select option:");
  display.println("");
  display.setTextSize(2);
  display.println("1. Card");
  display.println("");
  display.println("2. Code");
  display.display();

  char key = keypad.getKey();
  
  if (key) {
    if (key == '1') {
      handleRFID();
    } else if (key == '2') {
      display.clearDisplay();
      handleCodeNumber();
    } else {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Invalid");
      display.println("");
      display.println("Option");
      display.display();
      delay(1000);
      selectType();
    }
  }
}

void handleRFID() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Tap card");
  display.display();

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Card UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    display.clearDisplay();
    handleAmountNumber();
  }

  handleRFID();
}

void handleCodeNumber() {
  char key = keypad.getKey();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Enter Code:");
  display.display();
  if (key) {
    if (key == '#') {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Sending...");
      display.display();
      sendCodeDataToServer(codeNumber);
    }
    else if (key == '*') {
      codeNumber = "";
    }
    else {
      codeNumber += key;
    }
    
    display.setTextSize(2);
    display.setCursor(0, 16);
    display.println(codeNumber);
    display.display();
  }

  handleCodeNumber();
}

void handleAmountNumber() {
  char key = keypad.getKey();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Enter Amount:");
  display.display();

  if (key) {
    if (key == '#') {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Sending...");
      display.display();
      sendCodeAndAmountDataToServer(amountNumber, uid);
    }
    else if (key == '*') {
      amountNumber = "";
    }
    else {
      amountNumber += key;
    }
    
    display.setTextSize(2);
    display.setCursor(0, 16);
    display.println(amountNumber);
    display.display();
  }

  handleAmountNumber();
}

void handleResponseError(String message) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(3000);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  restartCircuit();
}

void handleResponseCode(String code) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("You can pay");
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.println(code);
  display.display();
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(6000);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  restartCircuit();
}

void handleRelay(int volume) {
  while(1>0){
    int remainingVolume = volume - totalDispensedmL;

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    
    // Display the remaining volume
    display.print("Remaining: ");
    display.print(remainingVolume);
    display.print(" mL");
    display.display();
      if ((millis() - oldTime) > 1000) {
      // Disable the interrupt while calculating flow rate
      detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
      
      // Calculate flow rate (L/min)
      flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
      float volumeDispensed = (flowRate / 60.0) * 1000.0; // Convert L/min to mL/s
      totalDispensedmL += volumeDispensed;
      
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
      // Ensure totalDispensedmL does not go below 0
      if (totalDispensedmL < 0) {
        totalDispensedmL = 0;
      }

      // Control the relay based on the total dispensed volume
      if (totalDispensedmL >= volume) {
        digitalWrite(RELAY_PIN, LOW); // Turn off the relay
        pumpRunning = false;
        Serial.println("Pump OFF - Target volume reached");
        restartCircuit();
      } else if (!pumpRunning) {
        digitalWrite(RELAY_PIN, HIGH); // Turn on the relay
        pumpRunning = true;
        Serial.println("Pump ON - Dispensing...");
      }

      // Print the total dispensed volume (for debugging purposes)
      Serial.print("Total dispensed: ");
      Serial.print(totalDispensedmL);
      Serial.println(" mL");
    }
  }
}

void sendCodeDataToServer(String code) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    DynamicJsonDocument doc(1024);
    doc["code"] = code;
    
    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(jsonString);
    
    if (httpResponseCode > 0) {
      
      String response = http.getString();

      Serial.println("Server Response:");
      Serial.println(response);

      DynamicJsonDocument responseDoc(1024);
      DeserializationError error = deserializeJson(responseDoc, response);

      if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.f_str());
        return;
      }

      const char* status = responseDoc["status"];
      const char* message = responseDoc["message"];
      int amount = responseDoc["amount"].as<int>();

      if (strcmp(status, "2") == 0) {
        handleRelay(amount);
      } else if (strcmp(status, "0") == 0) {
        
        handleResponseError(message);
      }

    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.println("Retrying...");
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Error ");
      display.println("Retrying...");
      display.display();
      delay(2000);
      sendCodeDataToServer(code);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void sendCodeAndAmountDataToServer(String amount, String card) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    DynamicJsonDocument doc(1024);
    doc["amount"] = amount;
    doc["card"] = card;
    
    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    int httpResponseCode = http.POST(jsonString);
    
    if (httpResponseCode > 0) {
      
      String response = http.getString();

      Serial.println("Server Response:");
      Serial.println(response);

      DynamicJsonDocument responseDoc(1024);
      DeserializationError error = deserializeJson(responseDoc, response);

      if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.f_str());
        return;
      }

      const char* status = responseDoc["status"];
      const char* message = responseDoc["message"];
      const char* code = responseDoc["code"];

      if (strcmp(status, "1") == 0) {

        Serial.println("Action: Please Pay");
        Serial.print("Fuel Code: ");
        Serial.println(code);
        handleResponseCode(code);
      } else if (strcmp(status, "0") == 0) {
        
        Serial.println("Action: Card Not Found");
        handleResponseError(message);
      }

    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.println("Retrying...");
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Error ");
      display.println("Retrying...");
      display.display();
      delay(2000);
      sendCodeAndAmountDataToServer(amount, card);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void restartCircuit() {
  Serial.println("Restarting the circuit...");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Restarting the circuit...");
  display.display();
  delay(2000);
  ESP.restart();
}