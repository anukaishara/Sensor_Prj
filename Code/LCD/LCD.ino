#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2); // Use address 0x3F

void setup() {
  lcd.init();              // Initialize the LCD
  lcd.backlight();         // Turn on the backlight
  lcd.setCursor(0, 0);     // Move the cursor to the first column, first row
  lcd.print("Hello, World!"); // Print a test message
}

void loop() {
  // No code here; the message should stay static on the LCD
}

