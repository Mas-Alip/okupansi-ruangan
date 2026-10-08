#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>

// 1. KONFIGURASI WI-FI HOTSPOT & FIREBASE
#define WIFI_SSID "LASTRI"
#define WIFI_PASSWORD "00181934"

#define DATABASE_URL "https://okupansi-ruangan-default-rtdb.asia-southeast1.firebasedatabase.app"
#define DATABASE_SECRET "TGjh89t8q9U9I2UrAYngIwlF8kWZnLdVO8fFgnPZ"

// 2. INISIALISASI HARDWARE & PIN
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pirPin = 13;
const int trigPin = 5;
const int echoPin = 18;

const int JARAK_AMBANG_PINTU = 80; // (cm)
const unsigned long HOLD_TIME = 10000; // 10 Detik untuk simulasi tes cepat

unsigned long lastActivityTime = 0;
bool statusTerisi = false;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

long bacaJarakUltrasonik() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);

  pinMode(pirPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ruang J.101");
  lcd.setCursor(0, 1);
  lcd.print("WiFi Connecting..");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Terhubung!");

  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  lcd.setCursor(0, 1);
  lcd.print("Status: KOSONG ");

  // HANYA UPDATE SENSOR STATUS (Tidak Menimpa Info Dosen)
  Firebase.setString(fbdo, "/Ruangan/J101/status", "KOSONG");

  Serial.println("=============================================");
  Serial.println(" SISTEM COMBINATION OKUPANSI RUANG READY!");
  Serial.println("=============================================");
}

void loop() {
  int pirState = digitalRead(pirPin);
  long jarakCm = bacaJarakUltrasonik();
  unsigned long currentMillis = millis();

  bool adaAktivitasPIR = (pirState == HIGH);
  bool adaOrangDiPintu = (jarakCm > 0 && jarakCm <= JARAK_AMBANG_PINTU);

  // A. Jika Sensor Mendeteksi Aktivitas (PIR ATAU Ultrasonik)
  if (adaAktivitasPIR || adaOrangDiPintu) {
    lastActivityTime = currentMillis;

    if (!statusTerisi) {
      statusTerisi = true;

      lcd.setCursor(0, 1);
      lcd.print("Status: TERISI ");

      Serial.println("[EVENT] Hardware Detecting Activity -> Sending TERISI to Firebase");
      Firebase.setString(fbdo, "/Ruangan/J101/status", "TERISI");
    }
  }

  // B. Jika Tidak Ada Aktivitas & Hold Time Habis
  if (statusTerisi && (currentMillis - lastActivityTime >= HOLD_TIME)) {
    statusTerisi = false;

    lcd.setCursor(0, 1);
    lcd.print("Status: KOSONG ");

    Serial.println("[EVENT] Hold Timer Expired -> Sending KOSONG to Firebase");
    Firebase.setString(fbdo, "/Ruangan/J101/status", "KOSONG");
  }

  delay(250);
}