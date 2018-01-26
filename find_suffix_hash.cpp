#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <chrono>
#include <iostream>

#include "crc32.hpp"

static constexpr uint32_t TARGET_HASH = 0x40C548E9;
static const char SUFFIX[] = ".mrea";
static constexpr auto SUFFIX_LEN = sizeof(SUFFIX) - 1;

using namespace std;
using namespace chrono;


int main() {
  for (uint32_t i = 0; i < UINT32_MAX; i++) {
    uint32_t hash = crc_str<SUFFIX, SUFFIX_LEN>(i);
    if (hash == TARGET_HASH) {
      printf("Target hash: %x\n", i);
    }
    if (i % 0x10000000 == 0) {
      printf("Processed %x\n", i);
    }
  }
  // Have to check 0xFFFFFFFF manually
  uint32_t hash = crc_str<SUFFIX, SUFFIX_LEN>(UINT32_MAX);
  if (hash == TARGET_HASH) {
    printf("Target hash: %x\n", UINT32_MAX);
  }
  return 0;
}