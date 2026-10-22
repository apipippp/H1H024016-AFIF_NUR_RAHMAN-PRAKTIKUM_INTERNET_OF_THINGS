"""
Modul 5: Pemrosesan Data Sensor dan Analisis Statistik
Percobaan 5B: Program Python Pengolah, Analisis Z-Score, dan Visualisasi Data Suhu
Praktikan: Afif Nur Rahman (H1H024016)
Asisten: Athallah Tsany Satriyaji

Alur 4 Tahapan:
1. Akuisisi Data: Menerima 20 sampel data streaming via protokol MQTT dari broker HiveMQ.
2. Penyimpanan CSV: Menyimpan data runtun waktu ke dalam file tabular "data_suhu.csv".
3. Analisis Statistik: Menghitung mean, standar deviasi, dan mendeteksi anomali berbasis Z-score (|z| > 2).
4. Visualisasi Data: Membuat kurva runtun waktu dan menonjolkan titik anomali menggunakan Matplotlib.
"""

import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

# Konfigurasi koneksi broker MQTT
BROKER = "broker.hivemq.com"
PORT   = 1883
TOPIC  = "unsoed/tk245004/kelompokApipHana/data"

# Jumlah sampel akuisisi data yang ditargetkan
JUMLAH_SAMPEL = 20
data = []

def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker MQTT, kode:", reason_code)
    client.subscribe(TOPIC)
    print(f"Berhasil subscribe ke topic: {TOPIC}")
    print("Menunggu data masuk...")

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

    # Menyimpan stempel waktu lokal dan nilai suhu
    waktu_sekarang = time.strftime("%Y-%m-%d %H:%M:%S")
    data.append({"waktu": waktu_sekarang, "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] {waktu_sekarang} | suhu = {suhu:.2f} °C")

    # Putus koneksi secara otomatis setelah 20 sampel terkumpul
    if len(data) >= JUMLAH_SAMPEL:
        print("\nTarget 20 sampel tercapai. Memutus koneksi dari broker...")
        client.disconnect()

# ====================================================================
# TAHAP 1: AKUISISI DATA DARI BROKER MQTT
# ====================================================================
print("--- TAHAP 1: MEMULAI AKUISISI DATA STREAMING MQTT ---")
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# ====================================================================
# TAHAP 2: EKSPOR DAN PENYIMPANAN DATA KE FILE CSV
# ====================================================================
print("\n--- TAHAP 2: PENYIMPANAN DATA HISTORIS KE CSV ---")
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data berhasil diarsipkan ke file 'data_suhu.csv'")

# ====================================================================
# TAHAP 3: ANALISIS STATISTIK DAN DETEKSI ANOMALI Z-SCORE
# ====================================================================
print("\n--- TAHAP 3: ANALISIS STATISTIK & DETEKSI ANOMALI Z-SCORE ---")
rata2 = df["suhu"].mean()
std   = df["suhu"].std()

if std == 0:
    df["zscore"] = 0.0
else:
    # Rumus Z-score: z = (x - mean) / std
    df["zscore"] = (df["suhu"] - rata2) / std

# Deteksi anomali dengan ambang batas |z| > 2 (aturan empiris 95.45% sebaran normal)
df["anomali"] = df["zscore"].abs() > 2

print(f"Rata-rata (Mean)    : {rata2:.2f} °C")
print(f"Standar Deviasi     : {std:.2f} °C")
print(f"Nilai Minimum       : {df['suhu'].min():.2f} °C")
print(f"Nilai Maksimum      : {df['suhu'].max():.2f} °C")
print(f"Jumlah Titik Anomali: {df['anomali'].sum()}")

# Tampilkan ringkasan data
print("\nRingkasan Tabel Data:")
print(df[["waktu", "suhu", "zscore", "anomali"]])

# ====================================================================
# TAHAP 4: VISUALISASI DATA DENGAN MATPLOTLIB
# ====================================================================
print("\n--- TAHAP 4: MEMBUAT VISUALISASI GRAFIK RUNTUN WAKTU ---")
plt.figure(figsize=(10, 4.5))

# Plot kurva runtun waktu data suhu
plt.plot(df.index, df["suhu"], marker="o", linestyle="-", color="#1f77b4", label="Data Suhu (°C)")

# Sorot titik data anomali dengan scatter plot lingkaran merah
anomali_df = df[df["anomali"]]
plt.scatter(anomali_df.index, anomali_df["suhu"], color="red", s=120, edgecolors="black", linewidth=1.5, label="Anomali (|z| > 2)", zorder=4)

# Garis referensi rata-rata (mean)
plt.axhline(rata2, color="gray", linestyle="--", linewidth=1.2, label=f"Rata-rata ({rata2:.2f} °C)")

plt.xlabel("Urutan Sampel Data")
plt.ylabel("Suhu (°C)")
plt.title("Visualisasi Runtun Waktu Suhu dan Deteksi Anomali Z-Score")
plt.grid(True, linestyle=":", alpha=0.6)
plt.legend(loc="upper right")
plt.tight_layout()

# Simpan grafik ke file gambar
plt.savefig("grafik_suhu.png", dpi=300)
print("Grafik berhasil disimpan ke 'grafik_suhu.png'")
plt.show()
