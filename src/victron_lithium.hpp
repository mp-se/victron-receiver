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

#ifndef SRC_VICTRON_LITHIUM_HPP_
#define SRC_VICTRON_LITHIUM_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronLithium : public VictronDevice {
  /*
   * Used for the following model numbers:
   *
   * This is not yet tested, lack data to test with
   */
 private:
  uint32_t _bmsFlags;
  uint16_t _error;
  float _cell[8];
  float _batteryVoltage;
  uint8_t _balancerStatus;
  float _batteryTemperature;

 public:
  VictronLithium(const uint8_t* data, uint16_t model) {
    setBaseData("Lithium", model, data);

    BitReader br(data, 21);

    _bmsFlags = br.readUnsigned(32);
    _error = br.readUnsigned(16);
    uint8_t cell[8];
    cell[0] = br.readUnsigned(7);
    cell[1] = br.readUnsigned(7);
    cell[2] = br.readUnsigned(7);
    cell[3] = br.readUnsigned(7);
    cell[4] = br.readUnsigned(7);
    cell[5] = br.readUnsigned(7);
    cell[6] = br.readUnsigned(7);
    cell[7] = br.readUnsigned(7);
    uint16_t batteryVoltage = br.readUnsigned(12);
    _balancerStatus = br.readUnsigned(4);
    uint16_t batteryTemperature = br.readUnsigned(7);

    for (int i = 0; i < sizeof(cell); i++) {
      _cell[i] = cell[i] != 0xFF ? static_cast<float>(cell[i]) / 100
                                 : NAN;  // 10 mV increments
    }

    _batteryVoltage = batteryVoltage != 0x0FFF
                          ? static_cast<float>(batteryVoltage) / 100
                          : NAN;  // 10 mV increments

    _batteryTemperature = batteryTemperature != 0x7F
                              ? static_cast<float>(batteryTemperature) - 40
                              : NAN;
  }

  float getBatteryVoltage() { return _batteryVoltage; }
  float getBatteryTemperature() { return _batteryTemperature; }
  float getCell(int i) { return _cell[i]; }

  uint32_t getBmsFlags() { return _bmsFlags; }
  uint8_t getBatteryStatus() { return _balancerStatus; }
  uint16_t getError() { return _error; }
  uint8_t getCellCount() { return 8; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    doc["bms_flags"] = getBmsFlags();
    doc["error"] = getError();

    if (!isnan(getBatteryVoltage()))
      doc["battery_voltage"] =
          serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));

    if (!isnan(getBatteryTemperature()))
      doc["battery_temperature"] =
          serialized(String(getBatteryTemperature(), DECIMALS_TEMP));

    if (!isnan(getBatteryStatus())) doc["balancer_status"] = getBatteryStatus();

    if (!isnan(getCell(0)))
      doc["cell1"] = serialized(String(getCell(0), DECIMALS_VOLTAGE));
    if (!isnan(getCell(1)))
      doc["cell2"] = serialized(String(getCell(1), DECIMALS_VOLTAGE));
    if (!isnan(getCell(2)))
      doc["cell3"] = serialized(String(getCell(2), DECIMALS_VOLTAGE));
    if (!isnan(getCell(3)))
      doc["cell4"] = serialized(String(getCell(3), DECIMALS_VOLTAGE));
    if (!isnan(getCell(4)))
      doc["cell5"] = serialized(String(getCell(4), DECIMALS_VOLTAGE));
    if (!isnan(getCell(5)))
      doc["cell6"] = serialized(String(getCell(5), DECIMALS_VOLTAGE));
    if (!isnan(getCell(6)))
      doc["cell7"] = serialized(String(getCell(6), DECIMALS_VOLTAGE));
    if (!isnan(getCell(7)))
      doc["cell8"] = serialized(String(getCell(7), DECIMALS_VOLTAGE));
  }
};

#endif  // SRC_VICTRON_LITHIUM_HPP_
