/**
 * Modul 2: Konfigurasi Jaringan (WiFi)
 * Percobaan 2B: Konfigurasi Mode Access Point (AP)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi:
 * Mengkonfigurasi mikrokontroler (ESP32/ESP8266) sebagai Access Point mandiri (SoftAP).
 * Perangkat akan memancarkan sinyal hotspot sendiri tanpa memerlukan router eksternal,
 * mengalokasikan IP default (192.168.4.1), dan memonitor jumlah client yang tersambung.
 */

// Deteksi board otomatis untuk memuat library WiFi yang tepat
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

// Parameter konfigurasi Access Point
const char* ap_ssid     = "apip diva hana";  // Nama hotspot yang dipancarkan
const char* ap_password = "12345678";        // Kata sandi (minimal 8 karakter untuk WPA2-PSK)

void setup() {
  // Inisialisasi komunikasi serial pada kecepatan 115200 bps
  Serial.begin(115200);
  delay(100);

  // Set mode operasi WiFi mikrokontroler menjadi Access Point (AP)
  WiFi.mode(WIFI_AP);

  // Mengaktifkan Access Point mandiri dengan SSID dan password yang ditentukan
  WiFi.softAP(ap_ssid, ap_password);

  // Mendapatkan alamat IP gateway Access Point yang dibuat
  IPAddress apIP = WiFi.softAPIP();

  // Menampilkan informasi Access Point ke Serial Monitor
  Serial.println("Access Point aktif!");
  Serial.print("SSID       : ");
  Serial.println(ap_ssid);
  Serial.print("IP Address : ");
  Serial.println(apIP); // Default pabrik: 192.168.4.1
}

void loop() {
  // Membaca dan menampilkan jumlah perangkat (client) yang sedang terhubung ke SoftAP
  int jumlahClient = WiFi.softAPgetStationNum();
  
  Serial.print("Jumlah perangkat terhubung: ");
  Serial.println(jumlahClient);

  // Jeda waktu pemantauan setiap 5 detik
  delay(5000);
}
