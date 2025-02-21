#include "populate_coordinates_avx2.h"

#include <immintrin.h>

void populate_coordinates_avx2(float* cx, float* cy, int w, int h, float x_lo, float x_hi, float y_lo, float y_hi) {
    // Precompute scale factors and offsets
    float x_scale = (x_hi - x_lo) / (float)w;
    float y_scale = (y_hi - y_lo) / (float)h;

    // Broadcast scale and offset values into AVX2 registers
    __m256 x_scale_vec = _mm256_set1_ps(x_scale);
    __m256 y_scale_vec = _mm256_set1_ps(y_scale);
    __m256 x_lo_vec = _mm256_set1_ps(x_lo);
    __m256 y_lo_vec = _mm256_set1_ps(y_lo);

    for (int j = 0; j < h; ++j) {
        // Broadcast the current row index
        __m256 y_index_vec = _mm256_set1_ps((float)j);

        for (int i = 0; i < w; i += 8) {
            // Load and convert column indices to float
            __m256i idx = _mm256_set_epi32(i + 7, i + 6, i + 5, i + 4, i + 3, i + 2, i + 1, i);
            __m256 x_index_vec = _mm256_cvtepi32_ps(idx);

            // Scale and translate x coordinates
            __m256 x_coords = _mm256_fmadd_ps(x_index_vec, x_scale_vec, x_lo_vec);
            _mm256_storeu_ps(&cx[j*w+i], x_coords);

            // Scale and translate y coordinates
            __m256 y_coords = _mm256_fmadd_ps(y_index_vec, y_scale_vec, y_lo_vec);
            _mm256_storeu_ps(&cy[j*w+i], y_coords);
        }
    }
}