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
#include <victron_dcdc.hpp>

test(dcdc_test1) {
  // These values has been validated with the App
  VictronTestData testData = {
      "DC-DC Charger",
      0xA3C0,
      DcDcConverter,
      {0x00, 0x00, 0x3D, 0x05, 0xFF, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x00,
       0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00}};
  VictronDcDcCharger v(&testData.decrypted[0], testData.model);
  uint16_t data;

  assertEqual(v.getDeviceName(), "DC-DC Charger");
  assertEqual(v.getModelNo(), 0xA3C0);
  assertEqual(v.getError(), 0);
  assertEqual(v.getState(), 0);
  data = v.getOffReasons();
  assertEqual(data, 128);
  data = v.getInputVoltage() * 100;
  assertEqual(data, 1341);
  assertEqual(isnan(v.getOutputVoltage()), true);
}

// EOF
