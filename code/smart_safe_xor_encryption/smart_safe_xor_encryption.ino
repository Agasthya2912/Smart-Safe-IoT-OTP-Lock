/***************************************************
 Smart Safe – XOR Encrypted Version
 - Password + OTP Authentication
 - XOR Encrypted OTP Transmission
 - Brute-force Protection
***************************************************/

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Safe"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <Keypad.h>

// -------- Encryption --------
const byte XOR_KEY = 0xAA;

// -------- WiFi --------
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// -------- Security --------
const String SECRET_PASSWORD = "1234";
int attempts = 0;
const int MAX_ATTEMPTS = 3;

int otp;
bool otpActive = false;
unsigned long otpStart;
const unsigned long OTP_TIMEOUT = 60000;

// -------- Servo --------
Servo servo;
const int SERVO_PIN = 4;

// -------- Buzzer --------
const int BUZZER_PIN = 15;

// -------- Keypad --------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
 {'1','2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}
};

byte rowPins[ROWS] = {13,12,14,27};
byte colPins[COLS] = {26,25,33,32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String buffer = "";

BlynkTimer timer;

// -------- XOR Function --------
String xorEncrypt(String data) {
  String result = "";
  for (int i = 0; i < data.length(); i++) {
    result += char(data[i] ^ XOR_KEY);
  }
  return result;
}

// -------- Helpers --------
void buzz() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
}

void lockSafe() {
  servo.write(0);
}

void unlockSafe() {
  servo.write(90);
}

void generateOTP() {
  otp = random(1000, 9999);
  otpActive = true;
  otpStart = millis();

  String encryptedOtp = xorEncrypt(String(otp));
  Blynk.logEvent("otp_sent", "Encrypted OTP Sent");

  WiFiClient client;
  if (client.connect("192.168.1.100", 8080)) {
    client.print(encryptedOtp);
    client.stop();
  }

  Serial.println("🔐 Encrypted OTP Transmitted");
}

void checkTimeout() {
  if (otpActive && millis() - otpStart > OTP_TIMEOUT) {
    otpActive = false;
    lockSafe();
    buzz();
  }
}

// -------- Setup --------
void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);

  servo.attach(SERVO_PIN);
  lockSafe();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, checkTimeout);

  Serial.println("System Ready");
}

// -------- Loop --------
void loop() {
  Blynk.run();
  timer.run();

  char key = keypad.getKey();
  if (!key) return;

  if (key == '#') {
    if (!otpActive) {
      if (buffer == SECRET_PASSWORD) {
        generateOTP();
      } else {
        attempts++;
        buzz();
        if (attempts >= MAX_ATTEMPTS) {
          Serial.println("🚫 Locked Permanently");
          while (true);
        }
      }
    } else {
      if (buffer.toInt() == otp) {
        unlockSafe();
        delay(10000);
        lockSafe();
        otpActive = false;
      } else {
        buzz();
      }
    }
    buffer = "";
  } else {
    buffer += key;
    Serial.print("*");
  }
}
