import librosa
import librosa.display
import numpy as np
import matplotlib.pyplot as plt

# --- NASTAVENÍ ---
path = 'C:\\Users\\vendy\\Desktop\\19452300.wav'
L_max = 78  # Kalibrační konstanta (0 dBFS = 120 dB SPL)
start_sec = 35
end_sec = 85
duration = end_sec - start_sec

print(f"1. Načítám audio od {start_sec}s do {end_sec}s...")
y, sr = librosa.load(path, sr=None, offset=start_sec, duration=duration)

# 2. Výpočet dB SPL pro časový průběh
y_dbfs = librosa.amplitude_to_db(np.abs(y) + 1e-9, ref=1.0)
y_spl_waveform = y_dbfs + L_max

# --- VÝPOČET PRŮMĚRNÉ HLADINY (Leq) ---
# Decibely nelze průměrovat aritmeticky, musíme přes energii
energy = 10**(y_spl_waveform / 10)
avg_spl = 10 * np.log10(np.mean(energy))

print("-" * 30)
print(f"Průměrná hladina v intervalu {start_sec}-{end_sec}s: {avg_spl:.2f} dB SPL")
print("-" * 30)

# 3. Spektrogram
print("2. Počítám spektrogram...")
n_fft = 2048
hop_length = 512
D = librosa.stft(y, n_fft=n_fft, hop_length=hop_length)
S_dbfs = librosa.amplitude_to_db(np.abs(D), ref=1.0)
S_spl = S_dbfs + L_max

# --- VIZUALIZACE ---
print("3. Vykresluji...")
plt.figure(figsize=(15, 7))

# Graf 1: Časový průběh
plt.subplot(1, 2, 1)
times = np.linspace(start_sec, end_sec, num=len(y))
plt.plot(times, y_spl_waveform, color='firebrick', linewidth=0.5)
# Přidání čáry pro průměr
plt.axhline(y=avg_spl, color='blue', linestyle='--', label=f'Průměr: {avg_spl:.1f} dB')
plt.title(f'Hladina akustického tlaku (Výřez {start_sec}-{end_sec}s)')
plt.ylabel('dB SPL')
plt.xlabel('Čas [s]')
plt.legend()
plt.grid(True)
plt.ylim(30, 90)

# Graf 2: Spektrogram
plt.subplot(1, 2, 2)
img = librosa.display.specshow(S_spl, sr=sr, hop_length=hop_length, 
                               x_axis='time', y_axis='hz', cmap='magma',
                               x_coords=np.linspace(start_sec, end_sec, S_spl.shape[1]))
plt.title('Spektrogram (dB SPL)')
plt.ylim(0, 5000)
plt.colorbar(img, format='%+2.0f dB')

plt.tight_layout()
plt.show()