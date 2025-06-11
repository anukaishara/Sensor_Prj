#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pins
#define RST_PIN 9
#define SS_PIN 10
#define PIR_PIN 2
#define GREEN_LED_PIN 5
#define RED_LED_PIN 3
#define BUZZER_PIN 7
#define SERVO_PIN 6

// I2C addresses
#define MPU_ADDR 0x68 // MPU-6050 I2C address
#define LCD_ADDR 0x3F // LCD I2C Address

// RFID setup
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo doorServo;
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

// Thresholds
#define MOTION_THRESHOLD 200  // Accelerometer motion threshold
#define BREACH_THRESHOLD 1000  // Gyroscope breach threshold

// States
bool servoMoving = false;
bool breachDetected = false; // Indicates if the system is in breach mode

// Buffers for storing cumulative values over 1 second
long accelXSum = 0, accelYSum = 0, accelZSum = 0;
long gyroXSum = 0, gyroYSum = 0, gyroZSum = 0;
int dataCount = 0; // Number of readings in 1 second

// Previous averages
long prevAccelX = 0, prevAccelY = 0, prevAccelZ = 0;
long prevGyroX = 0, prevGyroY = 0, prevGyroZ = 0;

// Timing for 1-second interval
unsigned long lastUpdate = 0;

// Authorized UID arrays
byte authorizedUID1[] = {0xB4, 0xE7, 0xDD, 0xE0};
byte authorizedUID2[] = {0xC2, 0x60, 0xA7, 0x1B};

// LCD state
bool isDisplayingScanMessage = false;

// Alarm timing
unsigned long lastBlinkTime = 0;
bool isLedOn = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System");
  lcd.setCursor(0, 1);
  lcd.print("Initializing");
  delay(1000);
  lcd.clear();
  lcd.noBacklight();

  pinMode(PIR_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  doorServo.attach(SERVO_PIN);
  doorServo.write(0);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);
  Wire.endTransmission();

  lcd.clear();
  lcd.backlight();
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
  lcd.noBacklight();
}

void loop() {
  if (breachDetected) {
    handleAlarm();
  }

  // Read data from MPU-6050
  int16_t ax, ay, az, gx, gy, gz;
  readMPU6050(ax, ay, az, gx, gy, gz);

  // Accumulate values
  accelXSum += ax;
  accelYSum += ay;
  accelZSum += az;
  gyroXSum += gx;
  gyroYSum += gy;
  gyroZSum += gz;
  dataCount++;

  // Every 1 second, calculate averages and check for motion or breach
  if (millis() - lastUpdate >= 1000) {
    // Calculate averages
    long avgAccelX = accelXSum / dataCount;
    long avgAccelY = accelYSum / dataCount;
    long avgAccelZ = accelZSum / dataCount;
    long avgGyroX = gyroXSum / dataCount;
    long avgGyroY = gyroYSum / dataCount;
    long avgGyroZ = gyroZSum / dataCount;

    // Reset accumulators
    accelXSum = accelYSum = accelZSum = 0;
    gyroXSum = gyroYSum = gyroZSum = 0;
    dataCount = 0;

    // Print averages for debugging
    Serial.print("Accel Avg: X=");
    Serial.print(avgAccelX);
    Serial.print(", Y=");
    Serial.print(avgAccelY);
    Serial.print(", Z=");
    Serial.print(avgAccelZ);
    Serial.print(" | Gyro Avg: X=");
    Serial.print(avgGyroX);
    Serial.print(", Y=");
    Serial.print(avgGyroY);
    Serial.print(", Z=");
    Serial.println(avgGyroZ);

if (abs(avgAccelX - prevAccelX) > MOTION_THRESHOLD ||
        abs(avgAccelY - prevAccelY) > MOTION_THRESHOLD ||
        abs(avgAccelZ - prevAccelZ) > MOTION_THRESHOLD) {
      Serial.println("======================");
      Serial.println("");
      Serial.println("Motion Detected");
      Serial.println("");
      Serial.println("======================");
    }



    // Check for motion or breach
    if (!breachDetected &&
        (abs(avgGyroX - prevGyroX) > BREACH_THRESHOLD ||
         abs(avgGyroY - prevGyroY) > BREACH_THRESHOLD ||
         abs(avgGyroZ - prevGyroZ) > BREACH_THRESHOLD)) {
      breachDetected = true;
      Serial.println("Breach Detected");
      lcd.clear();
      lcd.print("Security Breach!");
    }

    // Update previous averages
    prevAccelX = avgAccelX;
    prevAccelY = avgAccelY;
    prevAccelZ = avgAccelZ;
    prevGyroX = avgGyroX;
    prevGyroY = avgGyroY;
    prevGyroZ = avgGyroZ;

    // Update timing
    lastUpdate = millis();
  }

  // Handle PIR sensor and RFID in normal mode
  if ( digitalRead(PIR_PIN)) {
    if (!isDisplayingScanMessage) {
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("Scan the");
      lcd.setCursor(0, 1);
      lcd.print("Key Card");
      isDisplayingScanMessage = true;
    }

    // Check for RFID card
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      if (isAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size)) {
        accessGranted();
      } else {
        accessDenied();
      }
      mfrc522.PICC_HaltA(); // Stop RFID module
    }
  } else if (!breachDetected && isDisplayingScanMessage) {
    lcd.clear();
    isDisplayingScanMessage = false;
  }

  delay(50); // Small delay for smoother operation
}

