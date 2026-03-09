#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

constexpr size_t SIZE = 100000000;

void multiply_without_openmp(std::vector<double>& a, std::vector<double>& b, std::vector<double>& c) {
    for (size_t i = 0; i < SIZE; ++i) {
        c[i] = a[i] * b[i];
    }
}

void multiply_with_openmp(std::vector<double>& a, std::vector<double>& b, std::vector<double>& c) {
    #pragma omp parallel for
    for (size_t i = 0; i < SIZE; ++i) {
        c[i] = a[i] * b[i];
    }
}

int main() {
    std::vector<double> a(SIZE, 2.5);
    std::vector<double> b(SIZE, 3.5);
    std::vector<double> c(SIZE);

    std::cout << "Array size: " << SIZE << " elements\n";
    std::cout << "Available threads: " << omp_get_max_threads() << "\n\n";

    // Without OpenMP
    auto start = std::chrono::high_resolution_clock::now();
    multiply_without_openmp(a, b, c);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_no_omp = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Without OpenMP: " << duration_no_omp << " ms\n";

    // With OpenMP
    start = std::chrono::high_resolution_clock::now();
    multiply_with_openmp(a, b, c);
    end = std::chrono::high_resolution_clock::now();
    auto duration_omp = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "With OpenMP:    " << duration_omp << " ms\n";

    // Speedup
    double speedup = static_cast<double>(duration_no_omp) / duration_omp;
    std::cout << "\nSpeedup: " << speedup << "x\n";

    return 0;
}
