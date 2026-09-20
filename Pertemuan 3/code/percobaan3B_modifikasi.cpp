/**
 * Modul 3: Protokol Komunikasi IoT (HTTP dan MQTT dengan Format JSON)
 * Percobaan 3B (Modifikasi): MQTT Publish JSON dengan Parameter Waktu Uptime (millis())
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi Modifikasi:
 * Menambahkan data waktu (timestamp uptime sistem dalam milidetik menggunakan millis())
 * ke dalam struktur payload JSON yang dipublikasikan (publish) ke broker MQTT HiveMQ.
 */

// 1. Deteksi platform arsitektur mikrokontroler
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

// 2. Mengikutsertakan pustaka klien MQTT dan parser JSON
#include <PubSubClient.h>
#include <ArduinoJson.h>

// 3. Kredensial jaringan WiFi lokal
const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

// 4. Parameter koneksi broker MQTT publik
const char* mqttServer = "broker.hivemq.com";
const int   mqttPort   = 1883;

// 5. Topic unik pengiriman data MQTT
const char* mqttTopic  = "apipdivahana";

// 6. Instansiasi objek klien jaringan dan MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// 7. Fungsi untuk mengelola koneksi WiFi
void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

// 8. Fungsi untuk mengelola koneksi dan re-koneksi otomatis ke broker MQTT
void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    // 9. Generate ID unik untuk mencegah pemutusan koneksi sepihak oleh broker publik
    String clientId = "ESPClient-" + String(random(0xffff), HEX);

    // 10. Mencoba membuka session TCP dengan broker
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  // 11. Membuka port komunikasi serial 115200 bps
  Serial.begin(115200);
  delay(100);

  // 12. Menghubungkan ESP ke WiFi
  hubungkanWiFi();

  // 13. Menetapkan alamat dan port broker MQTT
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  // 14. Validasi koneksi broker MQTT, rekoneksi jika terputus
  if (!client.connected()) {
    hubungkanMQTT();
  }
  
  // 15. Memproses paket data internal dan menjaga koneksi keep-alive MQTT
  client.loop();

  // 16. Membentuk objek dokumen JSON
  JsonDocument doc;
  doc["suhu"] = 28.5;
  doc["kelembaban"] = 65.0;

  // === BARIS MODIFIKASI: PENAMBAHAN PARAMETER WAKTU (millis()) ===
  // 17. Menambahkan key "waktu" berisi uptime sistem mikrokontroler (ms)
  doc["waktu"] = millis();

  // 18. Menyiapkan buffer karakter untuk serialisasi data
  char buffer[128];
  
  // 19. Mengubah dokumen JSON menjadi array karakter
  serializeJson(doc, buffer);

  // 20. Mempublikasikan payload JSON ke topic target di broker
  client.publish(mqttTopic, buffer);
  
  // 21. Mencetak log pengiriman ke Serial Monitor
  Serial.print("Data terkirim ke topic ");
  Serial.print(mqttTopic);
  Serial.print(": ");
  Serial.println(buffer);

  // 22. Jeda interval pengiriman data selama 10 detik
  delay(10000);
}
