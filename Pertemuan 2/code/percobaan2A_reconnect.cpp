/**
 * Modul 2: Konfigurasi Jaringan (WiFi)
 * Percobaan 2A (Modifikasi): Auto Reconnect Mode Station (STA)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi Modifikasi:
 * Menambahkan mekanisme reconnect otomatis di dalam void loop().
 * Apabila koneksi WiFi terputus di tengah jalan, perangkat akan
 * secara otomatis memutus sisa koneksi yang menggantung (WiFi.disconnect())
 * dan memulai inisiasi sambungan baru dengan batas percobaan (maksimal 20 loop / 10 detik)
 * agar mikrokontroler tidak macet (freeze/hang) dan Watchdog Timer tidak terpicu.
 */

// 1. Preprocessor direktif untuk mendeteksi arsitektur board secara otomatis
#if defined(ESP32)
  #include <WiFi.h>
  const int ledPin = 2;   // GPIO 2 untuk LED bawaan ESP32
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  const int ledPin = D2;  // Pin D2 untuk LED indikator pada NodeMCU
#endif

// 2. Kredensial jaringan WiFi yang dituju
const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

void setup() {
  // 3. Konfigurasi kecepatan transfer data serial monitor ke 115200 bps
  Serial.begin(115200);
  delay(100);

  // 4. Inisialisasi pin LED indikator sebagai pin OUTPUT
  pinMode(ledPin, OUTPUT);
  // 5. Matikan LED di awal inisiasi
  digitalWrite(ledPin, LOW);

  // 6. Atur mikrokontroler murni menjadi mode Station (STA)
  WiFi.mode(WIFI_STA);

  // 7. Mulai proses penyambungan ke Access Point
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");

  // 8. Menunggu hingga koneksi pertama kali berhasil tersambung
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 9. Cetak informasi status dan konfigurasi jaringan setelah terhubung
  Serial.println("\nWiFi berhasil terhubung!");
  Serial.print("IP Address  : ");
  Serial.println(WiFi.localIP());     // Mengambil IP lokal dari router
  Serial.print("MAC Address : ");
  Serial.println(WiFi.macAddress());  // Mengambil MAC address fisik chip
  Serial.print("RSSI (dBm)  : ");
  Serial.println(WiFi.RSSI());        // Mengambil kekuatan sinyal (dBm)

  // 10. Nyalakan LED sebagai indikator bahwa sambungan berhasil terjalin
  digitalWrite(ledPin, HIGH);
}

void loop() {
  // 11. Periksa status sambungan WiFi secara berkala
  if (WiFi.status() == WL_CONNECTED) {
    // 12. Jika kondisi masih terhubung, tampilkan notifikasi di Serial Monitor
    Serial.println("Status: Terhubung");
  } else {
    // === BLOK MEKANISME AUTO RECONNECT ===
    // 13. Tampilkan peringatan bahwa koneksi ke router terputus
    Serial.println("Koneksi terputus! Mencoba menghubungkan ulang...");
    
    // 14. Matikan LED indikator untuk menandakan sambungan offline
    digitalWrite(ledPin, LOW);

    // 15. Putuskan sisa sesi koneksi lama yang menggantung agar buffer bersih
    WiFi.disconnect();
    
    // 16. Lakukan request sambungan baru ke Access Point dengan kredensial tersimpan
    WiFi.begin(ssid, password);

    // 17. Inisialisasi variabel counter batas percobaan reconnect
    int attempts = 0;
    
    // 18. Loop polling koneksi dengan batas maksimal 20 iterasi (20 x 500ms = 10 detik)
    //     Langkah pembatasan ini krusial agar CPU tidak terjebak dalam infinite loop (hang)
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    // 19. Evaluasi hasil setelah siklus 20 kali percobaan
    if (WiFi.status() == WL_CONNECTED) {
      // 20. Jika berhasil terhubung kembali, nyalakan kembali LED indikator
      Serial.println("\nBerhasil terhubung kembali!");
      digitalWrite(ledPin, HIGH);
    } else {
      // 21. Jika belum berhasil setelah 10 detik, berikan laporan dan lanjut ke siklus loop berikutnya
      Serial.println("\nGagal terhubung. Akan dicoba lagi pada siklus berikutnya.");
    }
  }

  // 22. Jeda interval 5 detik sebelum siklus pengujian berikutnya
  delay(5000);
}
