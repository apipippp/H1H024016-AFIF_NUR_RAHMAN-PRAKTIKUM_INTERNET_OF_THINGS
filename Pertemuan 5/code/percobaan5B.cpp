/**
 * Modul 5: Pemrosesan Data Sensor dan Analisis Statistik
 * Percobaan 5B: Pengirim Data Sensor ke Broker MQTT (ESP8266 / ESP32)
 * Praktikan: Afif Nur Rahman (H1H024016)
 * Asisten: Athallah Tsany Satriyaji
 * 
 * Deskripsi:
 * Mengirimkan data streaming suhu dalam format JSON ke broker MQTT publik (broker.hivemq.com)
 * pada topic "unsoed/tk245004/kelompokApipHana/data" setiap 5 detik.
 * Data ini kemudian akan diakuisisi, diarsip ke CSV, dan dianalisis statistik oleh script Python.
 */

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>

// Kredensial WiFi lokal
const char* ssid        = "..";
const char* password    = "pipnurrr";

// Konfigurasi Broker MQTT publik HiveMQ
const char* mqtt_server = "broker.hivemq.com";
const int   mqtt_port   = 1883;

// Topic MQTT khusus (harus sama persis dengan topic pada script Python)
const char* topic_data  = "unsoed/tk245004/kelompokApipHana/data";

WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi simulasi penghasil data suhu (pengganti sensor fisik DHT yang bermasalah)
float bacaSuhuMock() {
  static unsigned long jumlahBaca = 0;
  jumlahBaca++;

  float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);
  float noise = random(-5, 6) * 0.1;
  float suhu = suhuDasar + noise;

  // Injeksi lonjakan anomali buatan setiap 25 pembacaan (+8.0 °C)
  if (jumlahBaca % 25 == 0) {
    suhu += 8.0;
  }
  return suhu;
}

// Prosedur koneksi jaringan WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Prosedur koneksi dan rekoneksi otomatis ke broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP-Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Membaca nilai suhu
  float suhu = bacaSuhuMock();
  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal");
    return;
  }

  // Mengemas data ke dalam format JSON
  JsonDocument doc;
  doc["suhu"] = suhu;

  char jsonBuffer[128];
  serializeJson(doc, jsonBuffer);

  // Mempublikasikan data ke broker MQTT
  Serial.print("Publish message: ");
  Serial.println(jsonBuffer);
  client.publish(topic_data, jsonBuffer);

  // Interval pengiriman data setiap 5 detik
  delay(5000);
}
