#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin (-1 if sharing Arduino reset pin)

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// Default I2C pins for ESP32: GPIO21(SDA), GPIO22(SCL)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  Serial.println("OLED Display Test");
  
  // Clear the buffer
  display.clearDisplay();
}

void loop() {
  // Display text
  testText();
  delay(2000);
  
  // Display shapes
  testShapes();
  delay(2000);
  
  // Display scrolling text
  testScrollText();
  delay(2000);
}

void testText() {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("OLED Test"));
  
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.println(F("ESP32 IoT"));
  
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println(F("RFID Security"));
  display.setCursor(0, 50);
  display.println(F("System"));
  
  display.display();
}

void testShapes() {
  display.clearDisplay();
  
  // Draw a circle
  display.drawCircle(32, 32, 20, SSD1306_WHITE);
  
  // Draw a rectangle
  display.drawRect(70, 12, 40, 40, SSD1306_WHITE);
  
  // Draw a filled triangle
  display.fillTriangle(30, 60, 50, 40, 70, 60, SSD1306_WHITE);
  
  display.display();
}

void testScrollText() {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Scrolling Text Demo"));
  display.display();
  delay(500);
  
  // Scroll text horizontally
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(500);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
}