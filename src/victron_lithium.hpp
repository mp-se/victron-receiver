/*
MIT License

Copyright (c) 2024 Magnus

Based on code/ideas from these projects:
https://github.com/hoberman/Victron_BLE_Advertising_example
https://github.com/keshavdv/victron-ble

Victron docs on the manufacturer data in advertisement packets can be found at:
https://community.victronenergy.com/storage/attachments/48745-extra-manufacturer-data-2022-12-14.pdf

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
