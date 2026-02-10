# 🔐 Smart Safe – IoT-Based OTP Enabled Locking System

An IoT-powered smart safe system using **ESP32** that implements **two-factor authentication (Password + OTP)** with additional security layers like **XOR-encrypted OTP transmission**, **attempt limiting**, and **automatic relocking**.

---

## 🚀 Features
- Static password authentication via 4×4 keypad
- Time-based OTP (valid for 60 seconds)
- OTP delivery via **Blynk IoT notifications**
- XOR encryption for OTP transmission
- Attempt limiter (locks after 3 wrong tries)
- Servo motor–based locking mechanism
- Buzzer alerts for failures and timeouts
- Auto re-lock after successful access

---

## 🧠 Security Concepts Used
- Two-Factor Authentication (2FA)
- XOR Cipher (lightweight encryption)
- Brute-force attack mitigation
- Replay attack prevention
- Network sniffing resistance (tested with Netcat & Wireshark)

---

## 🛠️ Hardware Components
- ESP32 / ESP8266
- 4×4 Keypad
- Servo Motor
- Buzzer
- Breadboard & Jumper Wires
- Power Supply

---

## 💻 Software & Tools
- Arduino IDE
- Blynk IoT
- WiFiClient
- Netcat
- Wireshark

---

## 📂 Project Structure
