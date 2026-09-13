import time
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import serial

# --- CONFIGURATION ---
PORT = "COM3"
BAUD = 9600

print("Select test type:")
print("1 - Standard test (Arduino script: random positions, extremes, disconnect, straight)")
print("2 - Hysteresis test (gradual back and forth sweep for material memory analysis)")
mode = input("Enter mode number (1 or 2): ").strip()

ser = serial.Serial(PORT, BAUD)
time.sleep(2)

positions = []
ohms_list = []

print(
    f"\nDATA COLLECTION (Mode {mode}): Listening to serial port... (stop with"
    " Ctrl+C)"
)

# --- REAL TIME MONITORING ---
plt.ion()
fig, ax = plt.subplots(figsize=(10, 5))
(line,) = ax.plot([], [], "ro-", markersize=3, label="Live data (Sensor)")
ax.set_xlim(0, 600)
ax.set_ylim(0, 30000)
ax.set_xlabel("Motor position (steps)")
ax.set_ylabel("Resistance (Ω)")
ax.set_title(f"Real-time data collection - Mode {mode}")
ax.legend()
ax.grid(True)

try:
    while True:
        if ser.in_waiting > 0:
            line_data = ser.readline().decode("utf-8", errors="ignore").strip()
            parts = line_data.split("\t")

            if len(parts) >= 2:
                try:
                    pos = float(parts[0])
                    r = float(parts[1])

                    positions.append(pos)
                    ohms_list.append(r)

                    line.set_xdata(positions)
                    line.set_ydata(ohms_list)
                    plt.draw()
                    plt.pause(0.001)
                except ValueError:
                    pass

except KeyboardInterrupt:
    print("\nData collection ended. Running analysis based on selected mode...")
    ser.close()

# --- ANALYSIS ---
if len(positions) > 10:
    df = pd.DataFrame({"Position": positions, "Resistance": ohms_list})

    # Smoothing noise with a moving average
    window_size = 5
    df["Filtered_Resistance"] = (
        df["Resistance"].rolling(window=window_size, center=True).mean()
    )
    clean_df = df.dropna()

    plt.ioff()
    plt.figure(figsize=(10, 6))

    if mode == "1":
        # ANALYSIS FOR MODE 1: Standard Test (Regression curve and stability)
        coeffs = np.polyfit(clean_df["Position"], clean_df["Filtered_Resistance"], 2)
        poly_func = np.poly1d(coeffs)
        x_smooth = np.linspace(
            clean_df["Position"].min(), clean_df["Position"].max(), 200
        )
        y_smooth = poly_func(x_smooth)

        std_dev = np.std(clean_df["Resistance"])
        print(f"\n--- STANDARD TEST RESULTS ---")
        print(f"Total collected samples: {len(df)}")
        print(f"Resistance standard deviation: {std_dev:.2f} Ω")
        print(
            f"Regression equation: y = {coeffs[0]:.4f}x^2 + {coeffs[1]:.4f}x +"
            f" {coeffs[2]:.2f}"
        )

        plt.scatter(
            df["Position"],
            df["Resistance"],
            color="gray",
            alpha=0.6,
            label="Raw data",
        )
        plt.plot(
            df["Position"],
            df["Filtered_Resistance"],
            color="red",
            linewidth=2,
            label="Moving average",
        )
        plt.plot(
            x_smooth,
            y_smooth,
            color="blue",
            linestyle="--",
            linewidth=2,
            label="Polynomial regression",
        )
        plt.title("Standard Flex Sensor Test Analysis")

    elif mode == "2":
        # ANALYSIS FOR MODE 2: Hysteresis (Split into forward and backward sweep)
        half = len(clean_df) // 2
        phase_forward = clean_df.iloc[:half]
        phase_backward = clean_df.iloc[half:]

        # Maximum hysteresis deviation calculation
        max_hysteresis = abs(
            phase_forward["Filtered_Resistance"].values[: len(phase_backward)]
            - phase_backward["Filtered_Resistance"].values[: len(phase_forward)]
        ).max()

        print(f"\n--- HYSTERESIS TEST RESULTS ---")
        print(
            f"Max resistance difference between forward and backward bend:"
            f" {max_hysteresis:.2f} Ω"
        )

        plt.plot(
            phase_forward["Position"],
            phase_forward["Filtered_Resistance"],
            color="blue",
            linewidth=2,
            label="Forward Direction (Bending)",
        )
        plt.plot(
            phase_backward["Position"],
            phase_backward["Filtered_Resistance"],
            color="orange",
            linewidth=2,
            label="Backward Direction (Straightening)",
        )
        plt.title("Flex Sensor Hysteresis Analysis (Material Memory)")
    else:
        print("Invalid mode selection.")
        exit()

    plt.xlabel("Motor Position (steps)")
    plt.ylabel("Sensor Resistance (Ω)")
    plt.legend()
    plt.grid(True)
    plt.show()
else:
    print("Insufficient data for analysis.")