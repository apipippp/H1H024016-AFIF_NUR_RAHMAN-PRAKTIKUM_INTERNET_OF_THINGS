# Jawaban Soal Modifikasi Kode — Modul 2: Konfigurasi Jaringan

**Mata Kuliah** : Praktikum Internet of Things (TK245005)  
**Nama Praktikan** : Afif Nur Rahman  
**NIM** : H1H024016  
**Shift** : A  
**Asisten** : Athallah Tsany Satriyaji  

---

## 1. Percobaan 2A: Modifikasi Auto Reconnect Mode Station (STA)

### Soal Modul (Percobaan 2A Poin 4):
> **"Modifikasi program agar ESP32 mencoba menghubungkan ulang (reconnect) secara otomatis apabila koneksi WiFi terputus, dan berikan penjelasan di setiap baris kode yang ditambahkan dalam bentuk README.md!"**

---

### A. Penjelasan Singkat Percobaan & Tujuan Modifikasi
Pada program dasar Percobaan 2A, mikrokontroler (ESP32/ESP8266) hanya melakukan penyambungan WiFi satu kali pada blok `setup()`. Jika di tengah operasional koneksi WiFi terputus (misalnya router dimatikan, restart, atau sinyal hilang), mikrokontroler pada kode dasar hanya mencetak `"Status: Terputus"` di `loop()` tanpa melakukan usaha pemulihan koneksi.

**Tujuan Modifikasi:**
Menambahkan mekanisme *auto reconnect* cerdas di dalam fungsi `loop()` sehingga ketika sambungan terputus:
1. LED indikator segera dimatikan sebagai penanda visual status *offline*.
2. Sisa sesi koneksi lama dibersihkan (`WiFi.disconnect()`) agar buffer socket TCP/IP tidak menggantung (*stale*).
3. Mikrokontroler memulai inisiasi koneksi baru (`WiFi.begin()`).
4. Proses rekoneksi dibatasi maksimal 20 iterasi (sekitar 10 detik) menggunakan variabel *counter* `attempts`. Hal ini penting agar mikrokontroler **tidak mengalami freeze / infinite loop** dan mencegah terpicunya *Watchdog Timer* (WDT reset).
5. Jika berhasil terhubung kembali, LED indikator dinyalakan kembali dan status koneksi dipulihkan.

---

### B. Library / Dependencies yang Diperlukan
Program menggunakan pustaka bawaan *board package* mikrokontroler dengan deteksi preprocessor otomatis:
* **ESP32** : `<WiFi.h>` (Pustaka resmi Espressif)
* **ESP8266** : `<ESP8266WiFi.h>` (Pustaka resmi ESP8266 Community)

```cpp
#if defined(ESP32)
  #include <WiFi.h>
  const int ledPin = 2;   // LED bawaan pada ESP32 (GPIO 2)
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  const int ledPin = D2;  // Pin D2 pada NodeMCU ESP8266
#endif
```

---

### C. Kode Program Modifikasi Lengkap
File kode tersimpan pada: [`code/percobaan2A_reconnect.cpp`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%202/code/percobaan2A_reconnect.cpp)

```cpp
// 1. Deteksi platform mikrokontroler otomatis
#if defined(ESP32)
  #include <WiFi.h>
  const int ledPin = 2;   // GPIO 2 untuk LED bawaan ESP32
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  const int ledPin = D2;  // Pin D2 untuk LED pada NodeMCU
#endif

// 2. Kredensial WiFi target
const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

void setup() {
  // 3. Inisialisasi komunikasi serial
  Serial.begin(115200);
  delay(100);

  // 4. Konfigurasi pin LED sebagai OUTPUT dan matikan di awal
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // 5. Atur mode WiFi menjadi Station murni
  WiFi.mode(WIFI_STA);

  // 6. Mulai koneksi awal ke WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");

  // 7. Tunggu sampai koneksi awal berhasil
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 8. Tampilkan parameter jaringan saat pertama kali terhubung
  Serial.println("\nWiFi berhasil terhubung!");
  Serial.print("IP Address  : ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address : ");
  Serial.println(WiFi.macAddress());
  Serial.print("RSSI (dBm)  : ");
  Serial.println(WiFi.RSSI());

  // 9. Nyalakan LED indikator sukses terhubung
  digitalWrite(ledPin, HIGH);
}

void loop() {
  // 10. Periksa status koneksi secara berkala
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Terhubung");
  } else {
    // === BLOK AUTO RECONNECT (KODE TAMBAHAN) ===
    // 11. Tampilkan notifikasi pemutusan koneksi
    Serial.println("Koneksi terputus! Mencoba menghubungkan ulang...");
    
    // 12. Matikan LED indikator untuk menandakan status offline
    digitalWrite(ledPin, LOW);

    // 13. Putuskan sisa sesi koneksi lama yang menggantung
    WiFi.disconnect();
    
    // 14. Mulai inisiasi sambungan baru ke Access Point
    WiFi.begin(ssid, password);

    // 15. Inisialisasi variabel counter batas percobaan reconnect
    int attempts = 0;
    
    // 16. Loop polling reconnect dengan batas maksimal 20 percobaan (~10 detik)
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    // 17. Evaluasi hasil setelah siklus percobaan reconnect
    if (WiFi.status() == WL_CONNECTED) {
      // 18. Jika berhasil terhubung kembali, nyalakan kembali LED indikator
      Serial.println("\nBerhasil terhubung kembali!");
      digitalWrite(ledPin, HIGH);
    } else {
      // 19. Jika belum berhasil setelah 20 kali percobaan, cetak status gagal
      Serial.println("\nGagal terhubung. Akan dicoba lagi pada siklus berikutnya.");
    }
  }

  // 20. Jeda interval 5 detik sebelum siklus pengujian berikutnya
  delay(5000);
}
```

