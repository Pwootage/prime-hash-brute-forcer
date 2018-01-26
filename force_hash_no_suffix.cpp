#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "crc32.hpp"

//#define COUNT
#define THREADS 4

#ifdef COUNT
size_t hash_count = 0;
#endif


static const char TEST_CHARS[] = "abcdefghijklmnopqrstuvwxyz";
static constexpr auto TEST_CHAR_LEN = sizeof(TEST_CHARS) - 1;

static const uint32_t TARGET_HASH = 0x93d24718; // 0x40C548E9 + ".mrea"

static const char PREFIX[] = "$/worlds/iceworld/!iceworld/cooked/07_ice_";
static constexpr auto PREFIX_LEN = sizeof(PREFIX) - 1;

using namespace std;
using namespace chrono;

template<size_t currentChar, size_t numChars>
inline void search_crc(char *buff,
                       uint32_t prefixCRC,
                       typename enable_if<(numChars == currentChar)>::type * = 0) {
  #ifdef COUNT
  hash_count++;
  #endif
  if (prefixCRC == TARGET_HASH) {
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
inline void search_crc_prefix_in_thread(char *buff, uint32_t prefixCRC) {
  search_crc<1, numChars>(
    buff,
    prefixCRC
  );
}

template<size_t numChars>
inline void search_crc_prefix_in_threads() {
  //  TODO: thread pool this, to reduce contention
  uint32_t prefixCRC = crc_str<PREFIX, PREFIX_LEN>(DEFAULT_CRC);

  vector<thread> threads;
  for (size_t i = 0; i < TEST_CHAR_LEN; i++) {
    char c = TEST_CHARS[i];
    char *buff = new char[numChars + 1]; // Leaks but I don't care, it's not big enough to matter
    memset(buff, 0, numChars + 1);
    buff[0] = c;
    uint32_t crc = update_crc(prefixCRC, static_cast<uint8_t>(c));
    threads.push_back(thread(search_crc_prefix_in_thread<numChars>, buff, crc));
  }
  for (thread &thread : threads) {
    thread.join();
  }
}

template<size_t numChars>
inline void search_crc_prefix() {
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
  #ifdef COUNT
  hash_count = 0;
  #endif

  using Clock = high_resolution_clock;
  auto start = Clock::now();

  #ifdef THREADS
  search_crc_prefix_in_threads<len>();
  #else
  search_crc_prefix<len>();
  #endif

  #ifdef COUNT
  size_t total = hash_count;
  #else
  size_t total = static_cast<size_t>(pow((size_t) TEST_CHAR_LEN, len));
  #endif

  auto end = steady_clock::now();
  auto diff = end - start;
  double ms = chrono::duration<double, milli>(diff).count();
  cout << total << "(len " << len << ")" << " in " << ms << " ms ";
  cout << static_cast<size_t>(total / (ms / 1000)) << "/s" << endl;
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