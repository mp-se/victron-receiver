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

#ifndef SRC_TESTDATA_HPP_
#define SRC_TESTDATA_HPP_

#include <blescanner.hpp>

class VictronBleSimulationData {
  String _name;
  String _json;

 public:
  VictronBleSimulationData(String name, String json) {
    _name = name;
    _json = json;
  }

  String getName() { return _name; }
  String getJson() { return _json; }
};

struct VictronTestData {
  char name[30];
  uint16_t model;
  uint8_t recordType;
  uint8_t decrypted[21];
};

int getNoTestData();
VictronBleSimulationData createDeviceFromTestData(int idx);

#endif  // SRC_TESTDATA_HPP_