---

### D. Penjelasan Baris demi Baris Kode Modifikasi yang Ditambahkan

Bagian modifikasi utama terletak pada blok `else` di dalam fungsi `loop()`:

1. **`Serial.println("Koneksi terputus! Mencoba menghubungkan ulang...");`**  
   Mencetak log peringatan ke Serial Monitor bahwa koneksi ke Access Point telah putus dan proses *auto reconnect* dimulai.
2. **`digitalWrite(ledPin, LOW);`**  
   Mematikan LED indikator secara langsung agar pengguna mengetahui secara visual bahwa perangkat sedang kehilangan koneksi (*offline*).
3. **`WiFi.disconnect();`**  
   Memutuskan relasi sambungan yang menggantung dan membersihkan cache buffer socket TCP/IP pada *network stack* LwIP agar siap menerima koneksi baru tanpa error *stale connection*.
4. **`WiFi.begin(ssid, password);`**  
   Memanggil kembali API koneksi dengan kredensial SSID dan kata sandi yang telah disimpan untuk mengirimkan paket *Probe Request* dan *Association Request* baru ke router.
5. **`int attempts = 0;`**  
   Mendeklarasikan variabel pencacah lokal bertipe integer sebagai *guard counter* untuk menghitung berapa kali pengecekan status telah dilakukan.
6. **`while (WiFi.status() != WL_CONNECTED && attempts < 20)`**  
   Blok perulangan penahan (*bounded polling loop*) yang akan terus berjalan selama status belum terhubung **DAN** jumlah percobaan masih di bawah 20 kali. Batas 20 kali dengan delay 500 ms per siklus ($20 \times 500\text{ ms} = 10\text{ detik}$) menjamin mikrokontroler tidak macet (*hang forever*) seandainya router WiFi mati total.
7. **`delay(500); Serial.print("."); attempts++;`**  
   Memberikan jeda waktu 500 ms agar modul radio memiliki waktu melakukan proses *handshaking*, mencetak tanda titik ke serial monitor, dan menaikkan nilai counter percobaan (`attempts`).
8. **`if (WiFi.status() == WL_CONNECTED)`**  
   Pemeriksaan kondisi setelah keluar dari perulangan. Jika koneksi pulih dalam kurun waktu 10 detik tersebut, blok ini dieksekusi.
9. **`Serial.println("\nBerhasil terhubung kembali!"); digitalWrite(ledPin, HIGH);`**  
   Mencetak konfirmasi keberhasilan ke terminal dan menyalakan kembali LED indikator (`HIGH`).
10. **`else { Serial.println("\nGagal terhubung. Akan dicoba lagi pada siklus berikutnya."); }`**  
    Jika dalam batas 20 percobaan koneksi belum pulih, program tidak berhenti (*crash*), melainkan melepaskan eksekusi ke `delay(5000)` di akhir `loop()` dan akan mencoba menyambung ulang secara otomatis pada iterasi berikutnya.

---

### E. Penjelasan Setiap Fungsi yang Digunakan pada Percobaan 2A

