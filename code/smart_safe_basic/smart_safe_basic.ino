/***************************************************
 Smart Safe – Basic Version
 ESP32 Password + OTP Verification System
 - 4x4 Keypad
 - Servo Motor Lock
 - Buzzer Alerts
 - OTP via Blynk IoT
 - 3 Attempt Limit
 - 60s OTP Timeout
***************************************************/

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Safe"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <Keypad.h>

// -------- WiFi Credentials --------
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// -------- Security Settings --------
const String SECRET_PASSWORD = "1234";
int passwordAttempts = 0;
const int MAX_ATTEMPTS = 3;

int otp = 0;
bool otpActive = false;
unsigned long otpStartTime = 0;
const unsigned long OTP_TIMEOUT = 60000; // 60 seconds

// -------- Servo --------
Servo lockServo;
const int SERVO_PIN = 4;
const int LOCKED = 0;
const int UNLOCKED = 90;

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

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String inputBuffer = "";

// -------- Timers --------
BlynkTimer timer;

// -------- Helper Functions --------
void beepAlert() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
}

void lockSafe() {
  lockServo.write(LOCKED);
  Serial.println("🔒 Safe Locked");
}

void unlockSafe() {
  lockServo.write(UNLOCKED);
  Serial.println("🔓 Safe Unlocked");
}

void generateOTP() {
  otp = random(1000, 9999);
  otpActive = true;
  otpStartTime = millis();

  String message = "Your OTP is: " + String(otp);
  Blynk.logEvent("otp_sent", message);

  Serial.println("📲 OTP Sent via Blynk");
}

void checkOTPTimeout() {
  if (otpActive && millis() - otpStartTime > OTP_TIMEOUT) {
    Serial.println("⏰ OTP Expired");
    otpActive = false;
    lockSafe();
    beepAlert();
  }
}

// -------- Setup --------
void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  lockServo.attach(SERVO_PIN);
  lockSafe();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, checkOTPTimeout);

  Serial.println("\nSystem Ready. Enter Password.");
}

// -------- Loop --------
void loop() {
  Blynk.run();
  timer.run();

  char key = keypad.getKey();
  if (!key) return;

  if (key == '#') {
    if (!otpActive) {
      if (inputBuffer == SECRET_PASSWORD) {
        Serial.println("✅ Password Correct");
        generateOTP();
      } else {
        passwordAttempts++;
        Serial.println("❌ Wrong Password");
        beepAlert();

        if (passwordAttempts >= MAX_ATTEMPTS) {
          Serial.println("🚫 System Locked");
          while (true);
        }
      }
    } else {
      if (inputBuffer.toInt() == otp) {
        unlockSafe();
        delay(10000);
        lockSafe();
        otpActive = false;
      } else {
        Serial.println("❌ Wrong OTP");
        beepAlert();
      }
    }
    inputBuffer = "";
  }
  else {
    inputBuffer += key;
    Serial.print("*");
  }
}
