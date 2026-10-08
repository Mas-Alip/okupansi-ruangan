#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pirPin = 13;

// Pengaturan Waktu Tunggu Tahan Status (10 detik untuk tes)
const unsigned long HOLD_TIME = 10000; 

unsigned long lastMotionTime = 0;
bool statusTerisi = false;

void setup() {
  Serial.begin(115200);
  
  // Gunakan INPUT_PULLDOWN internal ESP32 agar pin tidak mudah menangkap sinyal statis
  pinMode(pirPin, INPUT_PULLDOWN);

  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Ruang J.101");
  lcd.setCursor(0, 1);
  lcd.print("Status: KOSONG ");

  Serial.println("=========================================");
  Serial.println("   Sistem Okupansi (Kalibrasi PIR)");
  Serial.println("=========================================");
  Serial.println("Pemanasan sensor 10 detik...");
  delay(10000); // Waktu kalibrasi awal sensor PIR
  Serial.println("Sensor PIR SIAP!");
}

void loop() {
  int pirState = digitalRead(pirPin);
  unsigned long currentMillis = millis();

  // Jika PIR mendeteksi sinyal HIGH (ada gerakan)
  if (pirState == HIGH) {
    lastMotionTime = currentMillis; // Reset timer waktu gerakan terakhir

    if (!statusTerisi) {
      statusTerisi = true;
      lcd.setCursor(0, 1);
      lcd.print("Status: TERISI ");
      Serial.println("[EVENT] Ada Gerakan! Status -> TERISI");
    }
  }

  // Jika status TERISI dan tidak ada gerakan selama melebihi HOLD_TIME
  if (statusTerisi && (currentMillis - lastMotionTime >= HOLD_TIME)) {
    statusTerisi = false;
    lcd.setCursor(0, 1);
    lcd.print("Status: KOSONG ");
    Serial.println("[EVENT] Waktu Tunggu Habis! Status -> KOSONG");
  }

  delay(200);
}