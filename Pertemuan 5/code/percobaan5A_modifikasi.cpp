/**
 * Modul 5: Pemrosesan Data Sensor dan Analisis Statistik
 * Percobaan 5A (Modifikasi): Moving Average dengan Perhitungan Simpangan Baku (Standard Deviation)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi Modifikasi:
 * Menambahkan fungsi hitungStandarDeviasi() untuk menghitung simpangan baku sampel
 * dari data yang tersimpan pada circular buffer mikrokontroler.
 * Rumus varians sampel: s^2 = sum((x_i - mean)^2) / (jumlah - 1)
 * Simpangan baku: s = sqrt(varians)
 */

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <math.h> // Library matematika untuk fungsi sqrt()

// 1. Parameter ukuran jendela circular buffer
const int N = 5;
float buffer[N];
int indeks = 0;
int jumlah = 0;

// 2. Variabel pelacakan suhu minimum dan maksimum
float suhuMin = 1000.0;
float suhuMax = -1000.0;

// 3. Mock function penghasil data suhu sintetis realistis
float bacaSuhuMock() {
  static unsigned long jumlahBaca = 0;
  jumlahBaca++;

  float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);
  float noise = random(-5, 6) * 0.1;
  float suhu = suhuDasar + noise;

  if (jumlahBaca % 25 == 0) {
    suhu += 8.0;
  }
  return suhu;
}

// 4. Fungsi menghitung rata-rata bergerak (Moving Average)
float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru;
  indeks = (indeks + 1) % N;
  if (jumlah < N) jumlah++;

  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }
  return total / jumlah;
}

// === BARIS MODIFIKASI: FUNGSI PERHITUNGAN SIMPANGAN BAKU SAMPEL ===
// 5. Menerima nilai rata-rata (mean) dari buffer aktif dan mengembalikan standar deviasi
float hitungStandarDeviasi(float rataRata) {
  // Minimal membutuhkan 2 sampel untuk menghitung varians sampel (agar pembagi jumlah - 1 tidak nol)
  if (jumlah <= 1) {
    return 0.0;
  }

  // 6. Akumulasi jumlah kuadrat deviasi: sum((x_i - mean)^2)
  float jumlahKuadratSelisih = 0.0;
  for (int i = 0; i < jumlah; i++) {
    float selisih = buffer[i] - rataRata;
    jumlahKuadratSelisih += (selisih * selisih);
  }

  // 7. Hitung varians sampel menggunakan derajat kebebasan (jumlah - 1)
  float varians = jumlahKuadratSelisih / (jumlah - 1);

  // 8. Akar kuadrat dari varians menghasilkan simpangan baku
  return sqrt(varians);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== PENGOLAHAN DATA DENGAN MOVING AVERAGE & STANDAR DEVIASI ===");
}

void loop() {
  float suhu = bacaSuhuMock();

  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    // 9. Hitung nilai filter moving average
    float suhuFilter = hitungMovingAverage(suhu);

    // 10. Hitung standar deviasi terhadap data yang ada di dalam circular buffer
    float stdDev = hitungStandarDeviasi(suhuFilter);

    // 11. Perbarui nilai ekstremum suhu minimum dan maksimum
    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    // 12. Cetak seluruh parameter statistik lokal ke Serial Monitor
    Serial.print("Mentah: ");
    Serial.print(suhu, 2);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter, 2);
    Serial.print(" | StdDev: ");
    Serial.print(stdDev, 2);
    Serial.print(" | Min: ");
    Serial.print(suhuMin, 2);
    Serial.print(" | Max: ");
    Serial.println(suhuMax, 2);
  }

  delay(2000); // Interval sampling setiap 2 detik
}
