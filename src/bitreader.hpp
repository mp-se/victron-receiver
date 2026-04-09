/*
GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007

Copyright (C) 2024-2026 Magnus
https://github.com/mp-se/victron-receiver

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
