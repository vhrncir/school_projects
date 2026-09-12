import cv2
from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

def capture_image_live(save_path="captured_image.jpg"):
    x=350
    y=350
    width= 420
    height= 30
    cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)  # Otevře kameru (0 = vestavěná, 1 = externí)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

    if not cap.isOpened():
        print("Chyba: Kamera nebyla detekována.")
        return None

    while True:
        ret, frame = cap.read()  # Načte snímek z kamery
        if not ret:
            print("Chyba: Nelze načíst snímek.")
            break

        cv2.rectangle(frame, (x, y), (x + width, y + height), (255, 0, 0), 2)
        cv2.imshow("Živý náhled - Stiskni 's' pro uložení, 'q' pro ukončení", frame)

        key = cv2.waitKey(1) & 0xFF
        if key == ord('s'):  # Stisknutí 's' uloží obrázek
            cropped = frame[y:y + height, x:x + width]
            cv2.imwrite(save_path, cropped)
            print(f"Obrázek uložen jako {save_path}")
            return save_path
        elif key == ord('q'):  # Stisknutí 'q' ukončí program
            print("Ukončuji bez uložení.")
            break

    cap.release()
    cv2.destroyAllWindows()
    return None

def analyze_spectrum(image_path, ref_spectrum_path, lambda_min, lambda_max):
    img = Image.open(image_path).convert("L")
    img_array = np.array(img)

    intensity_profile = np.mean(img_array, axis=0)
    intensity_profile = intensity_profile / np.max(intensity_profile)

    width = img_array.shape[1]
    wavelengths = np.linspace(lambda_min, lambda_max, width)

    ref_img = Image.open(ref_spectrum_path)
    ref_img_array = np.array(ref_img)

    plt.figure(figsize=(10, 5))
    plt.imshow(ref_img_array, aspect='auto', extent=[wavelengths[0], wavelengths[-1], 0, np.max(intensity_profile)],
               alpha=0.2)
    plt.plot(wavelengths, intensity_profile, color='black')
    plt.xlabel('Wavelength (nm)')
    plt.ylabel('Intensity')
    plt.title('Spectrum Analysis')
    plt.grid(True)
    plt.show()

# Pořízení fotografie a analýza spektra
image_path = capture_image_live("moje_fotka.jpg")
if image_path:
    print("calling spectrum analysis")
    analyze_spectrum(image_path, "ref_spectrum.png", lambda_min=350, lambda_max=800)