/**
 * Modul 4: Pertukaran Data Dua Arah (Publish dan Subscribe)
 * Percobaan 4A (Modifikasi): Subscribe & Deserialisasi JSON dengan Pengaturan Kecerahan LED via PWM
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi Modifikasi:
 * Menambahkan pembacaan parameter "intensitas" dari payload JSON (misal: {"perintah":"ON", "intensitas":200})
 * untuk mengatur tingkat kecerahan LED menggunakan sinyal Pulse Width Modulation (PWM).
 * Pada ESP8266 menggunakan analogWrite() rentang 0-1023, sedangkan pada ESP32 menggunakan ledcWrite() rentang 0-255.
 */

// 1. Deteksi arsitektur mikrokontroler otomatis
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  const int ledPin = D7; // Pin LED pada NodeMCU ESP8266 (GPIO 13)
#elif defined(ESP32)
  #include <WiFi.h>
  const int ledPin = 26; // Pin LED pada ESP32 (GPIO 26)
  // Konfigurasi kanal PWM untuk ESP32
  const int pwmChannel = 0;
  const int pwmFreq    = 5000;
  const int pwmRes     = 8; // Resolusi 8-bit (nilai 0 - 255)
#endif

// 2. Library pendukung protokol MQTT dan format data JSON
#include <PubSubClient.h>
#include <ArduinoJson.h>

// 3. Kredensial jaringan WiFi
const char* ssid          = "NAMA_WIFI_ANDA";
const char* password      = "PASSWORD_WIFI_ANDA";

// 4. Konfigurasi broker MQTT HiveMQ
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;

// 5. Topic perintah kendali
const char* topicPerintah = "unsoed/tk245004/kelompokapiphana/perintah";

WiFiClient espClient;
PubSubClient client(espClient);

// 6. Fungsi callback untuk menangani pesan masuk
void callback(char* topic, byte* payload, unsigned int length) {
  // 7. Merangkai byte array payload menjadi String
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  // 8. Deserialisasi teks payload JSON menjadi objek dokumen
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);

  // 9. Validasi error parsing
  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // 10. Ekstraksi nilai perintah kendali
  const char* perintah = doc["perintah"];

  // === BARIS MODIFIKASI: PENGAMBILAN PARAMETER INTENSITAS PWM ===
  // 11. Mengambil nilai intensitas dengan nilai default 0 jika key tidak ditemukan
  int intensitas = doc["intensitas"] | 0;

  // 12. Evaluasi perintah "ON" beserta penerapan sinyal PWM
  if (String(perintah) == "ON") {
    #if defined(ESP8266)
      // Pada ESP8266 rentang PWM default 0-1023; jika intensitas = 0, defaultkan ke full duty cycle (1023)
      analogWrite(ledPin, intensitas > 0 ? intensitas : 1023);
    #elif defined(ESP32)
      // Pada ESP32 menggunakan ledcWrite rentang 0-255
      ledcWrite(pwmChannel, intensitas > 0 ? intensitas : 255);
    #endif
    Serial.print("Aktuator: ON dengan intensitas PWM: ");
    Serial.println(intensitas);
  } 
  // 13. Evaluasi perintah "OFF" untuk mematikan LED (duty cycle 0)
  else if (String(perintah) == "OFF") {
    #if defined(ESP8266)
      analogWrite(ledPin, 0);
    #elif defined(ESP32)
      ledcWrite(pwmChannel, 0);
    #endif
    Serial.println("Aktuator: OFF (Intensitas 0)");
  }
}

// 14. Fungsi koneksi ke WiFi
void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

// 15. Fungsi koneksi dan auto-resubscribe ke broker MQTT
void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESPClient-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
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

  #if defined(ESP8266)
    analogWrite(ledPin, 0); // Matikan PWM di awal
  #elif defined(ESP32)
    // Inisialisasi kanal PWM pada ESP32
    ledcSetup(pwmChannel, pwmFreq, pwmRes);
    ledcAttachPin(ledPin, pwmChannel);
    ledcWrite(pwmChannel, 0);
  #endif

  hubungkanWiFi();

  // Daftarkan server broker dan fungsi callback
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop();
}
