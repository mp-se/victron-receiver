/*
MIT License

Copyright (c) 2024 Magnus

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
