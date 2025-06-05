#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <string.h>
#include <time.h>

#define PI 3.14159265358979323846

// External assembly function
extern void fft_vector(double* data, int n);

// Helper functions
void generate_sine_wave(double* data, int n) {
    for (int i = 0; i < n; i++) {
        data[2*i] = sin(2 * PI * i / n);     // real part
        data[2*i + 1] = 0.0;                 // imaginary part
    }
}

void print_complex_data(double* data, int n, const char* title) {
    printf("\n%s:\n", title);
    int limit = (n <= 8) ? n : 8;
    for (int i = 0; i < limit; i++) {
        printf("[%d] = %.4f + %.4fi\n", i, data[2*i], data[2*i+1]);
    }
    if (n > 8) printf("... (%d more elements)\n", n - 8);
}

int main() {
    printf("RISC-V Vector FFT Test Program\n");
    printf("==============================\n\n");
    
    // Test sizes
    int sizes[] = {8, 16, 32, 64};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        printf("Testing %d-point FFT:\n", n);
        
        // Allocate aligned memory for complex data (interleaved real/imag)
        double* data = (double*)aligned_alloc(16, 2 * n * sizeof(double));
        
        // Generate test signal
        generate_sine_wave(data, n);
        print_complex_data(data, n, "Input Signal");
        
        // Call assembly FFT
        clock_t start = clock();
        fft_vector(data, n);
        clock_t end = clock();
        
        double time_ms = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
        
        print_complex_data(data, n, "FFT Output");
        printf("Time: %.3f ms\n", time_ms);
        
        // Verify: Check magnitude at frequency bin 1
        double mag = sqrt(data[2] * data[2] + data[3] * data[3]);
        printf("Magnitude at bin 1: %.4f (expected: %.4f)\n", mag, n/2.0);
        
        free(data);
        printf("\n");
    }
    
    return 0;
}
