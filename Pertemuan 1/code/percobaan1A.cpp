#include <DHT.h>

#define DHTPIN 4          // pin data DHT11 terhubung ke GPIO 4
#define DHTTYPE DHT11     // tipe sensor yang digunakan diubah menjadi DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();            // inisialisasi sensor DHT11
  Serial.println("Memulai akuisisi data sensor DHT11...");
}

void loop() {
  // Membaca data kelembaban dan suhu
  float kelembaban = dht.readHumidity();
  float suhu = dht.readTemperature();

  // Periksa apakah pembacaan berhasil
  if (isnan(kelembaban) || isnan(suhu)) {
    Serial.println("Gagal membaca data dari sensor DHT11!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C, Kelembaban: ");
    Serial.print(kelembaban);
    Serial.println("%");
  }
  
  delay(2000); // jeda pembacaan setiap 2 detik
}