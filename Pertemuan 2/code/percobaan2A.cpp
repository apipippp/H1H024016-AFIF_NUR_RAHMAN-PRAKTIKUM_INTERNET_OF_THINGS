/**
 * Modul 2: Konfigurasi Jaringan (WiFi)
 * Percobaan 2A: Konfigurasi Mode Station (STA)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi:
 * Mengkonfigurasi mikrokontroler (ESP32/ESP8266) sebagai Station (STA/klien)
 * untuk terhubung ke jaringan Access Point/Router yang sudah ada, lalu
 * menampilkan parameter jaringan (IP Address, MAC Address, dan RSSI)
 * serta mengontrol LED indikator koneksi.
 */

// Deteksi board otomatis untuk memuat library WiFi yang tepat
#if defined(ESP32)
  #include <WiFi.h>
  const int ledPin = 2;   // Built-in LED pada GPIO 2 (ESP32)
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  const int ledPin = D2;  // Pin D2 / GPIO 4 / GPIO 2 pada NodeMCU ESP8266
#endif

// Kredensial jaringan WiFi (Access Point / Router luar)
const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

void setup() {
  // Inisialisasi komunikasi serial dengan baud rate 115200 bps
  Serial.begin(115200);
  delay(100);

  // Inisialisasi pin LED indikator sebagai OUTPUT dan matikan di awal
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Mengatur mode operasi WiFi menjadi Station murni
  WiFi.mode(WIFI_STA);

  // Memulai proses koneksi (autentikasi) ke jaringan WiFi target
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");

  // Menunggu hingga mikrokontroler berhasil terhubung (status WL_CONNECTED)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Menampilkan informasi jaringan setelah sukses terhubung
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
  Serial.print("IP Address  : ");
  Serial.println(WiFi.localIP());     // Mencetak alamat IP lokal dari router
  Serial.print("MAC Address : ");
  Serial.println(WiFi.macAddress());  // Mencetak alamat fisik hardware (MAC)
  Serial.print("RSSI (dBm)  : ");
  Serial.println(WiFi.RSSI());        // Mencetak kekuatan sinyal nirkabel

  // Menyalakan LED sebagai indikator bahwa perangkat telah terhubung ke jaringan
  digitalWrite(ledPin, HIGH);
}

void loop() {
  // Melakukan validasi berkala status koneksi WiFi setiap 5 detik
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Terhubung");
  } else {
    Serial.println("Status: Terputus");
    digitalWrite(ledPin, LOW); // Matikan LED indikator jika sambungan terputus
  }

  delay(5000); // Jeda pengecekan status
}
