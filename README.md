# 🚪 RFID Based Smart Access Control System with Intruder Detection and Automated Door Mechanism

This project implements a smart access control system using RFID technology, enhanced with intruder detection and an automated door mechanism. Designed for both residential and commercial environments, it ensures secure, automated, and monitored access.

---

## 🌟 Features

- **RFID Access Control:** Only authorized RFID tags or cards can unlock the door.
- **PIR Sensor Activation:** The system uses a PIR (Passive Infrared) sensor to power up when movement is detected, improving energy efficiency and responsiveness.
- **Intruder Detection with MPU6050:** An MPU6050 accelerometer and gyroscope monitor door movements. Forceful or unusual actions—suggesting a possible break-in—trigger immediate alerts.
- **Automated Door Mechanism:** The door opens and closes automatically after successful authentication.
- **Real-time Notifications:** Users are notified of access attempts and intrusion events. (Customizable as needed.)
- **Audit Logging:** Every entry, exit, and intrusion event is logged for future review.

---

## 🛠️ Hardware Requirements

- Microcontroller (e.g., Arduino Uno/Nano)
- RFID Reader Module (e.g., MFRC522)
- RFID Tags or Cards
- Servo Motor / Door Lock Mechanism
- PIR Motion Sensor (for motion detection and system activation)
- MPU6050 Accelerometer & Gyroscope (for door/intrusion monitoring)
- Buzzer or Alarm Module
- LEDs (for status indication)
- Power Supply
- (Optional) Display module (LCD/OLED)

---

## 💻 Software Requirements

- Arduino IDE (or compatible C++ compiler)
- Required Arduino libraries:
  - `MFRC522`
  - `Servo`
  - `Wire`
  - `Adafruit_MPU6050` (or similar)
- Additional libraries as needed for your hardware

---

## 🚦 System Overview

- **PIR Sensor Activation:** The PIR sensor powers up the system only when movement is detected, ensuring energy savings and quick readiness.
- **Intruder Detection:** The MPU6050 monitors the door for abnormal movements. If forceful or suspicious activity is detected, the system triggers an alert, indicating potential forced entry.

---

## ⚙️ Getting Started

### Hardware Setup

Connect the components as follows:

- RFID Reader to microcontroller (SPI pins)
- Servo motor/relay for door control
- PIR sensor to a digital input pin
- MPU6050 to I2C pins (SCL/SDA)
- Buzzer and LEDs to output pins

### Software Setup

1. **Clone this repository:**
   ```bash
   git clone https://github.com/anukaishara/Sensor_Prj.git
   ```
2. **Open the project in Arduino IDE.**
3. **Install the necessary libraries via Library Manager.**
4. **Update the code with your authorized RFID tag IDs.**
5. **Upload the code to your microcontroller.**

---

## 🚀 Operation

1. Present your RFID card/tag to the reader.
2. If authorized, the door unlocks and opens automatically.
3. If unauthorized, the alarm is triggered and the event is logged.
4. The PIR sensor wakes the system on movement.
5. The MPU6050 monitors for forced door movements and triggers alerts if a break-in is detected.

---

## 📁 Project Structure

```
Sensor_Prj/
├── Codes/           # Main firmware source code
├── Documentation/   # Additional documentation
├── Demonstration/   # Demonstration videos
├── README.md        # Project description and instructions
```

---

## 🔧 Customization

- Add more sensors/modules (e.g., GSM for SMS alerts, WiFi for IoT integration)
- Adapt logging to SD card or cloud database
- Integrate with home automation platforms (e.g., Home Assistant)

---

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

## 👤 Author

**anukaishara**