| Nama Fungsi | Parameter Masukan | Return Value | Penjelasan Kegunaan Teknis |
|---|---|---|---|
| `Serial.begin(speed)` | `115200` | `void` | Mengaktifkan transmisi data serial UART antara mikrokontroler dan komputer pada baud rate 115200 bps. |
| `pinMode(pin, mode)` | `ledPin`, `OUTPUT` | `void` | Mengonfigurasi mode pin GPIO mikrokontroler sebagai pin keluaran (*output*) digital. |
| `digitalWrite(pin, val)`| `ledPin`, `HIGH` / `LOW` | `void` | Mengatur taraf tegangan logika pin; `HIGH` (3.3V) menyalakan LED dan `LOW` (0V) mematikan LED. |
| `WiFi.mode(mode)` | `WIFI_STA` | `bool` | Mengonfigurasi antarmuka radio WiFi beroperasi murni sebagai klien (*Station*). |
| `WiFi.begin(ssid, pass)`| `ssid`, `password` | `wl_status_t` | Memulai inisiasi komunikasi nirkabel dan autentikasi WPA2 ke router WiFi target. |
| `WiFi.status()` | *None* | `wl_status_t` (enum) | Memeriksa status koneksi WiFi saat ini (misal: `WL_CONNECTED` bernilai integer 3). |
| `WiFi.disconnect()` | *None* | `bool` | Memutus koneksi aktif dan mereset status antarmuka jaringan nirkabel. |
| `WiFi.localIP()` | *None* | `IPAddress` | Mengembalikan alamat IPv4 lokal yang diberikan oleh DHCP Server router luar. |
| `WiFi.macAddress()` | *None* | `String` | Membaca alamat fisik permanen (*Media Access Control*) 48-bit pada chip radio ESP. |
| `WiFi.RSSI()` | *None* | `int32_t` | Mengukur kekuatan sinyal radio nirkabel yang diterima dalam satuan dBm. |
| `delay(ms)` | `500` / `5000` | `void` | Memberikan jeda waktu tunda eksekusi instruksi dalam satuan milidetik. |

---

### F. Penjelasan Percabangan / Conditional Logic pada Percobaan 2A

1. **Percabangan `if-else` Monitoring Jaringan**:
   ```cpp
   if (WiFi.status() == WL_CONNECTED) { ... } else { ... }
   ```
   * **Logika**: Menguji apakah kondisi `WiFi.status() == WL_CONNECTED` bernilai `true` (koneksi aktif).
   * **Alur**: Jika `true`, sistem mempertahankan status terhubung. Jika `false`, alur langsung dialihkan ke blok `else` untuk mengeksekusi urutan pemulihan jaringan (*auto reconnect*).

2. **Perulangan Bersyarat Ganda (*Bounded While Loop*)**:
   ```cpp
   while (WiFi.status() != WL_CONNECTED && attempts < 20)
   ```
   * **Logika**: Menggunakan operator logika AND (`&&`). Perulangan hanya berjalan jika **KEDUA** kondisi bernilai `true`:
     1. Status WiFi belum terhubung (`WiFi.status() != WL_CONNECTED`).
     2. Variabel pencacah belum melampaui batas (`attempts < 20`).
   * **Tujuan**: Mencegah *Deadlock / Infinite Loop*. Jika router mati dalam waktu lama, program akan berhenti mencoba setelah 20 kali ($10\text{ detik}$) dan mengizinkan mikrokontroler memproses tugas lain sebelum mencoba kembali di siklus berikutnya.

3. **Percabangan Validasi Hasil Rekoneksi**:
   ```cpp
   if (WiFi.status() == WL_CONNECTED) { ... } else { ... }
   ```
   * **Logika**: Menguji apakah keluarnya alur dari perulangan `while` disebabkan oleh keberhasilan sambungan atau karena batas *timeout* 20 kali tercapai.

---

---

## 2. Percobaan 2B: Modifikasi Dual Mode AP + STA (Hybrid)

### Soal Modul (Percobaan 2B Poin 4):
> **"Modifikasi program agar ESP32 berjalan pada mode AP+STA (terhubung ke WiFi rumah sekaligus menyediakan Access Point), dan berikan penjelasan di setiap baris kodenya dalam bentuk README.md!"**

---

### A. Penjelasan Singkat Percobaan & Tujuan Modifikasi
Pada program dasar Percobaan 2B, mikrokontroler hanya beroperasi murni sebagai *Access Point* mandiri (`WIFI_AP`), di mana ESP memancarkan hotspot sendiri namun tidak memiliki sambungan ke jaringan luar/internet.

