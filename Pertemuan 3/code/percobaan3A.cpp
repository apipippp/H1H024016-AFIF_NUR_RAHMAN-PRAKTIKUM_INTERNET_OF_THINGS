/**
 * Modul 3: Protokol Komunikasi IoT (HTTP dan MQTT dengan Format JSON)
 * Percobaan 3A: Komunikasi Data Menggunakan HTTP POST (Base Code)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi:
 * Mengirimkan data telemetri sensor buatan (suhu dan kelembaban) dalam format JSON
 * ke server pengujian publik (httpbin.org/post) menggunakan protokol HTTP metode POST.
 */

// Deteksi otomatis arsitektur board mikrokontroler
#if defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif

#include <ArduinoJson.h>

// Kredensial jaringan WiFi
const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

// URL server endpoint uji HTTP POST
const char* serverUrl = "http://httpbin.org/post";

void setup() {
  // Inisialisasi komunikasi Serial pada baud rate 115200
  Serial.begin(115200);
  delay(100);

  // Mulai koneksi WiFi Station
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");

  // Menunggu hingga ESP berhasil tersambung ke jaringan
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi berhasil terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Periksa apakah koneksi WiFi masih aktif
  if (WiFi.status() == WL_CONNECTED) {
    #if defined(ESP32)
      HTTPClient http;
      http.begin(serverUrl);
    #elif defined(ESP8266)
      WiFiClient client;
      HTTPClient http;
      http.begin(client, serverUrl);
    #endif

    // Menambahkan HTTP header untuk memberitahu server bahwa payload berformat JSON
    http.addHeader("Content-Type", "application/json");

    // Membuat objek dokumen data sensor menggunakan ArduinoJson
    JsonDocument doc;
    doc["suhu"] = 28.5;         // Data suhu dummy (°C)
    doc["kelembaban"] = 65.0;   // Data kelembaban dummy (%)

    // Serialisasi objek JSON menjadi string teks
    String requestBody;
    serializeJson(doc, requestBody);

    Serial.print("Mengirim data: ");
    Serial.println(requestBody);

    // Mengirimkan data payload melalui HTTP POST
    int httpResponseCode = http.POST(requestBody);

    // Evaluasi respon dari server
    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);
      Serial.println("Isi Response:");
      Serial.println(http.getString());
    } else {
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }

    // Menutup koneksi HTTP dan membebaskan alokasi memori
    http.end();
  } else {
    Serial.println("WiFi terputus, tidak dapat mengirim data.");
  }

  // Jeda pengiriman data setiap 10 detik
  delay(10000);
}
