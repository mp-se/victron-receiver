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
