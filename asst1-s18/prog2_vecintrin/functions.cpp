#include "CMU418intrin.h"
#include "logger.h"
#include <algorithm>
using namespace std;

void absSerial(float *values, float *output, int N) {
  for (int i = 0; i < N; i++) {
    float x = values[i];
    if (x < 0) {
      output[i] = -x;
    } else {
      output[i] = x;
    }
  }
}

// implementation of absolute value using 15418 instrinsics
void absVector(float *values, float *output, int N) {
  __cmu418_vec_float x;
  __cmu418_vec_float result;
  __cmu418_vec_float zero = _cmu418_vset_float(0.f);
  __cmu418_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH) {

    // All ones
    maskAll = _cmu418_init_ones();

    // All zeros
    maskIsNegative = _cmu418_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _cmu418_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _cmu418_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _cmu418_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _cmu418_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _cmu418_vload_float(result, values + i,
                        maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _cmu418_vstore_float(output + i, result, maskAll);
  }
}

// Accepts an array of values and an array of exponents
// For each element, compute values[i]^exponents[i] and clamp value to
// 4.18.  Store result in outputs.
// Uses iterative squaring, so that total iterations is proportional
// to the log_2 of the exponent
void clampedExpSerial(float *values, int *exponents, float *output, int N) {
  for (int i = 0; i < N; i++) {
    float x = values[i];
    float result = 1.f;
    int y = exponents[i];
    float xpower = x;
    while (y > 0) {
      if (y & 0x1)
        result *= xpower;
      xpower = xpower * xpower;
      y >>= 1;
    }
    if (result > 4.18f) {
      result = 4.18f;
    }
    output[i] = result;
  }
}


void clampedExpVector(float *values, int *exponents, float *output, int N) {
  // Implement your vectorized version of clampedExpSerial here
  //  ...
	
	__cmu418_vec_float result = _cmu418_vset_float(1.0f);
	__cmu418_vec_float clampThreshold = _cmu418_vset_float(4.18f);
	__cmu418_vec_float positiveMask;
	__cmu418_vec_float x;
	__cmu418_vec_int zeroInt = _cmu418_vset_int(0);
	__cmu418_vec_float zeroFloat = _cmu418_vset_float(0.0f);
	__cmu418_vec_int ones = _cmu418_vset_int(1);
	__cmu418_vec_int exps;
	__cmu418_vec_int allBitOnes = _cmu418_vset_int(0xff);
	__cmu418_vec_int bitandResult;
	__cmu418_mask maskAll, maskGreaterThanZero, leastOne;
	__cmu418_mask eqZeroMask, neqZeroMask, clampMask;
	
	int idx;
	for (idx = 0; idx + VECTOR_WIDTH < N; idx += VECTOR_WIDTH) {
		maskAll = _cmu418_init_ones();
		_cmu418_vload_float(x, values + idx, maskAll);
		_cmu418_vload_int(exps, exponents + idx, maskAll);
		
		while (true) {
			_cmu418_vgt_int(maskGreaterThanZero, exps, zeroInt, maskAll);
			if (!_cmu418_cntbits(maskGreaterThanZero)) {
				break;
			}
			_cmu418_vbitand_int(bitandResult, exps, ones, maskAll);	
			_cmu418_veq_int(eqZeroMask, bitandResult, zeroInt, maskAll);
			neqZeroMask = _cmu418_mask_not(eqZeroMask);
			_cmu418_vmult_float(result, result, x, neqZeroMask);
			_cmu418_vmult_float(x, x, x, maskAll);
			_cmu418_vshiftright_int(exps, exps, ones, maskAll);

			// vector min
			_cmu418_vgt_float(clampMask, result, clampThreshold, maskAll);
			_cmu418_vadd_float(result, zeroFloat, clampThreshold, clampMask);
			_cmu418_vstore_float(output + idx, result, maskAll);
		}
	}
	if (idx != N) {
		clampedExpSerial(values + idx, exponents + idx, output + idx, N - idx);
	}
}

float arraySumSerial(float *values, int N) {
  float sum = 0;
  for (int i = 0; i < N; i++) {
    sum += values[i];
  }

  return sum;
}

// Assume N % VECTOR_WIDTH == 0
// Assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N) {
  // Implement your vectorized version here
  //  ...
}
