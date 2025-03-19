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
#include <AUnit.h>
#include <Arduino.h>

#include <testdata.hpp>
#include <victron_battprotect.hpp>

test(battprot_test1) {
  // These values has been validated with the App
  VictronTestData testData = {
      "Battery Protect",
      0xA3B0,
      BatteryProtect,
      {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x74,0x0A,0x0C,0x00,0x08,0x00,0x00,0x00,0xFF,0x31,0x43,0x00,0x00,0x00}};
  VictronBatteryProtect v(&testData.decrypted[0], testData.model);
  uint16_t data;

  assertEqual(v.getDeviceName(), "Battery Protect");
  assertEqual(v.getModelNo(), 0xA3B0);
  assertEqual(v.getError(), 0);
  assertEqual(v.getState(), 0);
  assertEqual(v.getOutputState(), 0);
  assertEqual(v.getAlarm(), 0);
  assertEqual(v.getWarning(),0);
  data = v.getOffReasons();
  assertEqual(data, 8);
  data = v.getInputVoltage() * 100;
  assertEqual(data, 2676);
  data = v.getOutputVoltage() * 100;
  assertEqual(data, 12);
}

test(battprot_test2) {
  // These values has been validated with the App
  VictronTestData testData = {
      "Battery Protect",
      0xA3B0,
      BatteryProtect,
      {0xF9,0x01,0x00,0x00,0x00,0x00,0x00,0x24,0x0A,0x23,0x0A,0x00,0x00,0x00,0x00,0x00,0x89,0x9E,0x00,0x00,0x00}};
  VictronBatteryProtect v(&testData.decrypted[0], testData.model);
  uint16_t data;

  assertEqual(v.getDeviceName(), "Battery Protect");
  assertEqual(v.getModelNo(), 0xA3B0);
  assertEqual(v.getError(), 0);
  assertEqual(v.getState(), 249);
  assertEqual(v.getOutputState(), 1);
  assertEqual(v.getAlarm(), 0);
  assertEqual(v.getWarning(),0);
  data = v.getOffReasons();
  assertEqual(data, 0);
  data = v.getInputVoltage() * 100;
  assertEqual(data, 2596);
  data = v.getOutputVoltage() * 100;
  assertEqual(data, 2595);
}

// EOF
