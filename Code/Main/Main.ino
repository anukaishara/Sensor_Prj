#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pins
#define RST_PIN 9
#define SS_PIN 10
#define PIR_PIN 2
#define VIBRATION_PIN 3
#define GREEN_LED_PIN 5
#define RED_LED_PIN 4
#define BUZZER_PIN 7
#define SERVO_PIN 6

// RFID setup
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Servo and LCD setup
Servo doorServo;
LiquidCrystal_I2C lcd(0x3F, 16, 2); // Adjust I2C address if needed

// Define authorized card UIDs
byte authorizedUID1[] = {0xB4, 0xE7, 0xDD, 0xE0};  // First new authorized UID
byte authorizedUID2[] = {0xC2, 0x60, 0xA7, 0x1B};  // Second new authorized UID

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  // Initialize components
  pinMode(PIR_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Servo and LCD
  doorServo.attach(SERVO_PIN);
  lcd.init();
  lcd.backlight();

  // Set initial states
  doorServo.write(0); // Close door initially
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
}

void loop() {
  bool motionDetected = digitalRead(PIR_PIN);
  bool vibrationDetected = digitalRead(VIBRATION_PIN);
  
  // Check for forced entry when system is idle
  if (vibrationDetected) {
    triggerForcedEntryAlarm(); // Trigger alarm immediately on vibration
  }
  
  // If motion detected, activate RFID reader
  if (motionDetected) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan the");
    lcd.setCursor(0,1);
    lcd.print("Key Card");

    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // Check if the scanned card is authorized
      if (isAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size)) {
        accessGranted();
      } else {
        accessDenied();
      }
      mfrc522.PICC_HaltA(); // Stop RFID module
    }
  }
}

// Check if scanned UID matches any authorized UID
bool isAuthorized(byte *uid, byte size) {
  if (size != 4) return false; // Ensure UID is 4 bytes
  
  // Check if UID matches authorizedUID1
  bool matchUID1 = true;
  for (byte i = 0; i < size; i++) {
    if (uid[i] != authorizedUID1[i]) {
      matchUID1 = false;
      break;
    }
  }
  
  // Check if UID matches authorizedUID2 if first UID didn't match
  bool matchUID2 = true;
  if (!matchUID1) {
    for (byte i = 0; i < size; i++) {
      if (uid[i] != authorizedUID2[i]) {
        matchUID2 = false;
        break;
      }
    }
  }
  
  return matchUID1 || matchUID2;
}

void accessGranted() {
  // Indicate successful access
  lcd.clear();
  lcd.print("Access Granted");
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  tone(BUZZER_PIN, 1000, 500); // Short beep for success
  delay(500);
  doorServo.write(90); // Open door
  delay(3000); // Keep door open for 3 seconds
  doorServo.write(0); // Close door
  
  // Reset indicators
  digitalWrite(GREEN_LED_PIN, LOW);
}

void accessDenied() {
  // Indicate failed access
  lcd.clear();
  lcd.print("Access Rejected");
  digitalWrite(RED_LED_PIN, HIGH);
  tone(BUZZER_PIN, 500, 1000); // Longer beep for rejection
  delay(1000);
  digitalWrite(RED_LED_PIN, LOW);
}

void triggerForcedEntryAlarm() {
  // Trigger alarm for forced entry
  lcd.clear();
  lcd.print("Forced Entry!");
  
  while (true) { // Continuous loop until an authorized card is scanned
    digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN)); // Blink red LED
    tone(BUZZER_PIN, 2000); // High-pitched alarm
    delay(500);

    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      if (isAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size)) {
        // Authorized card scanned, disarm the alarm
        noTone(BUZZER_PIN);
        digitalWrite(RED_LED_PIN, LOW);
        lcd.clear();
        lcd.print("Alarm Disarmed");
        delay(2000); // Show message briefly
        mfrc522.PICC_HaltA(); // Halt RFID scanner
        break; // Exit the loop to allow future detections
      }
    }
  }
}
