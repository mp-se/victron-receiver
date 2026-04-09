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
#include <victron_solar.hpp>

test(data_test1) {
  // The will parse the listed packets and dump the values to the serial
  // console.

  VictronTestData testData[] = {

      // This is test data from Thomas Jobs

      {"Solar Charger",
       0xA060,
       SolarCharger,
       {0x00, 0x00, 0x1C, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xFE, 0x00, 0x50, 0xFE, 0x00, 0x35, 0xE5, 0x00, 0x00, 0x00}},
      {"Solar Charger",
       0xA060,
       SolarCharger,
       {0x00, 0x00, 0x8E, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xFE, 0xFF, 0x40, 0xFE, 0x00, 0x08, 0xE7, 0x00, 0x00, 0x00}},

      {"Battery Monitor",
       0xA3A4,
       BatteryMonitor,
       {0xFF, 0xFF, 0x1B, 0x05, 0x00, 0x00, 0xC0, 0x6C, 0xFE, 0xFF, 0x7F,
        0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3A, 0xE5, 0x00, 0x00, 0x00}},
      {"Battery Monitor",
       0xA3A4,
       BatteryMonitor,
       {0xFF, 0xFF, 0x19, 0x05, 0x00, 0x00, 0xC0, 0x6C, 0xFE, 0xFF, 0x7F,
        0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xB7, 0xE6, 0x00, 0x00, 0x00}},

      {"Shunt", 0xA389, BatteryMonitor, {0x38, 0x09, 0x1B, 0x05, 0x00, 0x00,
                                         0x01, 0x05, 0xCC, 0xCB, 0xFF, 0x08,
                                         0x00, 0x50, 0xFE, 0x00, 0x68, 0xE5,
                                         0x00, 0x00, 0x00}},
      {"Shunt", 0xA389, BatteryMonitor, {0xFF, 0xFF, 0x8C, 0x05, 0x00, 0x00,
                                         0x1C, 0x05, 0x00, 0x00, 0x00, 0x0B,
                                         0x00, 0x40, 0xFE, 0x00, 0x3B, 0xE7,
                                         0x00, 0x00, 0x00}},
      {"DC-DC Charger",
       0xA060,
       DcDcConverter,
       {0x00, 0x00, 0xFE, 0x04, 0xFF, 0x7F, 0x08, 0x00, 0x00, 0x00, 0xFF,
        0x08, 0x00, 0x50, 0xFE, 0x00, 0x62, 0x8F, 0x00, 0x00, 0x00}},
      {"DC-DC Charger",
       0xA060,
       DcDcConverter,
       {0x00, 0x00, 0x1D, 0x05, 0xFF, 0x7F, 0x08, 0x00, 0x00, 0x00, 0x00,
        0x0B, 0x00, 0x40, 0xFE, 0x00, 0x35, 0x91, 0x00, 0x00, 0x00}},
      {"AC Charger", 0xA060, AcCharger, {0x04, 0x00, 0x91, 0x65, 0x06, 0xFF,
                                         0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAD,
                                         0xFF, 0x40, 0xFE, 0x00, 0x41, 0x00,
                                         0x00, 0x00, 0x00}}};

  for (int idx = 0; idx < sizeof(testData) / sizeof(VictronTestData); idx++) {
    switch (testData[idx].recordType) {
      case VictronDeviceType::BatteryMonitor: {
        if (testData[idx].model == 0xA3A4 || testData[idx].model == 0xA3A5) {
          VictronBatteryMonitor vbm(&testData[idx].decrypted[0],
                                    testData[idx].model);
        } else {
          VictronShunt vbm(&testData[idx].decrypted[0], testData[idx].model);
        }
      } break;

      case VictronDeviceType::DcDcConverter: {
        VictronDcDcCharger vdc(&testData[idx].decrypted[0],
                               testData[idx].model);
      } break;

      case VictronDeviceType::AcCharger: {
        VictronAcCharger vbm(&testData[idx].decrypted[0], testData[idx].model);
      } break;

      case VictronDeviceType::SolarCharger: {
        VictronSolarCharger vbm(&testData[idx].decrypted[0],
                                testData[idx].model);
      } break;
    }
  }
}

// EOF
