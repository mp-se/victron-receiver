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
#include <victron_ac.hpp>

test(ac_test1)
{
  // These values has been validated with the App
  VictronTestData testData = {
      "AC Charger", 0xA339, AcCharger, {0x06, 0x00, 0x3D, 0xA5, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xA1, 0x41, 0x00, 0x00, 0x00}};

  VictronAcCharger v(&testData.decrypted[0], testData.model);
  uint16_t data;

  assertEqual(v.getDeviceName(), "AC Charger");
  assertEqual(v.getModelNo(), 0xA339);
  assertEqual(v.getError(), 0);
  assertEqual(v.getState(), 6);
  data = v.getVoltage1() * 100;
  assertEqual(data, 1341);
  data = v.getCurrent1() * 100;
  assertEqual(data, 130);

  assertEqual(isnan(v.getVoltage2()), true);
  assertEqual(isnan(v.getCurrent2()), true);
  assertEqual(isnan(v.getVoltage3()), true);
  assertEqual(isnan(v.getCurrent3()), true);

  assertEqual(isnan(v.getTemperature()), true);
  assertEqual(isnan(v.getCurrentAC()), true);
}

test(ac_test2)
{
  VictronTestData testData = {
      "AC Charger", 0xA339, AcCharger, {0x04, 0x00, 0xA0, 0x25, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x7C, 0x00, 0x00, 0x00, 0x00}};
  VictronAcCharger v(&testData.decrypted[0], testData.model);
  uint16_t data;

  assertEqual(v.getDeviceName(), "AC Charger");
  assertEqual(v.getModelNo(), 0xA339);
  assertEqual(v.getError(), 0);
  assertEqual(v.getState(), 4);
  data = v.getVoltage1() * 100;
  assertEqual(data, 1440);
  data = v.getCurrent1() * 100;
  assertEqual(data, 330);

  assertEqual(isnan(v.getVoltage2()), true);
  assertEqual(isnan(v.getCurrent2()), true);
  assertEqual(isnan(v.getVoltage3()), true);
  assertEqual(isnan(v.getCurrent3()), true);

  assertEqual(isnan(v.getTemperature()), true);
  assertEqual(isnan(v.getCurrentAC()), true);
}

test(ac_test3)
{
  VictronTestData testData = {
      "AC Charger", 0xA339, AcCharger, {0x0B, 0x00, 0x05, 0x25, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xA8, 0xFF, 0xFF, 0xFF, 0x40, 0xEC, 0xF2, 0x00, 0x00, 0x00}};
  VictronAcCharger v(&testData.decrypted[0], testData.model);
  uint16_t data;

  assertEqual(v.getDeviceName(), "AC Charger");
  assertEqual(v.getModelNo(), 0xA339);
  assertEqual(v.getError(), 0);
  assertEqual(v.getState(), 11);
  data = v.getVoltage1() * 100;
  assertEqual(data, 1285);
  data = v.getCurrent1() * 100;
  assertEqual(data, 410);

  assertEqual(isnan(v.getVoltage2()), true);
  assertEqual(isnan(v.getCurrent2()), true);
  assertEqual(isnan(v.getVoltage3()), true);
  assertEqual(isnan(v.getCurrent3()), true);
  assertEqual(v.getTemperature(), 0.0);
  assertEqual(isnan(v.getCurrentAC()), true);
}

// EOF
