/**
 * Modul 4: Pertukaran Data Dua Arah (Publish dan Subscribe)
 * Percobaan 4B (Modifikasi): Dua Topic Perintah Terpisah untuk Mengendalikan Dua Aktuator (LED & Buzzer)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi Modifikasi:
 * Menambahkan topic perintah baru sehingga mikrokontroler dapat membedakan perintah
 * untuk mengendalikan dua aktuator yang berbeda:
 * 1. Aktuator 1: LED pada topic "unsoed/tk245004/kelompokapiphana/perintah/led"
 * 2. Aktuator 2: Buzzer pada topic "unsoed/tk245004/kelompokapiphana/perintah/buzzer"
 * Pemisahan topic dilakukan pada fungsi callback() menggunakan fungsi strcmp().
 */

// 1. Deteksi arsitektur board mikrokontroler
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #define DHTPIN D2
  const int ledPin    = D7; // Pin LED pada NodeMCU ESP8266
  const int buzzerPin = D5; // Pin Buzzer pada NodeMCU ESP8266
#elif defined(ESP32)
  #include <WiFi.h>
  #define DHTPIN 4
  const int ledPin    = 26; // Pin LED pada ESP32
  const int buzzerPin = 18; // Pin Buzzer pada ESP32
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// 2. Kredensial WiFi
const char* ssid          = "NAMA_WIFI_ANDA";
const char* password      = "PASSWORD_WIFI_ANDA";

// 3. Konfigurasi broker MQTT
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;

// 4. Topic pengiriman data telemetri sensor
const char* topicData     = "unsoed/tk245004/kelompokapiphana/data";

// === BARIS MODIFIKASI: DUA TOPIC PERINTAH TERPISAH ===
// 5. Topic khusus untuk kendali aktuator LED
const char* topicLED      = "unsoed/tk245004/kelompokapiphana/perintah/led";
// 6. Topic khusus untuk kendali aktuator Buzzer
const char* topicBuzzer   = "unsoed/tk245004/kelompokapiphana/perintah/buzzer";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long    intervalPublish      = 5000; // 5000 ms (5 detik)

// 7. Modifikasi fungsi callback dengan percabangan topic
void callback(char* topic, byte* payload, unsigned int length) {
  // 8. Rangkai byte payload menjadi string
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  // 9. Deserialisasi JSON
  JsonDocument doc;
  if (deserializeJson(doc, pesan)) {
    return; // Abaikan bila parsing gagal
  }

  // 10. Ekstraksi nilai perintah
  const char* perintah = doc["perintah"];

  // === BARIS MODIFIKASI: PEMILAHAN TOPIC DENGAN strcmp() ===
  // 11. Jika topic yang masuk adalah topicLED
  if (strcmp(topic, topicLED) == 0) {
    digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
    Serial.print("Perintah dieksekusi pada LED -> ");
    Serial.println(perintah);
  }
  // 12. Jika topic yang masuk adalah topicBuzzer
  else if (strcmp(topic, topicBuzzer) == 0) {
    digitalWrite(buzzerPin, String(perintah) == "ON" ? HIGH : LOW);
    Serial.print("Perintah dieksekusi pada Buzzer -> ");
    Serial.println(perintah);
  }
}

// 13. Fungsi koneksi WiFi
void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

// 14. Modifikasi fungsi hubungkanMQTT untuk subscribe ke dua topic sekaligus
void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESPClient-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      // === BARIS MODIFIKASI: SUBSCRIBE KE KEDUA TOPIC PERINTAH ===
      client.subscribe(topicLED);
      client.subscribe(topicBuzzer);
      Serial.println("Terhubung dan subscribe ke topic LED & Buzzer!");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // 15. Inisialisasi pin aktuator sebagai OUTPUT
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  dht.begin();
  hubungkanWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  // 16. Memastikan koneksi broker tetap aktif
  if (!client.connected()) {
    hubungkanMQTT();
  }
  // 17. Menjaga fungsi loop MQTT agar pesan masuk langsung diproses
  client.loop();

  // 18. Mekanisme non-blocking publish berbasis perbandingan waktu millis()
  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();

    float suhu = dht.readTemperature();
    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = (int)suhu;

      char buffer[128];
      serializeJson(doc, buffer);

      client.publish(topicData, buffer);
      Serial.print("Data telemetri terkirim: ");
      Serial.println(buffer);
    }
  }
}
