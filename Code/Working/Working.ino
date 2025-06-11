#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define pins
#define RST_PIN 9
#define SS_PIN 10
#define PIR_PIN 2
#define VIBRATION_PIN 3
#define GREEN_LED_PIN 5
#define RED_LED_PIN 4
#define BUZZER_PIN 7
#define SERVO_PIN 6

// RFID and LCD setup
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo doorServo;
LiquidCrystal_I2C lcd(0x3F, 16, 2); // Update address if needed

// Authorized card UIDs
byte authorizedUID1[] = {0xB4, 0xE7, 0xDD, 0xE0};
byte authorizedUID2[] = {0xC2, 0x60, 0xA7, 0x1B};

// Flags and timing
bool rfidActive = false;
unsigned long lastActiveTime = 0;
const unsigned long timeoutDuration = 5000; // 5-second timeout

void setup() {
  Serial.begin(9600);
  SPI.begin();
  
  pinMode(PIR_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  doorServo.attach(SERVO_PIN);
  lcd.init();
  lcd.noBacklight(); // Start with LCD off

  doorServo.write(0); // Close door initially
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

}

void loop() {
  bool motionDetected = digitalRead(PIR_PIN);
  bool vibrationDetected = digitalRead(VIBRATION_PIN);

  // Check for forced entry
  if (vibrationDetected) {
    triggerForcedEntryAlarm();
  }

  // Activate RFID and LCD on motion
  if (motionDetected) {
    activateRFIDandLCD();
  }

  // Check for timeout to turn off RFID and LCD
  if (rfidActive && millis() - lastActiveTime > timeoutDuration) {
    deactivateRFIDandLCD();
  }

  // Process RFID scan only if RFID is active
  if (rfidActive && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (isAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size)) {
      accessGranted();
    } else {
      accessDenied();
    }
    mfrc522.PICC_HaltA(); // Stop RFID scanning
    lastActiveTime = millis(); // Reset timeout after a card scan
  }
}

void activateRFIDandLCD() {
  if (!rfidActive) {
    mfrc522.PCD_Init(); // Initialize RFID scanner
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan the key");
    lcd.setCursor(0, 1);
    lcd.print("card");

    lastActiveTime = millis(); // Start timeout
    rfidActive = true;

  }
}

void deactivateRFIDandLCD() {
  mfrc522.PCD_AntennaOff(); // Turn off RFID scanner
  lcd.clear();
  lcd.noBacklight();

  rfidActive = false;

}

bool isAuthorized(byte *uid, byte size) {
  if (size != 4) return false;
  
  bool matchUID1 = true;
  for (byte i = 0; i < size; i++) {
    if (uid[i] != authorizedUID1[i]) {
      matchUID1 = false;
      break;
    }
  }
  
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
  lcd.clear();
  lcd.print("Access Granted");
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  tone(BUZZER_PIN, 1000, 500); // Short beep for success
  delay(500);
  doorServo.write(90); // Open door
  delay(3000); // Keep door open for 3 seconds
  doorServo.write(0); // Close door
  
  digitalWrite(GREEN_LED_PIN, LOW);
  lcd.clear();
  lcd.print("System Idle");
  Serial.println("Access granted");
}

void accessDenied() {
  lcd.clear();
  lcd.print("Access Rejected");
  digitalWrite(RED_LED_PIN, HIGH);
  tone(BUZZER_PIN, 500, 1000); // Longer beep for rejection
  delay(1000);
  digitalWrite(RED_LED_PIN, LOW);
  lcd.clear();
  lcd.print("System Idle");
  Serial.println("Access denied");
}

void triggerForcedEntryAlarm() {
  lcd.clear();
  lcd.print("Forced Entry!");
  bool authorized = false;
  
  while (!authorized) { 
    digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN)); // Blink red LED
    tone(BUZZER_PIN, 2000); // High-pitched alarm
    delay(500);
    
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      authorized = isAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size);
      mfrc522.PICC_HaltA(); // Stop RFID scanning
    }
  }
  
  noTone(BUZZER_PIN);
  digitalWrite(RED_LED_PIN, LOW);
  lcd.clear();
  lcd.print("Alarm Disarmed");
  delay(2000);
  lcd.clear();
  lcd.print("System Idle");
}
