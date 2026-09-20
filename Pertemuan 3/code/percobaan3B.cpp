/**
 * Modul 3: Protokol Komunikasi IoT (HTTP dan MQTT dengan Format JSON)
 * Percobaan 3B: Komunikasi Data Menggunakan MQTT (Base Code)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi:
 * Mengirimkan data telemetri sensor (suhu dan kelembaban) dalam format JSON
 * ke broker MQTT publik (broker.hivemq.com) pada port 1883 dengan pola publish-subscribe.
 */

// Deteksi otomatis arsitektur board mikrokontroler
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>

// Kredensial jaringan WiFi
const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

// Konfigurasi broker MQTT
const char* mqttServer = "broker.hivemq.com";
const int   mqttPort   = 1883;

// Nama topic MQTT (dibuat unik sesuai praktikum)
const char* mqttTopic  = "apipdivahana"; 

WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi untuk menghubungkan mikrokontroler ke jaringan WiFi
void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

// Fungsi rekoneksi otomatis ke broker MQTT
void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    // Membuat Client ID acak agar tidak bentrok dengan client lain di broker publik
    String clientId = "ESPClient-" + String(random(0xffff), HEX);

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
  Serial.begin(115200);
  delay(100);

  hubungkanWiFi();

  // Mendaftarkan host server broker MQTT dan port
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  // Menjaga koneksi MQTT tetap aktif
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop();

  // Membuat data sensor dalam format JSON
  JsonDocument doc;
  doc["suhu"] = 28.5;
  doc["kelembaban"] = 65.0;

  // Serialisasi data JSON ke dalam buffer char array (sesuai spesifikasi MQTT)
  char buffer[128];
  serializeJson(doc, buffer);

  // Mempublikasikan payload JSON ke topic target
  client.publish(mqttTopic, buffer);
  Serial.print("Data terkirim ke topic ");
  Serial.print(mqttTopic);
  Serial.print(": ");
  Serial.println(buffer);

  // Jeda pengiriman data setiap 10 detik (atau 5 detik sesuai kebutuhan praktikum)
  delay(10000);
}
