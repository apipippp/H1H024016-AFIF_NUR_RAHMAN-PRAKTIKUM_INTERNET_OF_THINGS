/**
 * Modul 2: Konfigurasi Jaringan (WiFi)
 * Percobaan 2B (Modifikasi): Dual Mode Access Point + Station (WIFI_AP_STA)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi Modifikasi:
 * Mengaktifkan mode ganda (WIFI_AP_STA) di mana mikrokontroler bertindak
 * secara simultan sebagai:
 * 1. Station (STA) yang terhubung ke router WiFi luar untuk akses internet/LAN.
 * 2. Access Point (AP) yang memancarkan sinyal hotspot mandiri agar perangkat
 *    lain (smartphone/laptop) dapat terhubung langsung ke mikrokontroler.
 */

// 1. Deteksi platform mikrokontroler secara otomatis
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

// 2. Kredensial untuk terhubung ke Router Luar (Mode Station)
const char* sta_ssid     = "WIFI_RUMAH";
const char* sta_password = "PASSWORD_WIFI";

// 3. Kredensial untuk memancarkan Hotspot Lokal (Mode Access Point)
const char* ap_ssid      = "ESP32_Hybrid_AP";
const char* ap_password  = "securepassword"; // Minimal 8 karakter

void setup() {
  // 4. Inisialisasi komunikasi serial dengan baud rate 115200
  Serial.begin(115200);
  delay(100);

  // 5. Mengaktifkan mode operasi WiFi ganda (AP + STA secara bersamaan)
  WiFi.mode(WIFI_AP_STA);

  // ==========================================
  // 6. KONFIGURASI 1: MODE STATION (Klien Router)
  // ==========================================
  // Memulai proses koneksi ke router utama
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Menghubungkan ke Router utama");

  // Loop polling menunggu sambungan ke router berhasil
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Tampilkan notifikasi keberhasilan dan cetak IP lokal yang diberikan oleh DHCP router
  Serial.println("\nBerhasil tersambung ke Router!");
  Serial.print("IP Station (Lokal)       : ");
  Serial.println(WiFi.localIP());

  // ==========================================
  // 7. KONFIGURASI 2: MODE ACCESS POINT (Hotspot)
  // ==========================================
  // Memancarkan sinyal hotspot mandiri dari chip mikrokontroler
  WiFi.softAP(ap_ssid, ap_password);

  // Tampilkan status aktifasi hotspot beserta SSID dan IP gateway lokal ESP
  Serial.println("\nAccess Point Hybrid juga diaktifkan!");
  Serial.print("SSID AP                  : ");
  Serial.println(ap_ssid);
  Serial.print("IP Access Point (Gateway): ");
  Serial.println(WiFi.softAPIP()); // Default: 192.168.4.1
}

void loop() {
  // 8. Memonitor jumlah client yang sedang menumpang ke sinyal AP ESP
  int jumlahClient = WiFi.softAPgetStationNum();

  Serial.print("Jumlah perangkat terhubung ke AP: ");
  Serial.println(jumlahClient);

  // 9. Mengecek kestabilan koneksi mode Station ke router
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Koneksi Station (Router)       : Stabil");
  } else {
    Serial.println("Koneksi Station (Router)       : Terputus!");
  }

  // 10. Jeda 5 detik sebelum pemantauan berkala berikutnya
  delay(5000);
}
