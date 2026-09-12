# school_projects
## 🛠️ Showcases of school projects
* 💊 **Smart Pill Dispenser** – An embedded hardware project featuring an automated medication dispenser built with Arduino (C++), designed to manage and verify pill dosages using weight and optical sensors. **[Code](https://github.com/vhrncir/school_projects/blob/main/Smart_pill_dispenser.ino)**
<img width="1148" height="293" alt="Smart_pill_dispenser" src="https://github.com/user-attachments/assets/8d94865b-ae3a-42b2-99ce-289294305ded" />

* 🦵 **Edema Sensor** – An end-to-end IoT medical monitoring device designed to detect swelling or edema using a flex sensor, trigger local alarms, and host a wireless web interface for remote caregiver acknowledgment. **[Code-ESP](https://github.com/vhrncir/school_projects/blob/main/Edema_sensor_ESP.ino)** and **[Code-Arduino](https://github.com/vhrncir/school_projects/blob/main/edema_sensor_arduino.ino)**
<p align="center">
  <img width="380" height="570" alt="MĚŘENÍ OTOKU NOHOU" src="https://github.com/user-attachments/assets/a28f6d2f-06c8-46b4-a2c6-eedbf28a665d" style="vertical-align: middle;     margin-right: 10px;" />
  <img width="500" height="208" alt="schema" src="https://github.com/user-attachments/assets/38fa5a65-7643-46d7-9cff-74be4c83b17a" style="vertical-align: middle;" />
</p>

* 🧪 **Potentiostat & Electrochemical Cell Simulation** – Simulation of cyclic voltammetry in NI Multisim, featuring the design of a potentiostat  ([Inspired](https://doi.org/10.1007/978-3-030-69117-2_2)) and a three-electrode electrochemical cell modeled as an equivalent Randles circuit. Parametric analysis of individual component values, scan rate analysis, AC sweep (impedance behavior), and statistical tolerance evaluation via Monte Carlo analysis.
<img width="1000" height="200" alt="cyclic_voltammetry" src="https://github.com/user-attachments/assets/de2eed65-fda5-41d2-8fe5-f1681cbc7469" />

* 💥 **Laser Pointer and Moving Target** – An embedded gaming and target practice system featuring a laser-activated moving target built with Arduino and controlled via multi-modal state logic. Implemented a physical color-wavelength filter over the photodiode to isolate the red laser spectrum from ambient light interference, combined with an automated dynamic threshold calibration during startup. Multiple gameplay modes (Static Target vs. Randomized Servo-Driven Motion via `randomSeed`), real-time hit tracking with an interrupt-like button debouncing debounce cycle, and a success state trigger upon reaching 5 hits.**[Code for the Target](https://github.com/vhrncir/school_projects/blob/main/red_light_target_code.ino)**
<img width="1042" height="607" alt="laser_target" src="https://github.com/user-attachments/assets/a7025ab3-d253-4b9c-9ce4-17f872dabf9f" />

📢 **Noise Level Meter and Flight Noise Analyzer** – An analog-digital sound level meter, featuring a custom-tuned pre-amplifier circuit, hardware low-pass filtering, and an Arduino-based processing unit ([Inspired](https://bestengineeringprojects.com/sound-level-meter-circuit-using-arduino/)). Designed an analog front-end using a BCM9765P-44 condenser microphone with a low-noise amplifier set to a 200x gain. Implemented a hardware passive RC low-pass filter ($f_c \approx 16\text{ kHz}$) based on human ear physiology to eliminate high-frequency noise. Decibel calculation from ADC values using a logarithmic/linear regression method, real-time averaging (1-second intervals) displayed on an I2C LCD panel, and a 3-stage LED visual alert system for noise safety thresholds.

The project was extended to analyze environmental aircraft noise over Prague during flight traffic alterations (the signal was obtained employing the DJI Mic Mini). Conducted time-frequency analysis using Short-Time Fourier Transforms (STFT) to generate spectrograms of aircraft flyovers and calculated the equivalent continuous sound level ($L_{Aeq}$) to quantify the environmental impact of shifting flight paths (Python).

<p align="center">
  <img width="500" height="250" alt="Noisemeter" src="https://github.com/user-attachments/assets/acc3304d-a45b-45d3-846d-b20a1062fc90" style="vertical-align: middle;       margin-right: 10px;" />
  <img width="500" height="250" alt="Noise_analysis" src="https://github.com/user-attachments/assets/c5599369-b433-4815-8d05-bce73f70a147" style="vertical-align: middle;" />
</p>

### 🏥 Regional Healthcare Quality Mapping in the Czech Republic
A statistical data analytics team project focused on identifying key socio-economic and capacity drivers to model regional healthcare quality (mortality and hospitalization rates) across Czech regions using the R language. Performed univariate and multivariate linear regression analysis. Evaluated and validated models against Gauss-Markov assumptions, dealing with a small sample size constraint while optimizing predictive capabilities. Disproved the direct statistical significance of medical staff salaries on regional mortality and hospitalization rates. Developed a highly robust multivariate model for hospitalization rates ($R^2 = 86.5\%$, $p\text{-value} = 0.0006$) driven by regional capacity metrics. Applied data clustering (PCA, k-means) to identify geographic healthcare trends (North vs. South mortality split, and a severe healthcare centralization load in Prague compared to its surrounding regions).

<img width="437" height="263" alt="Regional_Healthcare_Quality_Mapping" src="https://github.com/user-attachments/assets/c473264f-bca3-499a-bc60-e9410a9113fc" />

### 🚶‍♂️ Objective Gait Analysis for Multiple Sclerosis Progression Modeling

A sophisticated biomedical computing and statistical analysis project ($N = 285$) that leverages high-resolution spatio-temporal GAITrite telemetry to objectively quantify and model Multiple Sclerosis (MS) progression against the clinical EDSS scale using MATLAB. Verified data assumptions using Shapiro-Wilk, Bartlett, and Breusch-Pagan tests. Conducted non-parametric group comparisons and post-hoc diagnostics (Kruskal-Wallis, Wilcoxon signed-rank, and Dunn’s test with Bonferroni correction) alongside Spearman rank correlations. Applied analysis of covariance (ANCOVA) to eliminate age as a confounding covariate. Implemented LASSO (L1-regularized) regression with 10-fold cross-validation for strict parameter selection, and combined Principal Component Analysis (PCA) with k-means clustering ($k=7$) to isolate discrete physiological phenotypes. Developed a piecewise (segmented) regression workflow with 10-fold cross-validation to locate the critical inflection point of mobility degradation, evaluating structural changes via acceleration coefficients. Utilized ROC analysis (Youden's Index / AUC) to define non-linear classification thresholds.

<img width="437" height="300" alt="multiple_sclerosis" src="https://github.com/user-attachments/assets/1bf989c4-93d5-4d87-b29b-68e3ef432801" />

