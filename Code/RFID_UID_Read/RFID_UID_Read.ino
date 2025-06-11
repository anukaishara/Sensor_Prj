#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9          // RFID Module RST pin connected to Digital Pin 9
#define SS_PIN 10          // RFID Module SDA pin connected to Digital Pin 10
#define BUZZER_PIN 7       // Buzzer connected to Digital Pin 7

MFRC522 rfid(SS_PIN, RST_PIN);  // Create instance of the MFRC522 class

void setup() {
  Serial.begin(9600);          // Initialize serial communication
  SPI.begin();                 // Init SPI bus
  rfid.PCD_Init();             // Initialize the RFID reader
  pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as output
  Serial.println("Place your RFID card near the reader...");
}

void loop() {
  // Check if a new card is present
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Card detected, read UID
  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Beep the buzzer when card is detected
  tone(BUZZER_PIN, 1000);  // Set buzzer frequency to 1000 Hz
  delay(200);              // Beep duration
  noTone(BUZZER_PIN);      // Turn off buzzer
  delay(500);              // Short delay between scans

  // Halt PICC (to stop reading the same card multiple times)
  rfid.PICC_HaltA();
}
