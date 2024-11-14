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

#ifndef SRC_VICTRON_SOLAR_HPP_
#define SRC_VICTRON_SOLAR_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronSolarCharger : public VictronDevice {
  /*
   * Used for the following model numbers:
   * 0xA042: "BlueSolar MPPT 75|15"
   *
   * TODO: Add other models that this should support...
   */
 private:
  typedef struct {
    uint8_t state;
    uint8_t error;
    uint16_t batteryVoltage;
    uint16_t batteryCurrent;
    uint16_t yieldToday;
    uint16_t pvPower;
    uint16_t loadCurrent;
    uint8_t unused[9];
  } __attribute__((packed)) VictronData;

  uint8_t _state;
  uint8_t _error;
  float _batteryVoltage, _batteryCurrent;
  float _yieldToday, _pvPower, _loadCurrent;

 public:
  VictronSolarCharger(const uint8_t* data, uint16_t model) {
    VictronSolarCharger::VictronData* _data =
        (VictronSolarCharger::VictronData*)data;

    setBaseData("Solar Charger", model, data);

    _state = _data->state != 0xFF ? _data->state : 0;
    _error = _data->error != 0xFF ? _data->error : 0;

    _batteryVoltage =
        (_data->batteryVoltage & 0x7FFF) != 0x7FFF
            ? static_cast<float>(_data->batteryVoltage & 0x7FFF) / 100
            : 0;  // 10 mV increments
    _batteryCurrent =
        (_data->batteryCurrent & 0x7FFF) != 0x7FFF
            ? static_cast<float>(_data->batteryCurrent & 0x7FFF) / 10
            : 0;  // 0.1 A increments
    _yieldToday = _data->yieldToday != 0xFFFF
                      ? static_cast<float>(_data->yieldToday) / 100
                      : 0;  // 10 mV increments
    _pvPower =
        _data->pvPower != 0xFFFF ? static_cast<float>(_data->pvPower) : 0;  // W
    _loadCurrent = (_data->loadCurrent & 0x1FF) != 0x1FF
                       ? static_cast<float>(_data->loadCurrent & 0x1FF) / 10
                       : 0;  // 0.1 A increments

    Log.notice(F("VIC : Victron %s (%x) battVolt=%F V battCurrent=%F yield=%F "
                 "load=%F" CR),
               getDeviceName().c_str(), getModelNo(), getBatteryVoltage(),
               getBatteryCurrent(), getYieldToday(), getLoadCurrent());
  }

  uint8_t getState() { return _state; }
  uint8_t getError() { return _error; }

  float getBatteryVoltage() { return _batteryVoltage; }
  float getBatteryCurrent() { return _batteryCurrent; }
  float getYieldToday() { return _yieldToday; }
  float getPvPower() { return _pvPower; }
  float getLoadCurrent() { return _loadCurrent; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    doc["state"] = getState();
    doc["state_message"] = deviceStateToString(getState());
    doc["error"] = getError();
    doc["error_message"] = deviceChargerErrorToString(getError());
    doc["battery_voltage"] =
        serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));
    doc["battery_current"] =
        serialized(String(getBatteryCurrent(), DECIMALS_CURRENT));
    doc["pv_power"] = serialized(String(getPvPower(), DECIMALS_POWER));
    doc["load_current"] =
        serialized(String(getLoadCurrent(), DECIMALS_CURRENT));
  }
};

#endif  // SRC_VICTRON_SOLAR_HPP_
