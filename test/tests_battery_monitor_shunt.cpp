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
#include <victron_battmon_shunt.hpp>

test(shunt_test1) {
  // Test data from Thomas Jobs
  VictronTestData testData = {
      "Shunt", 0xA389, BatteryMonitor, {0xFF, 0xFF, 0x8C, 0x05, 0x00, 0x00,
                                        0x1C, 0x05, 0x00, 0x00, 0x00, 0x0B,
                                        0x00, 0x40, 0xFE, 0x00, 0x3B, 0xE7,
                                        0x00, 0x00, 0x00}};
  VictronShunt v(&testData.decrypted[0], testData.model);
  uint16_t data;
  int16_t data2;

  assertEqual(v.getDeviceName(), "Shunt");
  assertEqual(v.getModelNo(), 0xA389);
  assertEqual(v.getAlarm(), 0);
  assertEqual(v.getAuxMode(), 0);
  assertEqual(v.getRemaningMins(), 0);
  data = v.getSoc() * 10;
  assertEqual(data, 996);
  data2 = v.getBatteryCurrent() * 1000;
  assertEqual(data2, 0);
  data = v.getBatteryVoltage() * 100;
  assertEqual(data, 1420);
  data = v.getAux() * 100;
  assertEqual(data, 1308);
  data2 = v.getConsumedAh() * 10;
  assertEqual(data2, 11);
}

test(shunt_test2) {
  // Test data from Thomas Jobs
  VictronTestData testData = {
      "Shunt", 0xA389, BatteryMonitor, {0xDB,0x00,0x0A,0x05,0x00,0x00,0xEF,0x04,0x20,0xBF,0xFF,0x85,0x03,0x60,0xE2,0x40,0x64,0x04,0x00,0x00,0x00}};
  VictronShunt v(&testData.decrypted[0], testData.model);
  uint16_t data;
  int16_t data2;

  // Expected results, Consumed = -90.4, Soc = 55%, Current 4.12

  assertEqual(v.getDeviceName(), "Shunt");
  assertEqual(v.getModelNo(), 0xA389);
  assertEqual(v.getAlarm(), 0);
  assertEqual(v.getAuxMode(), 0);
  assertEqual(v.getRemaningMins(), 219);

  data = v.getSoc() * 10;
  assertEqual(data, 550);
  data2 = v.getBatteryCurrent() * 1000;
  assertEqual(data2, -4152);
  data = v.getBatteryVoltage() * 100;
  assertEqual(data, 1290);
  data = v.getAux() * 100;
  assertEqual(data, 1263);
  data2 = v.getConsumedAh() * 10;
  assertEqual(data2, 901);
}

test(shunt_test3) {
  VictronTestData testData = {
      "Shunt", 0xA389, BatteryMonitor, {0x38, 0x09, 0x1B, 0x05, 0x00, 0x00,
        0x01, 0x05, 0xCC, 0xCB, 0xFF, 0x08,
        0x00, 0x50, 0xFE, 0x00, 0x68, 0xE5,
        0x00, 0x00, 0x00}};
  VictronShunt v(&testData.decrypted[0], testData.model);
  uint16_t data;
  int16_t data2;
  
  assertEqual(v.getDeviceName(), "Shunt");
  assertEqual(v.getModelNo(), 0xA389);
  assertEqual(v.getAlarm(), 0);
  assertEqual(v.getAuxMode(), 0);
  assertEqual(v.getRemaningMins(), 2360);

  data = v.getSoc() * 10;
  assertEqual(data, 997);
  data2 = v.getBatteryCurrent() * 1000;
  assertEqual(data2, -3341);
  data2 = v.getBatteryVoltage() * 100;
  assertEqual(data2, 1307);
  data = v.getAux() * 100;
  assertEqual(data, 1281);
  data2 = v.getConsumedAh() * 10;
  assertEqual(data2, 8);
}

// EOF
