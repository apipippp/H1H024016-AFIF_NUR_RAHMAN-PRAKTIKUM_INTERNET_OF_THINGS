/**
 * Modul 3: Protokol Komunikasi IoT (HTTP dan MQTT dengan Format JSON)
 * Percobaan 3A (Modifikasi): HTTP POST JSON dengan Parameter Waktu Uptime (millis())
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi Modifikasi:
 * Menambahkan data waktu (timestamp berupa uptime sistem dalam milidetik menggunakan millis())
 * ke dalam struktur payload JSON yang dikirimkan melalui HTTP POST request.
 */

// 1. Deteksi platform mikrokontroler untuk menyertakan pustaka yang tepat
#if defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif

// 2. Mengikutsertakan pustaka manipulasi format data JSON
#include <ArduinoJson.h>

// 3. Mendefinisikan kredensial jaringan WiFi (SSID dan Password)
const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

// 4. URL server tujuan (endpoint HTTP POST publik)
const char* serverUrl = "http://httpbin.org/post";

void setup() {
  // 5. Menginisialisasi komunikasi Serial Monitor dengan kecepatan 115200 bps
  Serial.begin(115200);
  delay(100);

  // 6. Memulai proses koneksi mikrokontroler ke Access Point / WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");

  // 7. Polling perulangan menahan program sampai koneksi WiFi berhasil terhubung
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 8. Menampilkan informasi alamat IP lokal setelah terkoneksi
  Serial.println("\nWiFi berhasil terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // 9. Memvalidasi kelangsungan sambungan WiFi sebelum mencoba transmisi HTTP
  if (WiFi.status() == WL_CONNECTED) {
    // 10. Menginisialisasi objek klien HTTP berdasarkan platform mikrokontroler
    #if defined(ESP32)
      HTTPClient http;
      http.begin(serverUrl);
    #elif defined(ESP8266)
      WiFiClient client;
      HTTPClient http;
      http.begin(client, serverUrl);
    #endif

    // 11. Menetapkan HTTP Header 'Content-Type' menjadi 'application/json'
    http.addHeader("Content-Type", "application/json");

    // 12. Membuat objek dokumen JSON menggunakan pustaka ArduinoJson
    JsonDocument doc;
    
    // 13. Memasukkan data telemetri suhu ke dalam objek JSON
    doc["suhu"] = 28.5;
    
    // 14. Memasukkan data telemetri kelembaban ke dalam objek JSON
    doc["kelembaban"] = 65.0;

    // === BARIS MODIFIKASI: PENAMBAHAN PARAMETER WAKTU UPTIME ===
    // 15. Mengambil waktu berjalan mikrokontroler sejak booting (dalam ms) dan menyimpannya pada key "waktu"
    doc["waktu"] = millis();

    // 16. Menyiapkan variabel penampung string untuk hasil serialisasi JSON
    String requestBody;
    
    // 17. Mengonversi objek JSON menjadi format string teks (serialized JSON)
    serializeJson(doc, requestBody);

    // 18. Mencetak payload data yang akan dikirim ke Serial Monitor
    Serial.print("Mengirim data modifikasi: ");
    Serial.println(requestBody);

    // 19. Mengirimkan HTTP request bertipe POST beserta isi payload JSON ke server
    int httpResponseCode = http.POST(requestBody);

    // 20. Mengecek apakah transmisi HTTP berhasil (kode respon positif)
    if (httpResponseCode > 0) {
      // 21. Mencetak kode status respon HTTP (misal: 200 OK)
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);
      
      // 22. Mencetak isi respon (response body) yang dikembalikan oleh server
      Serial.println("Isi Response:");
      Serial.println(http.getString());
    } else {
      // 23. Mencetak pesan dan kode error jika transmisi HTTP gagal
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }

    // 24. Menutup koneksi HTTP untuk melepaskan resource soket TCP dan memori RAM
    http.end();
  } else {
    // 25. Peringatan jika status koneksi WiFi terputus
    Serial.println("WiFi terputus, transmisi dibatalkan.");
  }

  // 26. Jeda waktu 10 detik sebelum siklus transmisi data berikutnya
  delay(10000);
}
