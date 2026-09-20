# Laporan Praktikum Internet of Things — Modul 3: Protokol Komunikasi IoT
**HTTP dan MQTT Dengan Format JSON pada ESP32 / ESP8266**

---

### Informasi Praktikan
* **Nama** : Afif Nur Rahman
* **NIM** : H1H024016
* **Mata Kuliah** : Praktikum Internet of Things (TK245005)
* **Tahun / Semester** : 2026 / 5 (Genap)
* **Modul** : Modul 3 — Protokol Komunikasi IoT (HTTP dan MQTT dengan Format JSON)
* **Nama Asisten / NIM** : Athallah Tsany Satriyaji
* **Program Studi** : S1 Teknik Komputer
* **Fakultas / Universitas** : Fakultas Teknik, Universitas Jenderal Soedirman

---

## Daftar Isi
1. [Detail dan Ringkasan Percobaan](#1-detail-dan-ringkasan-percobaan)
   * [1.1 Tujuan Praktikum](#11-tujuan-praktikum)
   * [1.2 Dasar Teori](#12-dasar-teori)
   * [1.3 Alat dan Bahan](#13-alat-dan-bahan)
2. [Library dan Dependencies yang Diperlukan](#2-library-dan-dependencies-yang-diperlukan)
3. [Percobaan 3A: Komunikasi Data Menggunakan HTTP](#3-percobaan-3a-komunikasi-data-menggunakan-http)
   * [3.1 Penjelasan Kode Base Percobaan 3A](#31-penjelasan-kode-base-percobaan-3a)
   * [3.2 Jawaban Pertanyaan Praktikum 3A](#32-jawaban-pertanyaan-praktikum-3a)
   * [3.3 Modifikasi Kode Percobaan 3A (Menambahkan Parameter `millis()`)](#33-modifikasi-kode-percobaan-3a-menambahkan-parameter-millis)
   * [3.4 Penjelasan Baris demi Baris Kode Modifikasi 3A](#34-penjelasan-baris-demi-baris-kode-modifikasi-3a)
   * [3.5 Penjelasan Fungsi yang Digunakan pada Percobaan 3A](#35-penjelasan-fungsi-yang-digunakan-pada-percobaan-3a)
   * [3.6 Penjelasan Percabangan / Conditional Logic pada Percobaan 3A](#36-penjelasan-percabangan--conditional-logic-pada-percobaan-3a)
4. [Percobaan 3B: Komunikasi Data Menggunakan MQTT](#4-percobaan-3b-komunikasi-data-menggunakan-mqtt)
   * [4.1 Penjelasan Kode Base Percobaan 3B](#41-penjelasan-kode-base-percobaan-3b)
   * [4.2 Jawaban Pertanyaan Praktikum 3B](#42-jawaban-pertanyaan-praktikum-3b)
   * [4.3 Modifikasi Kode Percobaan 3B (Menambahkan Parameter `millis()`)](#43-modifikasi-kode-percobaan-3b-menambahkan-parameter-millis)
   * [4.4 Penjelasan Baris demi Baris Kode Modifikasi 3B](#44-penjelasan-baris-demi-baris-kode-modifikasi-3b)
   * [4.5 Penjelasan Fungsi yang Digunakan pada Percobaan 3B](#45-penjelasan-fungsi-yang-digunakan-pada-percobaan-3b)
   * [4.6 Penjelasan Percabangan / Conditional Logic pada Percobaan 3B](#46-penjelasan-percabangan--conditional-logic-pada-percobaan-3b)
5. [Jawaban Pertanyaan Analisis (Bagian 3.7 Modul)](#5-jawaban-pertanyaan-analisis-bagian-37-modul)
6. [Catatan Kendala dan Solusi](#6-catatan-kendala-dan-solusi)
7. [Dokumentasi Praktikum](#7-dokumentasi-praktikum)

---

## 1. Detail dan Ringkasan Percobaan

### 1.1 Tujuan Praktikum
Berdasarkan modul praktikum dan logbook, tujuan pembelajaran Modul 3 adalah:
1. Memahami konsep dasar protokol komunikasi pada sistem *Internet of Things* (IoT).
2. Memahami karakteristik, arsitektur, dan perbedaan mendasar antara protokol **HTTP** dan **MQTT** dalam konteks IoT.
3. Mengimplementasikan pengiriman data telemetri dari mikrokontroler (ESP32/ESP8266) ke server web menggunakan protokol **HTTP** dengan metode **POST**.
4. Mengimplementasikan pertukaran data dari mikrokontroler ke *broker* **MQTT** menggunakan pola arsitektur **publish-subscribe** dengan format data **JSON**.
5. Mampu menganalisis efisiensi, kelebihan, dan kekurangan masing-masing protokol untuk berbagai skenario aplikasi rekayasa IoT.

### 1.2 Dasar Teori
Protokol komunikasi adalah sekumpulan aturan terstandarisasi yang mengatur transmisi dan pertukaran data antar perangkat di dalam jaringan komputer. Pada ekosistem IoT, pemilihan protokol transmisi sangat krusial karena berdampak langsung pada latensi transfer, efisiensi konsumsi daya mikrokontroler, dan keandalan data pada jaringan dengan keterbatasan *bandwidth*.

```mermaid
graph TD
    subgraph Pola Komunikasi Protokol IoT
        subgraph HTTP Request-Response
            ClientHTTP["ESP32 / ESP8266 (Client)"] -->|1. HTTP POST Request + Header + JSON Body| ServerHTTP["Web Server (httpbin.org)"]
            ServerHTTP -->|2. HTTP 200 OK + Response Body| ClientHTTP
            ClientHTTP -.->|Koneksi Ditutup per Request (Stateless)| ClientHTTP
        end

        subgraph MQTT Publish-Subscribe
            PubESP["ESP32 / ESP8266 (Publisher)"] -->|Publish JSON Payload| BrokerMQTT["MQTT Broker (HiveMQ)"]
            BrokerMQTT -->|Distribusi berdasarkan Topic| SubClient["Aplikasi / Dashboard (Subscriber)"]
            PubESP <-->|Koneksi Persistent Tetap Terjaga (Keep-Alive)| BrokerMQTT
        end
    end
```

* **Protokol HTTP (Hypertext Transfer Protocol)**:
  * Menggunakan pola komunikasi *Request-Response* (klien aktif meminta, server pasif merespons).
  * Bersifat *stateless*, di mana setiap permintaan berdiri sendiri tanpa menyimpan status permintaan sebelumnya.
  * Menggunakan metode standar RESTful seperti `GET` (mengambil data) dan `POST` (mengirimkan data).
  * Memiliki beban *overhead header* yang relatif besar (berkisar antara 200 hingga 800 bytes per request) sehingga kurang efisien untuk transmisi berfrekuensi tinggi.
* **Protokol MQTT (Message Queuing Telemetry Transport)**:
  * Menggunakan pola komunikasi *Publish-Subscribe* yang dimediasi oleh sebuah *Broker*.
  * Dirancang khusus untuk perangkat terbatas (*constrained devices*) dan jaringan dengan keterbatasan *bandwidth*.
  * Menggunakan *header* tetap yang sangat ramping (hanya 2 bytes), menghemat konsumsi daya dan kuota data.
  * Menggunakan koneksi persisten (*persistent TCP connection*) yang dipelihara menggunakan mekanisme *Keep-Alive ping*.
* **Format Pertukaran Data JSON (JavaScript Object Notation)**:
  * Format pertukaran data berbasis teks standar terbuka yang terstruktur dalam pasangan kunci-nilai (*key-value pairs*).
  * Memungkinkan interoperabilitas data yang tinggi antara mikrokontroler berbasis bahasa C/C++ dengan server web berbasis Node.js, Python, PHP, maupun *cloud dashboard*.

### 1.3 Alat dan Bahan
* **Board Mikrokontroler**: NodeMCU ESP8266 (ESP-12E Module) / ESP32 DevKit V1 (1 buah).
* **Kabel Data USB**: Kabel Micro-USB untuk catu daya dan komunikasi serial UART.
* **Komputer / Laptop**: Terpasang Arduino IDE beserta library `ArduinoJson` dan `PubSubClient`.
* **Jaringan WiFi**: Hotspot WiFi 2.4 GHz yang terhubung langsung ke jaringan internet publik.
* **Aplikasi Penguji MQTT**: *MQTT Explorer* atau *HiveMQ WebSocket Client*.
* **Server Endpoint Publik**:
  * HTTP Endpoint: `http://httpbin.org/post`
  * MQTT Broker: `broker.hivemq.com` (Port 1883)

---

## 2. Library dan Dependencies yang Diperlukan

Untuk mengompilasi kode program Modul 3, diperlukan beberapa pustaka yang dapat diinstal melalui *Library Manager* Arduino IDE:

| Nama Pustaka | Pembuat / Author | Versi Rekomendasi | Fungsi Utama |
|---|---|---|---|
| **ArduinoJson** | Benoit Blanchon | Versi 7.x (atau 6.x) | Membuat, memanipulasi, dan menserialisasi objek data format JSON. |
| **PubSubClient** | Nick O'Leary | Versi 2.8+ | Mengelola koneksi TCP, protokol MQTT, fungsi *publish*, dan *subscribe*. |
| **HTTPClient / ESP8266HTTPClient** | Espressif / ESP8266 Core | Built-in Core | Mengirimkan HTTP request (POST/GET) dan membaca respon dari server web. |
| **WiFi / ESP8266WiFi** | Espressif / ESP8266 Core | Built-in Core | Mengelola koneksi nirkabel ke Access Point WiFi 2.4 GHz. |

### Penanganan Kompatibilitas Multi-Board (ESP32 & ESP8266)
Pada kode program diterapkan direktif *preprocessor* kondisional agar kode berjalan secara *cross-platform*:

```cpp
#if defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif
#include <ArduinoJson.h>
#include <PubSubClient.h>
```

---

## 3. Percobaan 3A: Komunikasi Data Menggunakan HTTP

### 3.1 Penjelasan Kode Base Percobaan 3A
File implementasi: [`code/percobaan3A.cpp`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%203/code/percobaan3A.cpp)

Pada percobaan ini, mikrokontroler bertindak sebagai HTTP Client:
1. Terhubung ke jaringan internet melalui router WiFi lokal.
2. Setiap 10 detik, membentuk dokumen JSON berisi data `suhu = 28.5` dan `kelembaban = 65.0`.
3. Mengonversi data JSON ke bentuk teks (*serializeJson*) lalu mengirimkannya menggunakan metode `http.POST()` ke endpoint `http://httpbin.org/post`.
4. Menerima dan mencetak status response code (HTTP 200) serta response body server yang memantulkan (*echoing*) kembali payload yang dikirim.

---

### 3.2 Jawaban Pertanyaan Praktikum 3A

#### 1. Gambarkan diagram alur (flowchart) proses pengiriman data melalui HTTP POST pada program di atas!

Diagram alur proses transmisi HTTP POST digambarkan sebagai berikut:

```mermaid
flowchart TD
    Start([Mulai]) --> InitSerial[Inisialisasi Serial Baudrate 115200]
    InitSerial --> ConnectWiFi[Koneksi ke Jaringan WiFi: WiFi.begin]
    ConnectWiFi --> CheckWiFiConn{WiFi Terhubung?<br/>WiFi.status == WL_CONNECTED}
    CheckWiFiConn -- Belum --> WaitWiFi[Delay 500ms & Cetak '.']
    WaitWiFi --> CheckWiFiConn
    CheckWiFiConn -- Ya --> PrintWiFiOk[Cetak 'WiFi berhasil terhubung!' & IP Address]
    PrintWiFiOk --> LoopStart{Periksa Status WiFi di loop}
    LoopStart -- Terhubung --> InitHTTP[Inisialisasi HTTPClient & Target URL Server]
    LoopStart -- Terputus --> LogFailWiFi[Cetak 'WiFi Terputus, Transmisi Gagal']
    InitHTTP --> AddHeader[Tambahkan Header:<br/>Content-Type: application/json]
    AddHeader --> CreateJSON[Buat Objek JSON:<br/>doc['suhu']=28.5, doc['kelembaban']=65.0]
    CreateJSON --> SerializeJSON[Serialisasi Objek JSON ke String requestBody]
    SerializeJSON --> ExecPOST[Kirim HTTP POST: http.POST requestBody]
    ExecPOST --> CheckResp{Response Code > 0?}
    CheckResp -- Ya --> PrintSuccess[Cetak HTTP Response Code misal: 200<br/>Cetak Isi Response Body http.getString]
    CheckResp -- Gagal / Error --> PrintError[Cetak Kode Error Transmisi]
    PrintSuccess --> EndHTTP[Tutup Koneksi HTTP: http.end]
    PrintError --> EndHTTP
    LogFailWiFi --> DelayLoop[Delay 10000ms]
    EndHTTP --> DelayLoop
    DelayLoop --> LoopStart
```

> Diagram alir dari catatan praktikum dapat dilihat pada file: [`dokumentasi/flowchart_http_post.png`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%203/dokumentasi/flowchart_http_post.png).

---

#### 2. Apa fungsi dari perintah `http.addHeader("Content-Type", "application/json")` pada program tersebut?
Perintah `http.addHeader("Content-Type", "application/json")` berfungsi untuk menyisipkan informasi metadata pada *HTTP Request Header* yang memberitahu server tujuan bahwa format data yang dikirimkan pada *request body* (*payload*) adalah bertipe MIME **JSON (*JavaScript Object Notation*)**.

**Dampak jika header ini tidak ada:**
Tanpa header tersebut, server web penerima akan memperlakukan payload sebagai teks mentah biasa (*plain text*) atau mengasumsikannya sebagai form HTML bertipe `application/x-www-form-urlencoded`. Akibatnya, modul *JSON parser* di sisi backend server tidak akan mem-parsing body tersebut, sehingga parameter sensor tidak dapat diproses atau bahkan request ditolak dengan kode status HTTP `415 Unsupported Media Type` atau `400 Bad Request`.

---

#### 3. Jelaskan arti dari kode response HTTP 200 dan sebutkan salah satu contoh kode response HTTP lain beserta artinya!
* **Kode Response HTTP 200 (OK)**:
  Menandakan bahwa permintaan (*HTTP Request*) yang dikirimkan oleh klien (ESP) telah **berhasil diterima, dipahami, dan diproses dengan sukses** oleh server tujuan, dan server mengembalikan data balasan yang sesuai (*response body*).
* **Contoh Kode Response HTTP Lainnya**:
  * **HTTP 404 (Not Found)**: Sumber daya, direktori, atau endpoint URL yang dituju oleh klien tidak ditemukan di server (misal salah ketik path `/post` menjadi `/posst`).
  * **HTTP 400 (Bad Request)**: Permintaan klien mengandung sintaks yang salah atau payload JSON korup sehingga tidak dapat dipahami oleh server.
  * **HTTP 401 (Unauthorized)**: Permintaan ditolak karena membutuhkan autentikasi (kunci API / token bearer) yang tidak disertakan atau tidak valid.
  * **HTTP 500 (Internal Server Error)**: Terjadi kegagalan atau *crash* pada sistem backend server saat memproses permintaan klien.

---

### 3.3 Modifikasi Kode Percobaan 3A (Menambahkan Parameter `millis()`)

#### Soal Modifikasi Modul:
> **"Modifikasi program agar ESP32 dapat mengirimkan data tambahan berupa waktu (dalam milidetik sejak dinyalakan menggunakan millis()) ke dalam JSON yang dikirim, dan berikan penjelasan di setiap baris kode yang ditambahkan dalam bentuk README.md"**

File implementasi: [`code/percobaan3A_modifikasi.cpp`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%203/code/percobaan3A_modifikasi.cpp)

```cpp
#if defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif
#include <ArduinoJson.h>

const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";
const char* serverUrl = "http://httpbin.org/post";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    #if defined(ESP32)
      HTTPClient http;
      http.begin(serverUrl);
    #elif defined(ESP8266)
      WiFiClient client;
      HTTPClient http;
      http.begin(client, serverUrl);
    #endif

    http.addHeader("Content-Type", "application/json");

    // Membuat objek dokumen JSON
    JsonDocument doc;
    doc["suhu"] = 28.5;
    doc["kelembaban"] = 65.0;

    // === BARIS MODIFIKASI: PENAMBAHAN PARAMETER WAKTU UPTIME ===
    doc["waktu"] = millis();

    // Serialisasi JSON menjadi String
    String requestBody;
    serializeJson(doc, requestBody);

    Serial.print("Mengirim data modifikasi: ");
    Serial.println(requestBody);

    // Mengirim data melalui HTTP POST
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);
      Serial.println("Isi Response:");
      Serial.println(http.getString());
    } else {
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi terputus, transmisi dibatalkan.");
  }

  delay(10000); // Jeda pengiriman setiap 10 detik
}
```

---

### 3.4 Penjelasan Baris demi Baris Kode Modifikasi 3A
Penjelasan khusus baris instruksi yang ditambahkan dan dimodifikasi:

1. **`doc["waktu"] = millis();`**  
   * **Fungsi**: Menambahkan *key* baru bernama `"waktu"` ke dalam objek JSON `doc`.
   * **Nilai**: Mengambil nilai kembalian dari fungsi sistem bawaan Arduino `millis()`, yaitu jumlah milidetik bertipe data `unsigned long` yang telah terlewati sejak board mikrokontroler pertama kali dinyalakan (*uptime system*).
   * **Hasil JSON**: Payload yang sebelumnya hanya berupa:
     ```json
     {"suhu": 28.5, "kelembaban": 65.0}
     ```
     Kini berubah menjadi format lengkap berstempel waktu:
     ```json
     {"suhu": 28.5, "kelembaban": 65.0, "waktu": 10034}
     ```
2. **`serializeJson(doc, requestBody);`**  
   Mengonversi struktur objek JSON yang telah memuat elemen data baru `"waktu"` tersebut menjadi format teks `String` terkompresi secara otomatis.
3. **`Serial.println(requestBody);`**  
   Mencetak string JSON hasil modifikasi ke Serial Monitor sebagai verifikasi lokal sebelum paket dikirim ke internet.
4. **`int httpResponseCode = http.POST(requestBody);`**  
   Mengirimkan payload termodifikasi ke server `httpbin.org/post`, di mana server akan menggemakan kembali seluruh objek JSON termasuk parameter `waktu` tersebut pada kunci `"data"`.

---

### 3.5 Penjelasan Fungsi yang Digunakan pada Percobaan 3A

| Nama Fungsi | Parameter Masukan | Return Value | Penjelasan Kegunaan Teknis |
|---|---|---|---|
| `http.begin(url)` / `http.begin(client, url)` | `serverUrl` (String) | `bool` | Membuka inisialisasi koneksi soket TCP menuju host dan path endpoint tujuan. |
| `http.addHeader(header, value)` | `"Content-Type"`, `"application/json"` | `void` | Menyisipkan header MIME type pada paket request HTTP sebelum dikirim. |
| `serializeJson(doc, output)` | `JsonDocument`, `String&` | `size_t` | Mengonversi objek data pohon JSON menjadi representasi string teks datar. |
| `http.POST(payload)` | `requestBody` (String) | `int` (HTTP Status Code) | Mengeksekusi pengiriman HTTP Request bertipe POST beserta isi body payload ke server. |
| `http.getString()` | *None* | `String` | Membaca seluruh data balasan (*response body*) yang dikembalikan oleh server web. |
| `http.end()` | *None* | `void` | Menutup session HTTP dan membebaskan alokasi heap memori internal mikrokontroler. |
| `millis()` | *None* | `unsigned long` | Mengembalikan waktu berjalan mikrokontroler sejak dihidupkan (dalam milidetik). |

---

### 3.6 Penjelasan Percabangan / Conditional Logic pada Percobaan 3A

1. **Percabangan Status WiFi `if (WiFi.status() == WL_CONNECTED)`**:
   * Memastikan bahwa perangkat memiliki konektivitas aktif ke router sebelum mencoba membuat koneksi TCP ke server web. Jika terputus, ESP tidak memaksakan eksekusi HTTP yang dapat menyebabkan sistem *freeze*.
2. **Percabangan Evaluasi Response Code `if (httpResponseCode > 0)`**:
   * Mengevaluasi apakah komunikasi HTTP berhasil mencapai level aplikasi.
   * **Kondisi Positif (`> 0`)**: Server memberikan respon standar HTTP (misal: 200, 404, 500), sehingga program dapat menampilkan response body server.
   * **Kondisi Negatif (`<= 0`)**: Terjadi kegagalan pada level jaringan (misal: error `-1` untuk *connection refused / timeout*, atau `-11` untuk *read timeout*), sehingga program menampilkan kode kegagalan jaringan.

---

---

## 4. Percobaan 3B: Komunikasi Data Menggunakan MQTT

### 4.1 Penjelasan Kode Base Percobaan 3B
File implementasi: [`code/percobaan3B.cpp`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%203/code/percobaan3B.cpp)

Pada percobaan ini, mikrokontroler bertindak sebagai **MQTT Publisher**:
1. Menggunakan pustaka `PubSubClient` untuk mengelola protokol MQTT melalui soket TCP `WiFiClient`.
2. Terhubung ke broker publik `broker.hivemq.com` pada port standar `1883`.
3. Menggunakan topic pengiriman spesifik: `"apipdivahana"`.
4. Menyusun data format JSON pada objek `JsonDocument`, kemudian menserialisasikannya ke dalam array karakter (`char buffer[128]`).
5. Mempublikasikan pesan menggunakan `client.publish(mqttTopic, buffer)` secara periodik.

---

### 4.2 Jawaban Pertanyaan Praktikum 3B

#### 1. Apa fungsi dari topic pada protokol MQTT, dan mengapa topic yang digunakan perlu dibuat unik?
* **Fungsi Topic**:
  Topic pada protokol MQTT berfungsi sebagai **alamat perutean logis (*routing key / category filter*)** tempat data atau pesan (*payload*) dikirimkan oleh *Publisher* dan disaring oleh *Broker*. MQTT tidak menggunakan alamat IP klien untuk mengirimkan pesan, melainkan mengelompokkan pesan berdasarkan struktur hirarki topic (seperti `kampus/gedungA/lantai1/suhu`). *Subscriber* yang tertarik hanya perlu mendaftar (*subscribe*) ke topic terkait untuk menerima pesan.
* **Alasan Mengapa Topic Perlu Dibuat Unik**:
  Pada praktikum ini digunakan broker MQTT publik (`broker.hivemq.com`) yang dapat diakses oleh ribuan pengguna di seluruh dunia secara bersamaan. Jika topic yang digunakan bersifat umum (misalnya hanya `sensor` atau `suhu`), data yang kita kirimkan akan tertimpa atau tercampur dengan data milik mahasiswa/pengguna lain. Oleh karena itu, topic harus dibuat unik (misal: `apipdivahana` atau menyertakan NIM `unsoed/H1H024016/sensor`) agar data terisolasi dan tidak terjadi distorsi penerimaan data.

---

#### 2. Jelaskan fungsi dari perintah `client.loop()` yang dipanggil pada setiap iterasi `loop()`!
Perintah `client.loop()` memiliki fungsi yang sangat krusial dalam siklus hidup protokol MQTT:
1. **Mempertahankan Koneksi (*Keep-Alive Management*)**: Mengirimkan paket `PINGREQ` secara berkala ke broker dan menerima `PINGRESP` agar sesi koneksi TCP tidak diputus sepihak (*timeout*) oleh broker.
2. **Memproses Pesan Masuk (*Inbound Message Processing*)**: Membaca paket data dari buffer jaringan soket dan meneruskannya ke fungsi *callback* (jika mikrokontroler bertindak sebagai *subscriber*).
3. **Mengelola Alur Transmisi (*Outbound Buffer Flush*)**: Memastikan paket publikasi QoS yang tertunda terkirim dan dibersihkan dari memori mikrokontroler.

> [!IMPORTANT]
> Jika `client.loop()` tidak dipanggil secara teratur dalam fungsi `loop()`, broker MQTT akan menganggap klien mati (*dead connection*) dan akan memutus sambungan (*disconnect*) setelah periode *keep-alive* terlewati (biasanya 15 detik).

---

#### 3. Apa yang akan terjadi apabila koneksi ke broker MQTT terputus di tengah program berjalan?
Apabila koneksi jaringan atau koneksi broker terputus di tengah jalan:
1. Panggilan fungsi `client.connected()` di dalam fungsi `loop()` mikrokontroler akan bernilai `false`.
2. Alur program mengeksekusi blok pemanggilan fungsi `hubungkanMQTT()`.
3. Di dalam fungsi `hubungkanMQTT()`, perulangan `while (!client.connected())` akan melakukan upaya rekoneksi secara otomatis dengan:
   * Meng-generate *Client ID* acak baru (misal: `ESPClient-XXXX`).
   * Mengirimkan paket `CONNECT` baru ke broker dengan jeda waktu tunda (*retry interval*) selama 2 detik.
4. Mikrokontroler akan terus mencoba menyambung kembali hingga status koneksi pulih, sehingga sistem IoT memiliki ketahanan (*resilience*) tinggi terhadap gangguan jaringan sementara.

---

### 4.3 Modifikasi Kode Percobaan 3B (Menambahkan Parameter `millis()`)

File implementasi: [`code/percobaan3B_modifikasi.cpp`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%203/code/percobaan3B_modifikasi.cpp)

```cpp
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid       = "NAMA_WIFI_ANDA";
const char* password   = "PASSWORD_WIFI_ANDA";
const char* mqttServer = "broker.hivemq.com";
const int   mqttPort   = 1883;
const char* mqttTopic  = "apipdivahana";

WiFiClient espClient;
PubSubClient client(espClient);

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
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
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop();

  // Membentuk data sensor dalam format JSON
  JsonDocument doc;
  doc["suhu"] = 28.5;
  doc["kelembaban"] = 65.0;

  // === BARIS MODIFIKASI: PENAMBAHAN PARAMETER WAKTU UPTIME (millis()) ===
  doc["waktu"] = millis();

  // Serialisasi data JSON ke dalam buffer karakter
  char buffer[128];
  serializeJson(doc, buffer);

  // Mempublikasikan payload JSON ke topic broker
  client.publish(mqttTopic, buffer);
  Serial.print("Data terkirim ke topic ");
  Serial.print(mqttTopic);
  Serial.print(": ");
  Serial.println(buffer);

  delay(10000); // Jeda publish setiap 10 detik
}
```

---

### 4.4 Penjelasan Baris demi Baris Kode Modifikasi 3B
1. **`doc["waktu"] = millis();`**  
   Menambahkan atribut baru ke objek JSON bernama `"waktu"` yang nilainya diambil dari penghitung milidetik *uptime* internal perangkat (`millis()`).
2. **`char buffer[128];`**  
   Menyiapkan alokasi memori berupa *array of char* berukuran 128 byte pada stack memori lokal. Pada protokol MQTT, fungsi `publish()` membutuhkan parameter masukan berupa *pointer* karakter (`const char*`) atau *byte array*, bukan objek `String` C++ dinamis.
3. **`serializeJson(doc, buffer);`**  
   Mengisi `buffer` dengan representasi teks JSON yang telah menyertakan parameter `waktu`.
4. **`client.publish(mqttTopic, buffer);`**  
   Mengeksekusi paket kontrol MQTT `PUBLISH` ke broker HiveMQ pada topik `apipdivahana`.
5. **Output Serial yang Dihasilkan**:
   ```text
   Data terkirim ke topic apipdivahana: {"suhu":28.5,"kelembaban":65.0,"waktu":10034}
   ```

---

### 4.5 Penjelasan Fungsi yang Digunakan pada Percobaan 3B

| Nama Fungsi | Parameter Masukan | Return Value | Penjelasan Kegunaan Teknis |
|---|---|---|---|
| `client.setServer(host, port)` | `mqttServer`, `mqttPort` | `PubSubClient&` | Mengonfigurasi parameter host broker MQTT dan nomor port (1883). |
| `client.connected()` | *None* | `bool` | Memeriksa apakah status sesi koneksi soket TCP ke broker masih aktif. |
| `client.connect(clientId)` | `const char* id` | `bool` | Mengirimkan paket `CONNECT` untuk membuka sesi komunikasi ke broker MQTT. |
| `client.state()` | *None* | `int` | Mengembalikan status internal koneksi/error kode return (rc) dari PubSubClient. |
| `client.loop()` | *None* | `bool` | Menjaga koneksi keep-alive, memproses paket masuk, dan menembakkan ping packet. |
| `client.publish(topic, payload)` | `topic`, `buffer` | `bool` | Mengirimkan paket publikasi data ke topic broker tertentu. |
| `random(max)` | `0xffff` | `long` | Menghasilkan angka acak pseudo untuk membangun Client ID yang unik. |

---

### 4.6 Penjelasan Percabangan / Conditional Logic pada Percobaan 3B

1. **Percabangan Deteksi Koneksi Putus `if (!client.connected())`**:
   * Mengevaluasi apakah koneksi aktif ke broker terputus. Jika terputus, memanggil sub-rutin `hubungkanMQTT()`.
2. **Perulangan Blocking Rekoneksi `while (!client.connected())`**:
   * Menjamin program tidak akan mempublikasikan data ke soket kosong selama belum berhasil menyambung kembali ke broker.
3. **Percabangan Validasi Handshake `if (client.connect(clientId.c_str()))`**:
   * Jika paket `CONNACK` dari broker mengindikasikan koneksi diterima (*accepted*), sistem melanjutkan eksekusi normal. Jika ditolak, sistem membaca kode penyebab lewat `client.state()` dan menunda percobaan berikutnya selama 2 detik.

---

---

## 5. Jawaban Pertanyaan Analisis (Bagian 3.7 Modul)

### 1. Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!
* **Percobaan 3A (HTTP POST)**:
  ESP berhasil tersambung ke jaringan WiFi internet dan mengirimkan data JSON secara periodik ke `httpbin.org/post`. Seluruh data 10 kali pengujian (detik ke-0 hingga 90) berhasil dikirim dengan kode respon **HTTP 200 (OK)** dan server mengembalikan echo payload yang cocok.
* **Percobaan 3B (MQTT Publish)**:
  ESP berhasil terhubung ke broker publik HiveMQ dan mempublikasikan data ke topik `apipdivahana`. Melalui aplikasi *MQTT Explorer / HiveMQ WebSocket Client*, data terverifikasi diterima secara *real-time* dengan integritas data 100% tanpa error.

---

### 2. Bandingkan besar overhead data dan pola komunikasi antara protokol HTTP dan MQTT berdasarkan hasil percobaan yang telah dilakukan!

| Kriteria Komparasi | Protokol HTTP | Protokol MQTT |
|---|---|---|
| **Pola Komunikasi** | *Request - Response* (Tarik/Dorong Statis) | *Publish - Subscribe* (Asinkronus / Terpusat Broker) |
| **Sifat Koneksi** | *Stateless* (Koneksi TCP dibuka dan ditutup berulang) | *Stateful / Persistent* (Koneksi TCP tetap terbuka) |
| **Ukuran Header Minimum** | **Besar** (~200 - 800 bytes per request) | **Sangat Kecil** (hanya **2 bytes** fixed header) |
| **Beban Bandwidth** | Boros kuota data karena header HTTP berulang | Sangat hemat, cocok untuk jaringan seluler/IoT |
| **Kecepatan / Latensi** | Lebih lambat karena ada *overhead handshaking* di setiap siklus | Sangat cepat dan *real-time* |

---

### 3. Untuk skenario pengiriman data sensor secara terus-menerus setiap beberapa detik dalam jangka waktu lama, protokol manakah (HTTP atau MQTT) yang lebih sesuai digunakan? Jelaskan alasannya!

**Protokol yang paling sesuai adalah MQTT.**

**Alasan Teknis**:
1. **Efisiensi Konsumsi Daya & Baterai**: Karena ukuran header MQTT hanya 2 byte dibandingkan ratusan byte pada HTTP, antena radio nirkabel ESP aktif dalam durasi yang jauh lebih singkat (*TX time lebih rendah*), secara signifikan menghemat konsumsi energi baterai.
2. **Efisiensi Bandwidth Jaringan**: Menghindari pemborosan kuota data internet hingga lebih dari 80-90% dibandingkan transmisi HTTP POST berkala.
3. **Koneksi Tetap Terjaga (Persistent TCP)**: MQTT tidak perlu membuang waktu dan sumber daya untuk melakukan proses *TCP 3-way handshake* dan *TLS handshake* berulang-ulang di setiap pengiriman data, melainkan cukup memanfaatkan koneksi yang sudah terbuka (*established*).

---

### 4. Bagaimana peran format JSON dalam mendukung interoperabilitas data antara perangkat IoT dan berbagai platform/aplikasi yang berbeda?
Format **JSON (*JavaScript Object Notation*)** bertindak sebagai standar *universal data interchange*:
1. **Bahasa Netral (*Language-Agnostic*)**: JSON didukung secara bawaan oleh hampir seluruh bahasa pemrograman modern (JavaScript/TypeScript, Python, PHP, C/C++, Java, Go, C#).
2. **Struktur Fleksibel & Terbaca (*Self-Describing*)**: Pasangan *key-value* memungkinkan mikrokontroler mengirimkan beragam parameter sensor (suhu, kelembaban, tekanan, status baterai, koordinat GPS) dalam satu payload terpadu yang mudah dibaca oleh manusia (*human-readable*) dan mudah diparsing oleh mesin (*machine-readable*).
3. **Kompatibilitas Cloud Platform**: Format JSON merupakan standar baku bagi integrasi *REST API*, *database NoSQL* (MongoDB, CouchDB), serta *IoT Cloud Dashboard* (ThingsBoard, AWS IoT Core, Google Cloud IoT, Adafruit IO).

---

## 6. Catatan Kendala dan Solusi

Berdasarkan pelaksanaan praktikum pada Modul 3, dicatat beberapa kendala teknis dan cara penanganannya:
1. **Perbedaan Pustaka HTTP Board**:
   * *Kendala*: Pada board ESP8266, penggunaan `#include <HTTPClient.h>` akan memicu kompilasi gagal karena nama header di ESP8266 adalah `<ESP8266HTTPClient.h>`. Selain itu fungsi `http.begin()` pada ESP8266 memerlukan instansiasi `WiFiClient client` sebagai argumen pertama (`http.begin(client, serverUrl)`).
   * *Solusi*: Menggunakan direktif pra-pemrosesan multi-board (`#if defined(ESP32) ... #elif defined(ESP8266) ... #endif`).
2. **Isu Port dan Protokol HTTPS vs HTTP**:
   * *Kendala*: Penggunaan URL `https://` pada ESP8266 membutuhkan `WiFiClientSecure` dan perintah `client.setInsecure()`, yang membebani memori RAM (heap) hingga dapat menyebabkan crash/error -1.
   * *Solusi*: Menggunakan endpoint `http://httpbin.org/post` pada port 80 untuk pengujian dasar yang stabil dan hemat RAM.
3. **Versi Pustaka ArduinoJson (v6 vs v7)**:
   * *Kendala*: Sintaks `JsonDocument doc;` merupakan sintaks resmi ArduinoJson versi 7.x. Jika pengguna menggunakan ArduinoJson versi 6.x, penulisan tersebut akan memicu error kompilasi dan harus ditulis menggunakan `StaticJsonDocument<200> doc;`.
   * *Solusi*: Memastikan versi library ArduinoJson pada Library Manager diperbarui ke versi 7 atau menyesuaikan tipe dokumen.
4. **Frekuensi Jaringan WiFi**:
   * *Kendala*: ESP8266 dan ESP32 varian standar hanya mendukung WiFi frekuensi **2.4 GHz**. Jika smartphone/router memancarkan WiFi pada band **5 GHz**, mikrokontroler tidak akan mendeteksi sinyal (*stuck connecting*).
   * *Solusi*: Mengatur konfigurasi hotspot ke frekuensi 2.4 GHz.

---

## 7. Dokumentasi Praktikum

Aset dokumentasi hasil pengujian praktikum dapat diakses pada folder: [`dokumentasi/`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%203/dokumentasi)

* **Flowchart HTTP POST**: [`dokumentasi/flowchart_http_post.png`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%203/dokumentasi/flowchart_http_post.png)
* **README Dokumentasi**: [`dokumentasi/README.md`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%203/dokumentasi/README.md)
