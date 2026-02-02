/*
MIT License

Copyright (c) 2026 Magnus

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
#ifndef SRC_EXIDE_DATA_PARSER_HPP_
#define SRC_EXIDE_DATA_PARSER_HPP_

#include <Arduino.h>

#include <cstddef>
#include <cstdint>

class ExideDataParser {
 public:
  struct ExideInfo {
    float voltage;
    float current;
    float temperature;
    uint8_t soc;  // State of charge
    uint16_t cycleCount;
    uint32_t capacity;  // Total capacity in mAh
    uint16_t cellVoltages[16];
    uint8_t cellCount;
    uint8_t errorState;
    String version;
    bool valid;
  };

  static ExideInfo parseRealTimeData(uint8_t* data, size_t len);
  static ExideInfo parseVersionData(uint8_t* data, size_t len);
  static ExideInfo parseBatteryState(uint8_t* data, size_t len);

 private:
  static uint16_t bytesToUint16(uint8_t* bytes);
  static int16_t bytesToInt16(uint8_t* bytes);
  static int32_t bytesToInt32(uint8_t* bytes);
};

#endif  // SRC_EXIDE_DATA_PARSER_HPP_

// EOF
