#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <chrono>
#include <iostream>

#include "crc32.hpp"

//$/sounds/audiogroups/ridley.agsc
//60544DEE

//#define TARGET_HASH (0x60544DEE)
#define TARGET_HASH (0x40C548E9)
#define PREFIX ("$/worlds/iceworld/!iceworld/cooked/07_ice_")
#define SUFFIX (".mrea")

#define MAX_SEARCH_LEN (8)

const char TEST_CHARS[] = "abcdefghijklmnopqrstuvwxyz";
static constexpr auto TEST_CHAR_COUNT = sizeof(TEST_CHARS) - 1;

using namespace std;
using namespace chrono;

inline uint32_t update_crc(uint32_t crc, uint8_t c) {
  const uint32_t lookup = (crc ^ c) & 0xFF;
  return (crc >> 8) ^ crc_table[lookup];
}

inline uint32_t crc_str(uint32_t initial, const char* str, size_t str_len) {
  uint32_t crc = initial;
  for (size_t i = 0; i < str_len; i++) {
    crc = update_crc(crc, static_cast<uint8_t>(str[i]));
  }
  return crc;
}

template<size_t currentChar, size_t numChars>
inline void search_crc_suffix(char *buff,
                              uint32_t prefixCRC,
                              const char *suffix,
                              size_t suffix_len,
                              typename enable_if<(numChars == currentChar)>::type * = 0) {
  // This final hash is fixed, based on what the prefix hash is
  // TODO: figure out the appropriate hash for what will be correct for a given suffix
  // This means that the suffix can basically be dropped, reducing the crc-bytes
  // to exactly the number of characters we're brute forcing
  uint32_t finalHash = crc_str(prefixCRC, suffix, suffix_len);
  if (finalHash == TARGET_HASH) {
    printf("Match: %x, %s\n", prefixCRC, buff);
  }
}

template<size_t currentChar, size_t numChars>
inline void search_crc_suffix(char *buff,
                              uint32_t prefixCRC,
                              const char *suffix,
                              size_t suffix_len,
                              typename enable_if<(numChars != currentChar)>::type * = 0) {
  for (size_t i = 0; i < TEST_CHAR_COUNT; i++) {
    char c = TEST_CHARS[i];
    buff[currentChar] = c;
    uint32_t updatedCRC = update_crc(prefixCRC, static_cast<uint8_t>(c));
    search_crc_suffix<currentChar + 1, numChars>(
      buff,
      updatedCRC,
      suffix,
      suffix_len
    );
  }
}

template<size_t numChars>
inline void search_crc_prefix_suffix(const string &prefix, const string &suffix) {
  uint32_t prefixCRC = crc_str(DEFAULT_CRC, prefix.c_str(), prefix.length());
  char buff[numChars + 1];
  memset(buff, 0, numChars + 1);
  search_crc_suffix<0, numChars>(
    buff,
    prefixCRC,
    suffix.c_str(),
    suffix.length()
  );
}

template<size_t len>
void execLen() {
  using Clock = high_resolution_clock;
  auto start = Clock::now();
  search_crc_prefix_suffix<len>(
    PREFIX,
    SUFFIX
  );
  size_t total = static_cast<size_t>(pow((size_t) TEST_CHAR_COUNT, len));
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