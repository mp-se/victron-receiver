/*
MIT License

Copyright (c) 2025 Magnus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#ifndef SRC_BITREADER_HPP_
#define SRC_BITREADER_HPP_

#include <Arduino.h>

#include <log.hpp>

class BitReader {
 private:
  const uint8_t *_data;
  uint8_t _len;
  uint8_t _offset;

  uint32_t readBit() {
    if ((_offset / 8) >= _len) {
      return 0;
    }

    uint8_t bit = *(_data + (_offset >> 3)) >> (_offset & 7) & 1;
    _offset++;
    return static_cast<uint32_t>(bit);
  }

 public:
  BitReader(const uint8_t *data, uint8_t len) {
    _data = data;
    _len = len;
    _offset = 0;
  }

  void resetOffset() { _offset = 0; }

  uint32_t readUnsigned(uint8_t noBits) {
    uint32_t value = 0;

    for (int i = 0; i < noBits; i++) {
      value |= readBit() << i;
    }

    return value;
  }

  int32_t readSigned(uint8_t noBits) {
    uint32_t value = readUnsigned(noBits);
    return convert(value, noBits);
  }

  int32_t convert(uint32_t value, uint8_t noBits) {
    if (noBits > 0 && noBits < 32) {
      uint32_t mask = 1 << (noBits - 1);
      value = (value ^ mask) - mask;
    }
    return static_cast<int32_t>(value);
  }
};

#endif  // SRC_BITREADER_HPP_
