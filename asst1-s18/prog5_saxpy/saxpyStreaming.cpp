#include <assert.h>
#include <immintrin.h>
#include <emmintrin.h> // For _mm_mul_ps
#include <smmintrin.h> // For _mm_stream_load_si128
#include <stdint.h>
#include <cstdlib>
#include <xmmintrin.h>

extern void saxpySerial(int N, float scale, float X[], float Y[],
                        float result[]);

void saxpyStreaming(int N, float scale, float X[], float Y[], float result[]) {
  // Replace this code with ones that make use of the streaming instructions
  __m128 x, y;
  __m128 s = _mm_set1_ps(scale);
  for (int i = 0; i < N; i += 4) {
    x = _mm_load_ps(X + i);
    y = _mm_load_ps(Y + i);
    _mm_store_ps(result + i, _mm_add_ps(_mm_mul_ps(s, x), y));
  }
}
