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
#include <AUnit.h>
#include <Arduino.h>

#include <testdata.hpp>
#include <victron_dcdc.hpp>

test(dcdc_test1) {
  VictronTestData testData = {
      "DC-DC Charger",
      0xA3C0,
      DcDcConverter,
      {0x00, 0x00, 0xF7, 0x04, 0xFF, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x37,
       0x40, 0x23, 0x09, 0x06, 0x00, 0x42, 0x10, 0x00, 0x00, 0x00}};
  VictronDcDcCharger v(&testData.decrypted[0], testData.model);
  uint16_t data;

  assertEqual(v.getDeviceName(), "DC-DC Charger");
  assertEqual(v.getModelNo(), 0xA3C0);
  assertEqual(v.getError(), 0);
  assertEqual(v.getState(), 0);
  data = v.getOffReasons();
  assertEqual(data, 128);
  data = v.getInputVoltage() * 100;
  assertEqual(data, 1271);
  assertEqual(isnan(v.getOutputVoltage()), true);
}

// EOF
