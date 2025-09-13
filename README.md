# Arduino-CAN-Security
Using a Arduino UNO v3 to read CAN Bus messages from a simulated ECU.  The simulated ECU is developed using Vector CANoe and transmitted to the Arduino via a Vector VN1640A.  Using Seeed Studio CAN Bus Shield v2 and their library for RX/TX of messages.  Using Arduino Cryptography Library by RWeather for Checksum generation.

# Dependencies
- PlatformIO
- Seed Studio CAN BUS Shield V2 https://wiki.seeedstudio.com/CAN-BUS_Shield_V2.0/
- Arduino Cryptography Library https://github.com/rweather/arduinolibs
