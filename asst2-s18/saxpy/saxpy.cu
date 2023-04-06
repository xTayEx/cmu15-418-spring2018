#include <stdio.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>
#include <sstream>
#include <iostream>

#include "CycleTimer.h"

inline void fatalError(const std::string &err, const char *filename, int lineno) {
  std::stringstream pos, msg;
  pos << "In " << filename << ": " << lineno << "\n";
  msg << err << "\n";
  std::cerr << pos.str () << msg.str();
  cudaDeviceReset();
  exit(1);
}

inline void checkCudaImpl(cudaError status, const char *filename, int lineno) {
  std::stringstream err;
  if (status != cudaSuccess) {
    err << "CUDA error: " << cudaGetErrorString(status) << "\n";
    fatalError(err.str(), filename, lineno);
  }
}

#define checkCuda(status) checkCudaImpl(status, __FILE__, __LINE__)


extern float toBW(int bytes, float sec);

__global__ void saxpy_kernel(int N, float alpha, float *x, float *y,
                             float *result) {

  // compute overall index from position of thread in current block,
  // and given the block we are in
  int index = blockIdx.x * blockDim.x + threadIdx.x;

  if (index < N)
    result[index] = alpha * x[index] + y[index];
}

void saxpyCuda(int N, float alpha, float *xarray, float *yarray,
               float *resultarray) {

  int totalBytes = sizeof(float) * 3 * N;

  // compute number of blocks and threads per block
  const int threadsPerBlock = 512;
  const int blocks = (N + threadsPerBlock - 1) / threadsPerBlock;

  float *device_x;
  float *device_y;
  float *device_result;

  //
  // TODO allocate device memory buffers on the GPU using cudaMalloc
  //
  checkCuda(cudaMalloc(&device_x, sizeof(float) * N));
  checkCuda(cudaMalloc(&device_y, sizeof(float) * N));
  checkCuda(cudaMalloc(&device_result, sizeof(float) * N));

  // start timing after allocation of device memory
  double startTime = CycleTimer::currentSeconds();

  //
  // TODO copy input arrays to the GPU using cudaMemcpy
  //
  cudaMemcpy(device_x, xarray, sizeof(float) * N, cudaMemcpyHostToDevice);
  cudaMemcpy(device_y, yarray, sizeof(float) * N, cudaMemcpyHostToDevice);

  double kernelStartTime = CycleTimer::currentSeconds();
  // run kernel
  saxpy_kernel<<<blocks, threadsPerBlock>>>(N, alpha, device_x, device_y,
                                            device_result);
  cudaThreadSynchronize();
  double kernelEndTime = CycleTimer::currentSeconds();
  printf("Kernel time: %.3f ms\n", 1000.0f * (kernelEndTime - kernelStartTime));

  //
  // TODO copy result from GPU using cudaMemcpy
  //
  cudaMemcpy(resultarray, device_result, sizeof(float) * N, cudaMemcpyDeviceToHost);

  // end timing after result has been copied back into host memory
  double endTime = CycleTimer::currentSeconds();

  cudaError_t errCode = cudaPeekAtLastError();
  if (errCode != cudaSuccess) {
    fprintf(stderr, "WARNING: A CUDA error occured: code=%d, %s\n", errCode,
            cudaGetErrorString(errCode));
  }

  double overallDuration = endTime - startTime;
  printf("Overall: %.3f ms\t\t[%.3f GB/s]\n", 1000.f * overallDuration,
         toBW(totalBytes, overallDuration));

  // TODO free memory buffers on the GPU
  cudaFree(device_x);
  cudaFree(device_y);
  cudaFree(device_result);
}

void printCudaInfo() {

  // for fun, just print out some stats on the machine

  int deviceCount = 0;
  cudaError_t err = cudaGetDeviceCount(&deviceCount);

  printf("---------------------------------------------------------\n");
  printf("Found %d CUDA devices\n", deviceCount);

  for (int i = 0; i < deviceCount; i++) {
    cudaDeviceProp deviceProps;
    cudaGetDeviceProperties(&deviceProps, i);
    printf("Device %d: %s\n", i, deviceProps.name);
    printf("   SMs:        %d\n", deviceProps.multiProcessorCount);
    printf("   Global mem: %.0f MB\n",
           static_cast<float>(deviceProps.totalGlobalMem) / (1024 * 1024));
    printf("   CUDA Cap:   %d.%d\n", deviceProps.major, deviceProps.minor);
  }
  printf("---------------------------------------------------------\n");
}