void handleAlarm() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastBlinkTime >= 500) {
    isLedOn = !isLedOn;
    digitalWrite(RED_LED_PIN, isLedOn);
    if (isLedOn) {
      tone(BUZZER_PIN, 1000);
    } else {
      noTone(BUZZER_PIN);
    }
    lastBlinkTime = currentMillis;
  }

  // Check for RFID card to turn off alarm
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    if (isAuthorized(mfrc522.uid.uidByte, mfrc522.uid.size)) {
      breachDetected = false;
      noTone(BUZZER_PIN);
      delay(500);
      digitalWrite(RED_LED_PIN, LOW);
      lcd.clear();
      lcd.backlight();
      lcd.print("Alarm Disarmed");
      digitalWrite(GREEN_LED_PIN, HIGH);
      delay(1000);
      digitalWrite(GREEN_LED_PIN, LOW);
      lcd.clear();
      lcd.noBacklight();
    }
    mfrc522.PICC_HaltA();
  }
}

void readMPU6050(int16_t &ax, int16_t &ay, int16_t &az, int16_t &gx, int16_t &gy, int16_t &gz) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // Starting register for accelerometer data
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  ax = (Wire.read() << 8 | Wire.read());
  ay = (Wire.read() << 8 | Wire.read());
  az = (Wire.read() << 8 | Wire.read());

  // Skip temperature data
  Wire.read();
  Wire.read();

  gx = (Wire.read() << 8 | Wire.read());
  gy = (Wire.read() << 8 | Wire.read());
  gz = (Wire.read() << 8 | Wire.read());
}

bool isAuthorized(byte *uid, byte size) {
  if (size != 4) return false;

  // Check against the first authorized UID
  bool match1 = true;
  for (byte i = 0; i < size; i++) {
    if (uid[i] != authorizedUID1[i]) {
      match1 = false;
      break;
    }
  }

  // Check against the second authorized UID
  bool match2 = true;
  for (byte i = 0; i < size; i++) {
    if (uid[i] != authorizedUID2[i]) {
      match2 = false;
      break;
    }
  }

  // Return true if either UID matches
  return match1 || match2;
}


void accessGranted() {
  servoMoving = true;
  lcd.backlight();
  lcd.clear();
  lcd.print("Access Granted");
  digitalWrite(GREEN_LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);
  delay(1000);
  doorServo.write(180);
  noTone(BUZZER_PIN);
  delay(5000);
  doorServo.write(0);
  digitalWrite(GREEN_LED_PIN, LOW);
  delay(1000);
  lcd.clear();
  lcd.noBacklight();
  servoMoving = false;
}

void accessDenied() {
  servoMoving = true;
  lcd.clear();
  lcd.backlight();
  lcd.print("Access Denied");
  digitalWrite(RED_LED_PIN, HIGH);
  tone(BUZZER_PIN, 500);
  delay(1000);
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);
  delay(1000);
  lcd.clear();
  lcd.noBacklight();
  servoMoving = false;
}