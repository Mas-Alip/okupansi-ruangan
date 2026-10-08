#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin HC-SR04
const int trigPin = 5;   // Pin Trig di GPIO 4 (D4)
const int echoPin = 18;  // Pin Echo di GPIO 18 (D18)

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Kalibrasi HC-SR04");
  
  Serial.println("=================================");
  Serial.println(" KALIBRASI SENSOR HC-SR04 ESP32 ");
  Serial.println("=================================");
}

// Fungsi Khusus Mengukur Jarak dengan Filter & Timeout
long bacaJarakCm() {
  // 1. Pastikan Trig LOW sebentar
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  // 2. Kirim Pulsa Trig HIGH selama 10 mikrodetik murni
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 3. Baca Echo dengan Timeout 30000 us (Maksimal jarak ~400 cm)
  long duration = pulseIn(echoPin, HIGH, 30000); 

  // Jika durasi 0 (tidak ada pantulan/di luar jangkauan)
  if (duration == 0) {
    return -1; // Tanda Out of Range
  }

  // Hitung jarak dalam cm
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  long jarak = bacaJarakCm();

  if (jarak == -1 || jarak > 400) {
    Serial.println("Jarak: Di luar jangkauan ( > 400 cm )");
    lcd.setCursor(0, 1);
    lcd.print("Jarak: > 400 cm ");
  } else {
    Serial.print("Jarak Objek: ");
    Serial.print(jarak);
    Serial.println(" cm");

    lcd.setCursor(0, 1);
    lcd.print("Jarak: ");
    lcd.print(jarak);
    lcd.print(" cm    ");
  }

  delay(300); // Pembacaan setiap 0.3 detik
}