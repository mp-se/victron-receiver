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
  typedef struct {
    uint32_t bmsFlags;
    uint16_t alarm;
    uint8_t cell[8];
    uint16_t batteryVoltage; // Also inclucde status
    uint16_t batteryTemperature;
  } __attribute__((packed)) VictronData;

  uint32_t _bmsFlags;
  uint16_t _alarm;
  float _cells[8];
  float _batteryVoltage;
  uint8_t _batteryStatus;
  float _batteryTemperature;

 public:
  VictronLithium(const uint8_t* data, uint16_t model) {
    VictronLithium::VictronData* _data =
        (VictronLithium::VictronData*)data;
    uint32_t v;

    setBaseData("Lithium", model, data);

    _bmsFlags = _data->bmsFlags;
    _alarm = _data->alarm;

    // TODO: Parse rest of the data, need data to test with.
  }

  float getBatteryVoltage() { return _batteryVoltage; }
  float getBatteryTemperature() { return _batteryTemperature; }
  float getCell(int i) { return _cells[i]; }

  uint32_t getBmsFlags() { return _bmsFlags; }
  uint8_t getBatteryStatus() { return _batteryStatus; }
  uint16_t getAlarm() { return _alarm; }
  uint8_t getCellCount() { return 8; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    doc["bms_flags"] = getBmsFlags();
    doc["alarm"] = getAlarm();

    doc["battery_voltage"] =
        serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));
    doc["battery_temperature"] =
        serialized(String(getBatteryTemperature(), DECIMALS_TEMP));
    doc["battery_status"] = getBatteryStatus();

    // TODO: Add list with cells
  }
};

#endif  // SRC_VICTRON_LITHIUM_HPP_
