
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <string.h>
#include <time.h>

#define PI 3.14159265358979323846

// Complex number type
typedef double complex Complex;

// Function prototypes
void fft_cooley_tukey(Complex* x, int n);
void fft_recursive(Complex* x, int n);
void bit_reverse(Complex* x, int n);
int reverse_bits(int num, int log2n);
void butterfly(Complex* x, int n);
Complex* generate_test_signal(int n, int type);
void print_complex_array(Complex* arr, int n, const char* title);
void verify_fft(int n);

// Bit reversal for in-place FFT
int reverse_bits(int num, int log2n) {
    int result = 0;
    for (int i = 0; i < log2n; i++) {
        result = (result << 1) | (num & 1);
        num >>= 1;
    }
    return result;
}

void bit_reverse(Complex* x, int n) {
    int log2n = 0;
    int temp = n;
    while (temp > 1) {
        log2n++;
        temp >>= 1;
    }
    
    for (int i = 0; i < n; i++) {
        int j = reverse_bits(i, log2n);
        if (i < j) {
            Complex temp = x[i];
            x[i] = x[j];
            x[j] = temp;
        }
    }
}

// Cooley-Tukey (iterative implementation)
void fft_cooley_tukey(Complex* x, int n) {
    // Bit reversal
    bit_reverse(x, n);
    
    // FFT working
    for (int len = 2; len <= n; len <<= 1) {
        double angle = -2 * PI / len;
        Complex wlen = cos(angle) + I * sin(angle);
        
        for (int i = 0; i < n; i += len) {
            Complex w = 1;
            for (int j = 0; j < len / 2; j++) {
                Complex u = x[i + j];
                Complex v = x[i + j + len/2] * w;
                x[i + j] = u + v;
                x[i + j + len/2] = u - v;
                w *= wlen;
            }
        }
    }
}

// Recursive FFT implementation (for comparison)
void fft_recursive(Complex* x, int n) {
    if (n <= 1) return;
    
    // Divide
    Complex* even = (Complex*)malloc(n/2 * sizeof(Complex));
    Complex* odd = (Complex*)malloc(n/2 * sizeof(Complex));
    
    for (int i = 0; i < n/2; i++) {
        even[i] = x[2*i];
        odd[i] = x[2*i + 1];
    }
    
    // Conquer
    fft_recursive(even, n/2);
    fft_recursive(odd, n/2);
    
    // Combine
    for (int k = 0; k < n/2; k++) {
        double angle = -2 * PI * k / n;
        Complex w = cos(angle) + I * sin(angle);
        Complex t = w * odd[k];
        x[k] = even[k] + t;
        x[k + n/2] = even[k] - t;
    }
    
    free(even);
    free(odd);
}

// Generate test signals
Complex* generate_test_signal(int n, int type) {
    Complex* signal = (Complex*)malloc(n * sizeof(Complex));
    
    switch(type) {
        case 0: // Impulse
            for (int i = 0; i < n; i++) {
                signal[i] = (i == 0) ? 1.0 : 0.0;
            }
            break;
            
        case 1: // Sine wave
            for (int i = 0; i < n; i++) {
                signal[i] = sin(2 * PI * i / n);
            }
            break;
            
        case 2: // Cosine wave
            for (int i = 0; i < n; i++) {
                signal[i] = cos(2 * PI * i / n);
            }
            break;
            
        case 3: // Complex exponential
            for (int i = 0; i < n; i++) {
                double angle = 2 * PI * i / n;
                signal[i] = cos(angle) + I * sin(angle);
            }
            break;
            
        case 4: // Random
            srand(time(NULL));
            for (int i = 0; i < n; i++) {
                double real = (double)rand() / RAND_MAX - 0.5;
                double imag = (double)rand() / RAND_MAX - 0.5;
                signal[i] = real + I * imag;
            }
            break;
    }
    
    return signal;
}

// Print complex array
void print_complex_array(Complex* arr, int n, const char* title) {
    printf("\n%s:\n", title);
    int limit = (n <= 8) ? n : 8;
    for (int i = 0; i < limit; i++) {
        printf("[%d] = %.4f + %.4fi\n", i, creal(arr[i]), cimag(arr[i]));
    }
    if (n > 8) printf("... (%d more elements)\n", n - 8);
}

// Verify FFT correctness
void verify_fft(int n) {
    printf("\n=== Verifying %d-point FFT ===\n", n);
    
    // Generate test signal (sine wave)
    Complex* signal1 = generate_test_signal(n, 1);
    Complex* signal2 = (Complex*)malloc(n * sizeof(Complex));
    memcpy(signal2, signal1, n * sizeof(Complex));
    
    print_complex_array(signal1, n, "Input Signal (Sine Wave)");
    
    // Apply FFT
    clock_t start = clock();
    fft_cooley_tukey(signal1, n);
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000;
    
    print_complex_array(signal1, n, "FFT Output");
    printf("Time taken: %.2f microseconds\n", time_taken);
    
    // Verify: for sine wave, should have peaks at frequency bins 1 and n-1
    double magnitude1 = cabs(signal1[1]);
    double magnitude_n1 = cabs(signal1[n-1]);
    printf("\nFrequency Analysis:\n");
    printf("Magnitude at bin 1: %.4f\n", magnitude1);
    printf("Magnitude at bin %d: %.4f\n", n-1, magnitude_n1);
    
    // Check energy is concentrated at expected frequencies
    double total_energy = 0;
    double peak_energy = magnitude1 * magnitude1 + magnitude_n1 * magnitude_n1;
    for (int i = 0; i < n; i++) {
        total_energy += cabs(signal1[i]) * cabs(signal1[i]);
    }
    
    printf("Energy concentration: %.2f%%\n", (peak_energy / total_energy) * 100);
    
    free(signal1);
    free(signal2);
}

int main() {
    printf("1D FFT Implementation using Cooley-Tukey Algorithm\n");
    printf("==================================================\n");
    
    // Test with different sizes
    int test_sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024};
    int num_tests = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    for (int i = 0; i < num_tests; i++) {
        verify_fft(test_sizes[i]);
    }
    
    // Detailed example with N=16
    printf("\n\n=== Detailed Example: 16-point FFT ===\n");
    int n = 16;
    
    // Test different signal types
    const char* signal_names[] = {"Impulse", "Sine Wave", "Cosine Wave", "Complex Exponential", "Random"};
    
    for (int type = 0; type < 5; type++) {
        printf("\n--- %s Signal ---\n", signal_names[type]);
        Complex* signal = generate_test_signal(n, type);
        
        if (type < 3) {  // Only print for simpler signals
            print_complex_array(signal, n, "Input");
        }
        
        fft_cooley_tukey(signal, n);
        
        if (type < 3) {
            print_complex_array(signal, n, "FFT Output");
        }
        
        free(signal);
    }
    
    return 0;
}
