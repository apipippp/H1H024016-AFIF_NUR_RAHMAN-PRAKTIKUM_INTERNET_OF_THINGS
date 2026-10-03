/**
 * Modul 4: Pertukaran Data Dua Arah (Publish dan Subscribe)
 * Percobaan 4A: Subscribe dan Deserialisasi Data JSON untuk Kendali Aktuator (Base Code)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi:
 * Mengkonfigurasi mikrokontroler (ESP8266 / ESP32) sebagai MQTT Subscriber.
 * Menerima payload perintah berbentuk JSON (misal: {"perintah":"ON"} / {"perintah":"OFF"})
 * dari broker MQTT HiveMQ pada topic perintah, mendeserialisasi JSON, dan
 * mengendalikan status aktuator LED secara real-time.
 */

// Deteksi otomatis arsitektur board mikrokontroler
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  const int ledPin = D7; // Pin LED pada NodeMCU ESP8266 (GPIO 13)
#elif defined(ESP32)
  #include <WiFi.h>
  const int ledPin = 26; // Pin LED pada ESP32 (GPIO 26)
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>

// Kredensial jaringan WiFi
const char* ssid          = "NAMA_WIFI_ANDA";
const char* password      = "PASSWORD_WIFI_ANDA";

// Konfigurasi broker MQTT publik
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;

// Topic MQTT untuk menerima perintah kendali aktuator
const char* topicPerintah = "unsoed/tk245004/kelompokapiphana/perintah";

WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi callback yang dipanggil otomatis saat ada pesan baru masuk pada topic yang di-subscribe
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  // Mengonversi byte array payload menjadi String
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  // Deserialisasi data JSON yang diterima
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);

  // Validasi format JSON
  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Mengambil nilai string dari key "perintah"
  const char* perintah = doc["perintah"];

  // Logika kendali aktuator berdasarkan nilai perintah
  if (String(perintah) == "ON") {
    digitalWrite(ledPin, HIGH);
    Serial.println("Aktuator: ON");
  } else if (String(perintah) == "OFF") {
    digitalWrite(ledPin, LOW);
    Serial.println("Aktuator: OFF");
  }
}

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

// Fungsi untuk menghubungkan dan mendaftar subscribe ke broker MQTT
void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESPClient-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      // Mendaftarkan subscribe ke topicPerintah segera setelah berhasil terhubung
      client.subscribe(topicPerintah);
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
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
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Pastikan LED mati di awal

  hubungkanWiFi();

  // Konfigurasi broker dan daftarkan fungsi callback
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  // Rekoneksi otomatis jika koneksi broker terputus
  if (!client.connected()) {
    hubungkanMQTT();
  }
  // Menjaga koneksi aktif dan memproses pesan masuk seketika
  client.loop();
}
