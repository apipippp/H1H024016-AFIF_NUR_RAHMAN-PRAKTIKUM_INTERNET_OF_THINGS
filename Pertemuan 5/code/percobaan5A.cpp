/**
 * Modul 5: Pemrosesan Data Sensor dan Analisis Statistik
 * Percobaan 5A: Pengolahan Data Sensor pada ESP32/ESP8266 (Moving Average dan Statistik)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi:
 * Mengimplementasikan pengolahan data lokal (Edge Processing) langsung di sisi mikrokontroler.
 * Menerapkan algoritma filter Moving Average (rata-rata bergerak) dengan ukuran jendela N = 5
 * menggunakan circular buffer untuk meredam derau acak (noise), serta melacak nilai ekstremum
 * (suhu minimum dan suhu maksimum) sepanjang program berjalan.
 */

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

// Parameter filter Moving Average
const int N = 5;          // Jumlah sampel moving average (ukuran jendela)
float buffer[N];          // Array circular buffer untuk menyimpan N pembacaan terakhir
int indeks = 0;           // Posisi indeks penulisan berikutnya pada circular buffer
int jumlah = 0;           // Jumlah sampel yang telah terisi (hingga mencapai N)

// Variabel pelacakan nilai ekstremum
float suhuMin = 1000.0;   // Inisialisasi nilai tinggi agar pembacaan pertama langsung memperbarui suhuMin
float suhuMax = -1000.0;  // Inisialisasi nilai rendah agar pembacaan pertama langsung memperbarui suhuMax

// Fungsi tiruan (Mock function) pengganti sensor fisik DHT yang rusak di lab
float bacaSuhuMock() {
  static unsigned long jumlahBaca = 0;
  jumlahBaca++;

  // 1. Suhu dasar berubah secara bertahap menggunakan kurva sinusoidal
  float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

  // 2. Derau acak kecil untuk mensimulasikan fluktuasi alami sensor (rentang -0.5 s.d. +0.5 °C)
  float noise = random(-5, 6) * 0.1;

  float suhu = suhuDasar + noise;

  // 3. Injeksi lonjakan suhu buatan setiap 25 pembacaan untuk menyimulasikan kejadian anomali
  if (jumlahBaca % 25 == 0) {
    suhu += 8.0;
  }

  return suhu;
}

// Fungsi menghitung rata-rata bergerak menggunakan circular buffer O(1) insertion
float hitungMovingAverage(float nilaiBaru) {
  // Masukkan data baru ke posisi indeks aktif
  buffer[indeks] = nilaiBaru;

  // Geser indeks melingkar menggunakan operasi modulo
  indeks = (indeks + 1) % N;

  // Tambah jumlah sampel jika buffer belum penuh
  if (jumlah < N) {
    jumlah++;
  }

  // Hitung total akumulasi seluruh elemen yang sudah terisi di dalam buffer
  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }

  // Kembalikan nilai rata-rata bergerak
  return total / jumlah;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== SISTEM PENGOLAHAN DATA SENSOR (MOVING AVERAGE) ===");
}

void loop() {
  // Membaca data suhu (dari fungsi simulasi atau sensor)
  float suhu = bacaSuhuMock();

  // Validasi pembacaan
  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    // 1. Terapkan algoritma filter Moving Average
    float suhuFilter = hitungMovingAverage(suhu);

    // 2. Perbarui nilai ekstremum minimum dan maksimum
    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    // 3. Tampilkan hasil komparasi ke Serial Monitor
    Serial.print("Mentah: ");
    Serial.print(suhu, 2);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter, 2);
    Serial.print(" | Min: ");
    Serial.print(suhuMin, 2);
    Serial.print(" | Max: ");
    Serial.println(suhuMax, 2);
  }

  // Jeda pengambilan sampel data setiap 2 detik
  delay(2000);
}