**Tujuan Modifikasi:**
Mengaktifkan mode ganda **`WIFI_AP_STA` (Hybrid Mode)** sehingga mikrokontroler menjalankan dua peran jaringan secara bersamaan:
1. **Sebagai Station (STA)**: Menghubungkan diri ke router WiFi rumah/utama untuk mendapatkan alamat IP lokal dan akses ke jaringan internet atau cloud.
2. **Sebagai Access Point (AP / SoftAP)**: Secara simultan memancarkan sinyal hotspot mandiri dengan SSID dan password sendiri, sehingga perangkat lain (laptop/smartphone) dapat terhubung langsung ke ESP tanpa melalui router perantara.

---

### B. Library / Dependencies yang Diperlukan
Pustaka yang digunakan sama dengan percobaan sebelumnya:
* `<WiFi.h>` (ESP32) atau `<ESP8266WiFi.h>` (ESP8266 / NodeMCU).

---

### C. Kode Program Modifikasi Lengkap
File kode tersimpan pada: [`code/percobaan2B_ap_sta.cpp`](file:///c:/KULIAHHHHHHHHH/TUGAS%20KULIAH/SEMESTER%205/PRAKTIKUM%20IOT/REPO%20GIT/H1H024016-AFIF_NUR_RAHMAN-PRAKTIKUM_INTERNET_OF_THINGS/Pertemuan%202/code/percobaan2B_ap_sta.cpp)

```cpp
// 1. Deteksi platform mikrokontroler secara otomatis
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

// 2. Kredensial untuk terhubung ke Router Luar (Mode Station)
const char* sta_ssid     = "WIFI_RUMAH";
const char* sta_password = "PASSWORD_WIFI";

// 3. Kredensial untuk memancarkan Hotspot Lokal (Mode Access Point)
const char* ap_ssid      = "ESP32_Hybrid_AP";
const char* ap_password  = "securepassword"; // Minimal 8 karakter

void setup() {
  // 4. Inisialisasi komunikasi serial dengan baud rate 115200
  Serial.begin(115200);
  delay(100);

  // 5. Mengaktifkan mode operasi WiFi ganda (AP + STA secara bersamaan)
  WiFi.mode(WIFI_AP_STA);

  // ==========================================
  // 6. KONFIGURASI 1: MODE STATION (Klien Router)
  // ==========================================
  // Memulai proses koneksi ke router utama
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Menghubungkan ke Router utama");

  // Loop polling menunggu sambungan ke router berhasil
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Tampilkan notifikasi keberhasilan dan cetak IP lokal dari DHCP router
  Serial.println("\nBerhasil tersambung ke Router!");
  Serial.print("IP Station (Lokal)       : ");
  Serial.println(WiFi.localIP());

  // ==========================================
  // 7. KONFIGURASI 2: MODE ACCESS POINT (Hotspot)
  // ==========================================
  // Memancarkan sinyal hotspot mandiri dari chip mikrokontroler
  WiFi.softAP(ap_ssid, ap_password);

  // Tampilkan status aktivasi hotspot beserta SSID dan IP gateway lokal ESP
  Serial.println("\nAccess Point Hybrid juga diaktifkan!");
  Serial.print("SSID AP                  : ");
  Serial.println(ap_ssid);
  Serial.print("IP Access Point (Gateway): ");
  Serial.println(WiFi.softAPIP()); // Default: 192.168.4.1
}

void loop() {
  // 8. Memonitor jumlah client yang sedang menumpang ke sinyal AP ESP
  int jumlahClient = WiFi.softAPgetStationNum();

  Serial.print("Jumlah perangkat terhubung ke AP: ");
  Serial.println(jumlahClient);

  // 9. Mengecek kestabilan koneksi mode Station ke router
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Koneksi Station (Router)       : Stabil");
  } else {
    Serial.println("Koneksi Station (Router)       : Terputus!");
  }

  // 10. Jeda 5 detik sebelum pemantauan berkala berikutnya
  delay(5000);
}
```

---

### D. Penjelasan Baris demi Baris Kode Modifikasi yang Ditambahkan

1. **`const char* sta_ssid = "..."; const char* sta_password = "...";`**  
   Mendefinisikan variabel string konstan untuk menyimpan SSID dan kata sandi router WiFi eksternal yang akan dituju oleh antarmuka Station.
2. **`const char* ap_ssid = "..."; const char* ap_password = "...";`**  
   Mendefinisikan variabel string konstan untuk nama SSID hotspot lokal yang dipancarkan oleh ESP beserta kata sandinya (minimal 8 karakter untuk memenuhi standar WPA2-PSK).
3. **`WiFi.mode(WIFI_AP_STA);`**  
   Instruksi utama yang mengonfigurasi *chip* radio ESP agar menjalankan antarmuka ganda (*Dual Interface*) secara simultan, yaitu antarmuka Station (`st_netif`) dan antarmuka SoftAP (`ap_netif`).
4. **`WiFi.begin(sta_ssid, sta_password);`**  
   Memerintahkan antarmuka Station pada ESP untuk memulai *handshake* dan mengaitkan diri ke router WiFi rumah.
5. **`while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }`**  
   Menahan alur eksekusi sementara sampai ESP mendapatkan konfirmasi asosiasi dan alokasi alamat IP dinamis dari DHCP router rumah.
6. **`Serial.println(WiFi.localIP());`**  
   Mencetak alamat IP lokal yang diberikan oleh DHCP router eksternal (contoh: `192.168.1.15` atau `10.224.85.81`).
7. **`WiFi.softAP(ap_ssid, ap_password);`**  
   Mengaktifkan pemancar nirkabel mandiri (*SoftAP*) menggunakan kredensial `ap_ssid` dan `ap_password` yang telah ditentukan, serta menyalakan DHCP Server lokal bawaan ESP.
8. **`Serial.println(WiFi.softAPIP());`**  
   Mengambil dan mencetak alamat IP gateway dari jaringan hotspot ESP (secara default bernilai `192.168.4.1`).
9. **`int jumlahClient = WiFi.softAPgetStationNum();`**  
   Di dalam fungsi `loop()`, fungsi ini membaca tabel asosiasi MAC layer untuk menghitung jumlah perangkat luar (*smartphone*/laptop) yang sedang terkoneksi ke hotspot ESP.
10. **`if (WiFi.status() == WL_CONNECTED) { ... } else { ... }`**  
    Memvalidasi apakah sambungan antarmuka Station ke router luar tetap berjalan stabil tanpa terganggu oleh aktivitas hotspot Access Point lokal.

---

### E. Penjelasan Setiap Fungsi yang Digunakan pada Percobaan 2B

| Nama Fungsi | Parameter Masukan | Return Value | Penjelasan Kegunaan Teknis |
|---|---|---|---|
| `WiFi.mode(mode)` | `WIFI_AP_STA` | `bool` | Mengaktifkan mode operasi nirkabel ganda (*Dual Mode: Station + Access Point*). |
| `WiFi.begin(ssid, pass)`| `sta_ssid`, `sta_password` | `wl_status_t` | Memulai proses koneksi antarmuka Station ke router luar. |
| `WiFi.localIP()` | *None* | `IPAddress` | Membaca alamat IP antarmuka Station yang diperoleh dari router luar. |
| `WiFi.softAP(ssid, pass)`| `ap_ssid`, `ap_password` | `bool` | Mengaktifkan dan memancarkan sinyal hotspot nirkabel lokal dengan enkripsi WPA2. |
| `WiFi.softAPIP()` | *None* | `IPAddress` | Mengembalikan alamat IP lokal Access Point ESP yang berfungsi sebagai *gateway* (default `192.168.4.1`). |
| `WiFi.softAPgetStationNum()`| *None* | `uint8_t` / `int` | Memonitor jumlah perangkat klien eksternal yang sedang terhubung ke sinyal SoftAP ESP. |
| `WiFi.status()` | *None* | `wl_status_t` (enum) | Memeriksa status kelangsungan koneksi antarmuka Station terhadap router luar. |

---

### F. Penjelasan Percabangan / Conditional Logic pada Percobaan 2B

1. **Perulangan Validasi Koneksi Station `while (WiFi.status() != WL_CONNECTED)`**:
   * Menjamin bahwa inisialisasi antarmuka Station ke router luar telah selesai sepenuhnya sebelum konfigurasi Access Point diaktifkan dan sebelum program mencetak informasi alamat IP ke Serial Monitor.
2. **Percabangan Status Berkala `if (WiFi.status() == WL_CONNECTED)` di `loop()`**:
   * Berfungsi sebagai pengawas (*watchdog status*) berkala setiap 5 detik untuk memisahkan logika monitoring antara sisi klien (Station) dan sisi server (Access Point).
   * Menunjukkan bahwa hilangnya koneksi ke router luar tidak mematikan hotspot lokal ESP, sehingga pengguna lokal tetap dapat berkomunikasi dengan ESP melalui antarmuka Access Point.
