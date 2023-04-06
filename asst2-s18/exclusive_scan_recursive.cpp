#include <array>
#include <iostream>

void exclusive_scan_recursive(int *start, int *end, int *output, int *scratch, int recursion) {
  int N = end - start;
  if (N == 0) {
    return;
  } else if (N == 1) {
    output[0] = 0;
    return;
  }
  // sum pairs in parallel.
  for (int i = 0; i < N / 2; i++)
    output[i] = start[2 * i] + start[2 * i + 1];

  std::cout << "after paired-adding, output is:" << std::endl;
  for (int i = 0; i < 8; ++i) {
    std::cout << output[i] << " ";
  }
  std::cout << std::endl;
  // prefix sum on the compacted array.
  exclusive_scan_recursive(output, output + N / 2, scratch, scratch + (N / 2), recursion + 1);
  // finally, update the odd values in parallel.
  std::cout << "-------------------------\n";
  std::cout << "in recursion " << recursion << ", scratch is" << std::endl;
  for (int i = 0; i < 8; ++i) {
    std::cout << scratch[i] << " ";
  }
  std::cout << std::endl;
  for (int i = 0; i < N; i++) {
    output[i] = scratch[i / 2];
    if (i % 2)
      output[i] += start[i - 1];
  }
  // std::cout << "finally, output in recursion " << recursion << " is: \n";
  // for (int i = 0; i < 8; ++i) {
  //   std::cout << output[i] << " ";
  // }
  std::cout << std::endl;
}

int main() {
  int start[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  auto end = start + 8;
  int output[16] = {0};
  int scratch[16] = {0};
  exclusive_scan_recursive(start, end, output, scratch, 1);
  for (int i = 0; i < 8; ++i) {
    std::cout << output[i] << " ";
  }
  std::cout << std::endl;
}
