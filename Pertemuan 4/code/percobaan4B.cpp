/**
 * Modul 4: Pertukaran Data Dua Arah (Publish dan Subscribe)
 * Percobaan 4B: Pertukaran Data Dua Arah / Full Duplex (Base Code)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi:
 * Mengintegrasikan akuisisi data sensor DHT11, pengiriman data periodik (publish)
 * secara non-blocking menggunakan millis() setiap 5 detik ke topic data,
 * sekaligus menerima perintah kendali (subscribe) secara bersamaan (full duplex)
 * melalui fungsi callback tanpa saling memblokir (blocking).
 */

// Deteksi platform mikrokontroler
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #define DHTPIN D2
  const int ledPin = D7; // Pin LED pada NodeMCU ESP8266
#elif defined(ESP32)
  #include <WiFi.h>
  #define DHTPIN 4
  const int ledPin = 26; // Pin LED pada ESP32
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Kredensial jaringan WiFi
const char* ssid          = "NAMA_WIFI_ANDA";
const char* password      = "PASSWORD_WIFI_ANDA";

// Parameter broker MQTT
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;

// Topic terpisah untuk data telemetri dan perintah kendali
const char* topicData     = "unsoed/tk245004/kelompokapiphana/data";
const char* topicPerintah = "unsoed/tk245004/kelompokapiphana/perintah";

WiFiClient espClient;
PubSubClient client(espClient);

// Variabel penanda waktu untuk eksekusi non-blocking
unsigned long waktuTerakhirPublish = 0;
const long    intervalPublish      = 5000; // Interval pengiriman data 5000 ms (5 detik)

// Fungsi callback untuk memproses perintah kendali masuk
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) {
    return; // Abaikan bila parsing JSON gagal
  }

  const char* perintah = doc["perintah"];
  // Mengubah status logika pin LED secara instan
  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
  
  Serial.print("Perintah diterima -> Aktuator: ");
  Serial.println(perintah);
}

// Fungsi koneksi WiFi
void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

// Fungsi koneksi MQTT dan pendaftaran subscription
void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESPClient-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(topicPerintah);
      Serial.println("Terhubung dan subscribe topic perintah");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  dht.begin();
  hubungkanWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  // 1. Memastikan koneksi broker MQTT tetap tersambung
  if (!client.connected()) {
    hubungkanMQTT();
  }
  
  // 2. Memproses paket data masuk dan menjaga koneksi keep-alive tanpa jeda
  client.loop();

  // 3. Mekanisme pengiriman data sensor secara NON-BLOCKING berbasis millis()
  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();

    float suhu = dht.readTemperature();

    // Validasi pembacaan sensor
    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = (int)suhu; // Mengemas nilai suhu ke dalam JSON

      char buffer[128];
      serializeJson(doc, buffer);

      // Mempublikasikan payload JSON ke topicData
      client.publish(topicData, buffer);
      Serial.print("Data terkirim: ");
      Serial.println(buffer);
    }
  }
}
