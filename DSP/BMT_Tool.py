import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt

def calculate_biquad_coefficients(cutoff_freq, sample_rate, num_stages=1):
    # Normalize the cutoff frequency
    nyquist_freq = 0.5 * sample_rate
    norm_cutoff_freq = cutoff_freq / nyquist_freq

    # Design a low-pass filter using the bilinear transform method
    b, a = signal.butter(2, norm_cutoff_freq, btype='low', analog=False)

    # Convert the filter coefficients to the biquad format
    biquad_coeffs = []
    for i in range(num_stages):
        biquad_coeffs.extend([b[0], b[1], b[2], -a[1], -a[2]])

    return b, a, biquad_coeffs

def print_biquad_coefficients(cutoff_freq, sample_rate, num_stages=1):
    b, a, coeffs = calculate_biquad_coefficients(cutoff_freq, sample_rate, num_stages)

    print(f"arm_biquad_casd_df1_inst_f32 S;  // Filter instance")
    print(f"float32_t biquadCoeffs[{5 * num_stages}] = {{")
    for coeff in coeffs:
        print(f"    {coeff:.8f},")
    print("};  // Coefficients for a low-pass filter")

    return b, a

def plot_frequency_response(b, a, sample_rate):
    # Calculate the frequency response
    w, h = signal.freqz(b, a, worN=8000)
    frequencies = (w / np.pi) * (sample_rate / 2)  # Convert to Hz

    # Plot the frequency response
    plt.figure(figsize=(10, 6))
    plt.plot(frequencies, 20 * np.log10(abs(h)), color='blue')
    plt.title('Frequency Response of the Biquad Filter')
    plt.xlabel('Frequency [Hz]')
    plt.ylabel('Magnitude [dB]')
    plt.grid(True)
    plt.axvline(cutoff_freq, color='red', linestyle='--', label=f'Cutoff Frequency: {cutoff_freq} Hz')
    plt.legend()
    plt.xlim(0, sample_rate / 2)
    plt.ylim(-60, 5)
    plt.show()

# Example usage
cutoff_freq = 200  # Cutoff frequency in Hz
sample_rate = 44100  # Sample rate in Hz
num_stages = 1       # Number of biquad stages

# Calculate coefficients and print them
b, a = print_biquad_coefficients(cutoff_freq, sample_rate, num_stages)

# Plot the frequency response
plot_frequency_response(b, a, sample_rate)