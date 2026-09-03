# Jawaban Pertanyaan Praktikum Modul 1

## Percobaan 1A: Akuisisi Data Sensor DHT11

**1.Gambarkan diagram alur (flowchart) proses akuisisi data sensor DHT22 pada program di atas!**

![diagram](../Pertemuan%201/dokumentasi/diagram.png)

**2. Apa fungsi dari perintah `isnan()` pada program tersebut?**
Perintah `isnan()` (Is Not a Number) berfungsi sebagai validasi untuk memeriksa apakah nilai yang dikembalikan oleh fungsi pembacaan sensor (`dht.readHumidity()` atau `dht.readTemperature()`) adalah angka yang valid atau tidak. Jika sensor gagal membaca data (misalnya karena kabel terlepas atau sensor rusak), fungsi akan mengembalikan nilai NaN, dan `isnan()` akan bernilai *true*, sehingga program dapat mendeteksi kegagalan tersebut dan mencegah program menampilkan nilai yang salah atau *error*.

**3. Jelaskan mengapa diperlukan jeda (delay) minimal sekitar 2 detik antar pembacaan sensor DHT11!**
Jeda waktu minimal 2 detik (2000 ms) diperlukan karena limitasi perangkat keras dari sensor DHT11 itu sendiri. Sensor ini memerlukan waktu untuk melakukan *sampling* (pengambilan sampel) perubahan suhu dan kelembaban di lingkungan fisiknya, memprosesnya, dan menyiapkan data digital untuk dikirimkan ke mikrokontroler ESP32. Jika mikrokontroler membaca data lebih cepat dari kecepatan *sampling* sensor (kurang dari 2 detik), sensor tidak akan bisa memberikan data terbaru, yang mengakibatkan pembacaan gagal (*error*) atau hanya mengembalikan nilai pembacaan terakhir yang tersimpan di memori sensor.

**4. Modifikasi program agar data suhu dan kelembaban dirata-ratakan dari 5 kali pembacaan sebelum ditampilkan, dan berikan penjelasan di setiap baris kode yang ditambahkan.**
Berikut adalah bagian *loop()* yang dimodifikasi untuk mencari rata-rata dari 5 kali pembacaan:

```cpp
void loop() {
  // 1. Inisialisasi variabel untuk menampung total jumlah suhu dan kelembaban
  float totalSuhu = 0;
  float totalKelembaban = 0;
  
  // 2. Inisialisasi variabel untuk menghitung berapa kali sensor sukses membaca data
  int pembacaanBerhasil = 0;

  // 3. Memulai perulangan (looping) sebanyak 5 kali pengambilan data
  for (int i = 0; i < 5; i++) {
    // 4. Membaca data kelembaban dan suhu dari sensor
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();
    
    // 5. Validasi: Memeriksa apakah kelembaban DAN suhu berhasil terbaca (bukan NaN)
    if (!isnan(kelembaban) && !isnan(suhu)) {
      // 6. Jika berhasil, tambahkan nilai bacaan ke total akumulasi
      totalKelembaban += kelembaban;
      totalSuhu += suhu;
      
      // 7. Tambah (increment) jumlah penghitung sukses
      pembacaanBerhasil++;
    } else {
      // 8. Tampilkan pesan jika terjadi gagal baca pada salah satu siklus
      Serial.println("Gagal membaca sampel data dari sensor!");
    }
    
    // 9. Berikan jeda 2 detik setiap kali selesai membaca data dari sensor
    delay(2000); 
  }

  // 10. Mengecek apakah minimal ada 1 data yang berhasil dibaca agar tidak terjadi pembagian dengan nol
  if (pembacaanBerhasil > 0) {
    // 11. Menghitung rata-rata dengan membagi total nilai dengan jumlah data yang sukses dibaca
    float rataSuhu = totalSuhu / pembacaanBerhasil;
    float rataKelembaban = totalKelembaban / pembacaanBerhasil;
    
    // 12. Menampilkan hasil rata-rata ke Serial Monitor
    Serial.print("Rata-rata Suhu: "); 
    Serial.print(rataSuhu); 
    Serial.println(" °C");
    Serial.print("Rata-rata Kelembaban: "); 
    Serial.print(rataKelembaban); 
    Serial.println(" %");
  } else {
    // 13. Pesan jika kelima perulangan gagal dibaca oleh sensor
    Serial.println("Gagal total menghitung rata-rata.");
  }
}
```

