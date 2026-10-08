#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>

// =================================================================
// 1. KONFIGURASI WI-FI HOTSPOT & FIREBASE (ISI DI SINI MAS AL)
// =================================================================
#define WIFI_SSID "LASTRI"
#define WIFI_PASSWORD "00181934"

#define DATABASE_URL "https://okupansi-ruangan-default-rtdb.asia-southeast1.firebasedatabase.app/" // Bebas tanpa slash / di akhir tidak apa-apa
#define DATABASE_SECRET "TGjh89t8q9U9I2UrAyNglwIF8kWZnLdVO8fFgnPZ"

// =================================================================
// 2. INISIALISASI HARDWARE & FIREBASE OBJECT
// =================================================================
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int pirPin = 13;

// Objek Firebase dari Library Mobizt
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Pengaturan Waktu Holding Status (10 Detik untuk Tes Praktik)
const unsigned long HOLD_TIME = 10000; 
unsigned long lastMotionTime = 0;
bool statusTerisi = false;

// =================================================================
// 3. SETUP PROGRAM
// =================================================================
void setup() {
  Serial.begin(115200);
  pinMode(pirPin, INPUT);

  // A. Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ruang J.101");
  lcd.setCursor(0, 1);
  lcd.print("WiFi Connecting..");

  // B. Konek ke Wi-Fi Hotspot
  Serial.print("Menghubungkan ke Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Terhubung!");
  Serial.print("IP Address ESP32: ");
  Serial.println(WiFi.localIP());

  // C. Inisialisasi Firebase Database
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // D. Set Tampilan Awal LCD & Serial
  lcd.setCursor(0, 1);
  lcd.print("Status: KOSONG ");

  // Set Status Awal di Firebase
  Firebase.setString(fbdo, "/Ruangan/J101/status", "KOSONG");

  Serial.println("=========================================");
  Serial.println("  Sistem Okupansi IoT Firebase Sederhana ");
  Serial.println("=========================================");
}

// =================================================================
// 4. LOOP PROGRAM MAIN LOGIC
// =================================================================
void loop() {
  int pirState = digitalRead(pirPin);
  unsigned long currentMillis = millis();

  // A. Jika PIR mendeteksi Sinyal HIGH (Ada Gerakan)
  if (pirState == HIGH) {
    lastMotionTime = currentMillis; // Reset timer waktu gerakan

    if (!statusTerisi) {
      statusTerisi = true;
      
      // Update Tampilan LCD
      lcd.setCursor(0, 1);
      lcd.print("Status: TERISI ");
      Serial.println("[EVENT] Ada Gerakan! Status Local -> TERISI");

      // Kirim Data Realtime ke Cloud Firebase!
      if (Firebase.setString(fbdo, "/Ruangan/J101/status", "TERISI")) {
        Serial.println("[FIREBASE SUCCESS] Status 'TERISI' Berhasil Terkirim ke Cloud!");
      } else {
        Serial.print("[FIREBASE ERROR] Gagal Kirim: ");
        Serial.println(fbdo.errorReason());
      }
    }
  }

  // B. Jika status TERISI dan tidak ada gerakan selama melebihi HOLD_TIME
  if (statusTerisi && (currentMillis - lastMotionTime >= HOLD_TIME)) {
    statusTerisi = false;
    
    // Update Tampilan LCD
    lcd.setCursor(0, 1);
    lcd.print("Status: KOSONG ");
    Serial.println("[EVENT] Waktu Tunggu Habis! Status Local -> KOSONG");

    // Kirim Data Realtime ke Cloud Firebase!
    if (Firebase.setString(fbdo, "/Ruangan/J101/status", "KOSONG")) {
      Serial.println("[FIREBASE SUCCESS] Status 'KOSONG' Berhasil Terkirim ke Cloud!");
    } else {
      Serial.print("[FIREBASE ERROR] Gagal Kirim: ");
      Serial.println(fbdo.errorReason());
    }
  }

  delay(200);
}