#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <chrono>
#include <iostream>

#include "crc32.hpp"

static const char TEST_CHARS[] = "abcdefghijklmnopqrstuvwxyz";
static constexpr auto TEST_CHAR_LEN = sizeof(TEST_CHARS) - 1;

static const uint32_t TARGET_HASH = 0x40C548E9;

static const char PREFIX[] = "$/worlds/iceworld/!iceworld/cooked/07_ice_";
static constexpr auto PREFIX_LEN = sizeof(PREFIX) - 1;

static const char SUFFIX[] = ".mrea";
static constexpr auto SUFFIX_LEN = sizeof(SUFFIX) - 1;

using namespace std;
using namespace chrono;

template<size_t currentChar, size_t numChars>
inline void search_crc(char *buff,
                       uint32_t prefixCRC,
                       typename enable_if<(numChars == currentChar)>::type * = 0) {
  // This final hash is fixed, based on what the prefix hash is
  // TODO: figure out the appropriate hash for what will be correct for a given suffix
  // This means that the suffix can basically be dropped, reducing the crc-bytes
  // to exactly the number of characters we're brute forcing
  uint32_t finalHash = crc_str<SUFFIX, SUFFIX_LEN>(prefixCRC);
  if (finalHash == TARGET_HASH) {
    printf("Match: %x, %s\n", prefixCRC, buff);
  }
}

template<size_t currentChar, size_t numChars>
inline void search_crc(char *buff,
                       uint32_t prefixCRC,
                       typename enable_if<(numChars != currentChar)>::type * = 0) {
  for (size_t i = 0; i < TEST_CHAR_LEN; i++) {
    char c = TEST_CHARS[i];
    buff[currentChar] = c;
    uint32_t updatedCRC = update_crc(prefixCRC, static_cast<uint8_t>(c));
    search_crc<currentChar + 1, numChars>(
      buff,
      updatedCRC
    );
  }
}

template<size_t numChars>
inline void search_crc_prefix_suffix() {
  uint32_t prefixCRC = crc_str<PREFIX, PREFIX_LEN>(DEFAULT_CRC);
  char buff[numChars + 1];
  memset(buff, 0, numChars + 1);
  search_crc<0, numChars>(
    buff,
    prefixCRC
  );
}

template<size_t len>
void execLen() {
  using Clock = high_resolution_clock;
  auto start = Clock::now();
  search_crc_prefix_suffix<len>();
  size_t total = static_cast<size_t>(pow((size_t) TEST_CHAR_LEN, len));
  auto end = steady_clock::now();
  auto diff = end - start;
  double ms = chrono::duration<double, milli>(diff).count();
  cout << total << "(len " << len << ")" << " in " << ms << " ms ";
  cout << total / ms << "/ms" << endl;
}

int main() {
  execLen<1>();
  execLen<2>();
  execLen<3>();
  execLen<4>();
  execLen<5>();
  execLen<6>();
  execLen<7>();
  execLen<8>();

  return 0;
}