"""
Modul 5: Pemrosesan Data Sensor dan Analisis Statistik
Percobaan 5B (Modifikasi): Analisis Z-Score dengan Pra-Pemrosesan Moving Average
Praktikan: Afif Nur Rahman (H1H024016)
Asisten: Athallah Tsany Satriyaji

Deskripsi Modifikasi:
Menerapkan filter Moving Average (jendela 5) pada data runtun waktu sebelum dilakukan
penghitungan Z-score dan deteksi anomali.
Membandingkan efek peredaman fluktuasi sesaat (spike noise) terhadap sensitivitas deteksi anomali.
"""

import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "broker.hivemq.com"
PORT   = 1883
TOPIC  = "unsoed/tk245004/kelompokApipHana/data"

JUMLAH_SAMPEL = 20
data = []

def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

    data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")
    
    if len(data) >= JUMLAH_SAMPEL:
        client.disconnect()

# ====================================================================
# TAHAP 1: AKUISISI DATA DARI MQTT (Bisa membaca langsung file CSV jika sudah ada)
# ====================================================================
try:
    df = pd.read_csv("data_suhu.csv")
    print("Memuat data dari data_suhu.csv yang sudah ada...")
except FileNotFoundError:
    print("Mengakuisisi data baru dari broker MQTT...")
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER, PORT)
    client.loop_forever()

    df = pd.DataFrame(data)
    df.to_csv("data_suhu.csv", index=False)

# ====================================================================
# TAHAP 2: PERHITUNGAN Z-SCORE DATA MENTAH
# ====================================================================
rata2_mentah = df["suhu"].mean()
std_mentah   = df["suhu"].std()
df["zscore_mentah"] = (df["suhu"] - rata2_mentah) / std_mentah if std_mentah != 0 else 0.0
df["anomali_mentah"] = df["zscore_mentah"].abs() > 2

# ====================================================================
# TAHAP 3 (MODIFIKASI): PENERAPAN MOVING AVERAGE (N=5) SEBELUM Z-SCORE
# ====================================================================
# 1. Menghitung rata-rata bergerak dari 5 data berurutan (min_periods=1 agar awal data tidak NaN)
df["suhu_ma"] = df["suhu"].rolling(window=5, min_periods=1).mean()

# 2. Menghitung parameter statistik baru berbasis data terfilter
rata2_ma = df["suhu_ma"].mean()
std_ma   = df["suhu_ma"].std()

# 3. Menghitung Z-score dari data hasil filter Moving Average
df["zscore_ma"] = (df["suhu_ma"] - rata2_ma) / std_ma if std_ma != 0 else 0.0
df["anomali_ma"] = df["zscore_ma"].abs() > 2

# ====================================================================
# TAHAP 4: PERBANDINGAN DAN ANALISIS HASIL
# ====================================================================
print("\n" + "="*50)
print("=== PERBANDINGAN DETEKSI ANOMALI SEBELUM & SESUDAH FILTER MA ===")
print("="*50)
print(f"Rata-rata Suhu Mentah       : {rata2_mentah:.2f} °C")
print(f"Rata-rata Suhu Setelah MA   : {rata2_ma:.2f} °C")
print(f"Nilai Puncak Suhu Mentah    : {df['suhu'].max():.2f} °C")
print(f"Nilai Puncak Suhu Setelah MA: {df['suhu_ma'].max():.2f} °C")
print(f"Jumlah Anomali Data Mentah  : {df['anomali_mentah'].sum()} titik")
print(f"Jumlah Anomali Setelah MA(5): {df['anomali_ma'].sum()} titik")
print("="*50)

# ====================================================================
# TAHAP 5: VISUALISASI PERBANDINGAN GRAFIK
# ====================================================================
plt.figure(figsize=(11, 5))

# Plot kurva data mentah
plt.plot(df.index, df["suhu"], marker="o", linestyle=":", color="lightgray", label="Data Suhu Mentah")

# Plot kurva data hasil filter Moving Average
plt.plot(df.index, df["suhu_ma"], marker="s", linestyle="-", color="#1f77b4", linewidth=2, label="Moving Average (Jendela N=5)")

# Sorot titik anomali data mentah
anom_mentah = df[df["anomali_mentah"]]
plt.scatter(anom_mentah.index, anom_mentah["suhu"], color="red", s=130, marker="o", label="Anomali Mentah (|z| > 2)", zorder=5)

# Sorot titik anomali setelah filter MA (jika ada)
anom_ma = df[df["anomali_ma"]]
if not anom_ma.empty:
    plt.scatter(anom_ma.index, anom_ma["suhu_ma"], color="orange", s=100, marker="^", label="Anomali MA (|z| > 2)", zorder=6)

plt.axhline(rata2_mentah, color="gray", linestyle="--", alpha=0.7, label=f"Rata-rata ({rata2_mentah:.2f} °C)")
plt.xlabel("Urutan Sampel Data")
plt.ylabel("Suhu (°C)")
plt.title("Komparasi Efek Filter Moving Average terhadap Deteksi Anomali Z-Score")
plt.grid(True, linestyle=":", alpha=0.6)
plt.legend(loc="upper right")
plt.tight_layout()
plt.savefig("grafik_perbandingan_ma_zscore.png", dpi=300)
print("\nGrafik perbandingan berhasil disimpan ke 'grafik_perbandingan_ma_zscore.png'")
plt.show()
