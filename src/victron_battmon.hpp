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

#ifndef SRC_VICTRON_BATTMON_HPP_
#define SRC_VICTRON_BATTMON_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronBatteryMonitor : public VictronDevice {
  /*
   * Used for the following model numbers:
   * 0xA3A4: "Smart Battery Sense",
   * 0xA3A5: "Smart Battery Sense",
   */
 private:
  typedef struct {
    uint16_t unused;
    int16_t batteryVoltage;
    uint16_t alarm;
    uint16_t temperature;
    uint8_t unused2[7];
  } __attribute__((packed)) VictronData;

  float _batteryVoltage;
  float _temperatureC;

 public:
  VictronBatteryMonitor(const uint8_t* data, uint16_t model) {
    VictronBatteryMonitor::VictronData* _data =
        (VictronBatteryMonitor::VictronData*)data;
    uint32_t v;

    setBaseData("Smart Battery Monitor", model, data);

    _batteryVoltage = _data->batteryVoltage != 0x7FFF
                          ? static_cast<float>(_data->batteryVoltage) / 100
                          : 0;  // 10 mV increments
    _temperatureC = (static_cast<float>(_data->temperature) / 100) -
                    273.15;  // Value of the temperature (K)

    Log.notice(F("VIC : Victron %s (%x) volt=%F V temp=%F C" CR),
               getDeviceName().c_str(), getModelNo(), getBatteryVoltage(),
               getTemperatureC());
  }

  float getBatteryVoltage() { return _batteryVoltage; }
  float getTemperatureC() { return _temperatureC; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    doc["battery_voltage"] =
        serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));
    doc["temperature"] = serialized(String(getTemperatureC(), DECIMALS_TEMP));
  }
};

#endif  // SRC_VICTRON_BATTMON_HPP_