---

## Percobaan 2A: Kendali Aktuator Relay Berdasarkan Data Sensor

**1. Mengapa diperlukan nilai ambang batas (threshold) dalam sistem kendali aktuator berbasis sensor?**
Nilai ambang batas (*threshold*) berfungsi sebagai titik acuan (referensi) atau aturan pengambilan keputusan (logika IF) bagi mikrokontroler. Dengan adanya nilai ambang batas, sistem dapat memutuskan kapan harus mengirimkan sinyal perintah (ON/OFF) kepada aktuator berdasarkan data fisik lingkungan (seperti suhu) yang ditangkap oleh sensor. Tanpa nilai ini, mikrokontroler tidak tahu pada kondisi suhu berapa ia harus mengambil tindakan untuk mengubah status dari aktuator.

**2. Jelaskan apa yang akan terjadi apabila nilai suhu Threshold diturunkan menjadi sangat rendah, misalnya 20.0!**
Jika *threshold* diatur ke angka 20.0 °C, aktuator (seperti lampu, kipas, atau pompa) kemungkinan besar akan menyala secara terus-menerus tanpa pernah mati. Hal ini dikarenakan suhu ruangan normal di lingkungan sekitar pada umumnya (terutama di wilayah tropis seperti Indonesia) hampir selalu berada di atas 20.0 °C. Akibatnya, kondisi logika `suhu > suhuThreshold` akan selalu terpenuhi (`true`).

**3. Apa perbedaan antara kendali aktuator secara terus-menerus (kondisi tunggal) dengan kendali menggunakan histerisis (dua ambang batas)?**
*   **Kendali Tunggal:** Hanya menggunakan satu nilai acuan ukur. Jika suhu persis berada di angka ambang batas dan berfluktuasi sedikit saja (naik-turun tipis), aktuator akan terus menerus berganti status (mati-nyala-mati-nyala) dengan sangat cepat atau *flickering*. Ini dapat merusak perangkat keras aktuator (misalnya membuat relay berderik).
*   **Kendali Histerisis:** Menggunakan rentang toleransi dengan menetapkan dua ambang batas (batas atas dan batas bawah). Aktuator baru menyala saat suhu melewati batas atas, dan tidak akan langsung mati saat suhu turun sedikit, melainkan menunggu hingga suhu menembus batas bawah. Hal ini menstabilkan sistem dan memperpanjang umur komponen elektronika.

**4. Modifikasi program agar menggunakan dua ambang batas (histerisis), misalnya aktuator menyala pada suhu di atas 30°C dan baru mati pada suhu di bawah 28°C, dan berikan penjelasan di setiap baris kode nya.**
Berikut adalah bagian *loop()* yang dimodifikasi beserta variabel pendukungnya:

```cpp
// 1. Mendefinisikan dua variabel konstanta (histerisis) untuk batas atas dan bawah
const float suhuAtas = 30.0;
const float suhuBawah = 28.0;

void loop() {
  // 2. Membaca data suhu dari sensor
  float suhu = dht.readTemperature();

  // 3. Mengecek apakah sensor berhasil membaca suhu
  if (isnan(suhu)) {
    Serial.println("Gagal membaca data sensor!");
  } else {
    // 4. Menampilkan suhu saat ini di Serial Monitor
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");

    // 5. Logika Histerisis batas atas: Jika suhu lebih besar dari 30.0
    if (suhu > suhuAtas) {
      // 6. Memberi tegangan HIGH untuk mengaktifkan Relay/LED
      digitalWrite(RELAYPIN, HIGH); 
      Serial.println("Aktuator: ON");
    } 
    // 7. Logika Histerisis batas bawah: Jika suhu kurang dari 28.0
    else if (suhu < suhuBawah) {
      // 8. Menurunkan tegangan ke LOW untuk mematikan Relay/LED
      digitalWrite(RELAYPIN, LOW);
      Serial.println("Aktuator: OFF");
    }
    // 9. Kondisi jika suhu di antara 28.0 dan 30.0
    // Pada rentang ini (contoh: 29.5), tidak ada instruksi perintah perubahan.
    // Artinya mikrokontroler akan menahan (holding) perintah 'digitalWrite' yang 
    // terakhir kali dieksekusi, sehingga status aktuator tetap stabil.
  }
  
  // 10. Jeda 2 detik sebelum perulangan berikutnya
  delay(2000); 
}
```