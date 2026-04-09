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
