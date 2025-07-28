import numpy as np
import scipy.signal as signal
import scipy.fftpack as fft
import matplotlib.pyplot as plt
import wave

# Function to read WAV file
def read_wav(filename):
    with wave.open(filename, 'rb') as wf:
        num_frames = wf.getnframes()
        num_channels = wf.getnchannels()
        sample_width = wf.getsampwidth()
        framerate = wf.getframerate()
        
        data = np.frombuffer(wf.readframes(num_frames), dtype=np.int16)
        if num_channels == 2:
            data = data[::2]  # Convert stereo to mono
            
        return data, framerate

# Function to apply a bandpass filter
def apply_filter(data, sample_rate, low_cutoff=500, high_cutoff=5000, filter_order=4):
    nyquist = 0.5 * sample_rate
    low = low_cutoff / nyquist
    high = high_cutoff / nyquist
    b, a = signal.butter(filter_order, [low, high], btype='band')
    return signal.filtfilt(b, a, data)

# Function to compute FFT
def compute_fft(data, sample_rate):
    N = len(data)
    freqs = np.fft.rfftfreq(N, d=1/sample_rate)
    fft_data = np.abs(fft.rfft(data))  # Take magnitude of FFT
    return freqs, fft_data

# Function to plot data
def plot_waveform_and_spectrum(original, filtered, sample_rate):
    time_axis = np.linspace(0, len(original) / sample_rate, num=len(original))
    
    # Compute FFT for both signals
    freqs, original_fft = compute_fft(original, sample_rate)
    _, filtered_fft = compute_fft(filtered, sample_rate)

    plt.figure(figsize=(12, 6))

    # Plot Time Domain
    plt.subplot(2, 1, 1)
    plt.plot(time_axis, original, label="Original", alpha=0.7)
    plt.plot(time_axis, filtered, label="Filtered", alpha=0.7)
    plt.title("Time Domain Waveform")
    plt.xlabel("Time (s)")
    plt.ylabel("Amplitude")
    plt.legend()

    # Plot Frequency Domain (Log Scale for better visibility)
    plt.subplot(2, 1, 2)
    plt.semilogy(freqs, original_fft, label="Original Spectrum", alpha=0.7)
    plt.semilogy(freqs, filtered_fft, label="Filtered Spectrum", alpha=0.7, linestyle="--")
    plt.title("Frequency Spectrum")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude (dB)")
    plt.legend()

    plt.tight_layout()
    plt.show()

# Load and process the WAV file
wav_file = "songs/ShapeOfYou.wav"  # Change this to your file
original_data, sample_rate = read_wav(wav_file)
filtered_data = apply_filter(original_data, sample_rate)

# Plot the results
plot_waveform_and_spectrum(original_data, filtered_data, sample_rate)
