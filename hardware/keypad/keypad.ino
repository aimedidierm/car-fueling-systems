#include <Keypad.h>

// Define the keypad layout
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

// Define the keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

// Connect row pins
byte rowPins[ROWS] = {32, 33, 25, 26};
byte colPins[COLS] = {17, 16, 14};

// Create the keypad object
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Keypad Test");
  Serial.println("Press any key on the keypad...");
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
  }
}