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
#include <victron_battmon.hpp>

test(battmon_test1) {
  // These values has been validated with the App
  VictronTestData testData = {
      "Battery Monitor",
      0xA3A5,
      BatteryMonitor,
      {0xFF, 0xFF, 0xBF, 0x04, 0x00, 0x00, 0x00, 0x73, 0xFE, 0xFF, 0x7F,
       0xFF, 0xFF, 0xFF, 0xFF, 0x40, 0x3C, 0xD5, 0x00, 0x00, 0x00}};
  VictronBatteryMonitor v(&testData.decrypted[0], testData.model);
  uint16_t data;

  assertEqual(v.getDeviceName(), "Battery Monitor");
  assertEqual(v.getModelNo(), 0xA3A5);
  data = v.getBatteryVoltage() * 100;
  assertEqual(data, 1215);
  data = v.getTemperatureC() * 100;
  assertEqual(data, 2124);
}

// EOF
