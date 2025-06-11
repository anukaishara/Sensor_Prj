# RFID Based Smart Access Control System with Intruder Detection and Automated Door Mechanism

This project implements a smart access control system using RFID technology, integrated with intruder detection and an automated door mechanism. It is designed to enhance security in residential and commercial settings by providing secure, automated, and monitored access.

## Features

- **RFID Access Control:** Only authorized RFID tags can unlock the door.
- **Intruder Detection:** Detects unauthorized attempts using sensors and triggers alerts.
- **Automated Door Mechanism:** The door opens and closes automatically upon successful authentication.
- **Real-time Notifications:** Notifies users of access attempts and intrusions (customize as needed).
- **Audit Logging:** Maintains a log of entries, exits, and intrusion events.

## Hardware Requirements

- Microcontroller (e.g., Arduino Uno/Nano)
- RFID Reader Module (e.g., MFRC522)
- RFID Tags/Cards
- Servo Motor or Door Lock Mechanism
- Intruder Detection Sensor (e.g., PIR Motion Sensor)
- Buzzer or Alarm Module
- LEDs (for status indication)
- Power Supply
- Optional: Display module (LCD/OLED)

## Software Requirements

- Arduino IDE (or compatible C++ compiler)
- Required Arduino libraries (MFRC522, Servo, etc.)

## Getting Started

### 1. Hardware Setup

Connect the components as follows:

- RFID Reader to microcontroller (SPI pins)
- Servo motor/relay to control door mechanism
- PIR sensor to digital input pin
- Buzzer and LEDs to output pins

### 2. Software Setup

1. Clone this repository:
   ```bash
   git clone https://github.com/anukaishara/Sensor_Prj.git
   ```
2. Open the project in Arduino IDE.
3. Install necessary libraries via Library Manager.
4. Update the code with your authorized RFID tag IDs.
5. Upload the code to your microcontroller.

### 3. Operation

- Present an RFID card/tag to the reader.
- If authorized, the door unlocks and opens automatically.
- If an unauthorized attempt is detected, the system triggers the alarm and logs the event.
- The PIR sensor monitors for intruders, triggering alerts if movement is detected when access is denied.

## Project Structure

```
Sensor_Prj/
├── /Codes                 # Main firmware source code
├── /Documentaion          # Additional documentation
├── /Demonstration         # Demonstration Video
├── README.md              # Description
```

## Customization

- Add more sensors or modules as needed (e.g., GSM for SMS alerts, WiFi for IoT integration).
- Adapt access logging to SD card or cloud database.
- Integrate with home automation platforms.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Author

**anukaishara**

---

Feel free to update with your specific wiring diagrams, demo photos, or additional instructions! If you need this tailored further, let me know.
