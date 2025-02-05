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
  uint8_t _state;
  uint8_t _error;
  float _batteryVoltage, _batteryCurrent;
  float _yieldToday, _pvPower, _loadCurrent;

 public:
  VictronSolarCharger(const uint8_t* data, uint16_t model) {
    setBaseData("Solar Charger", model, data);

    BitReader br(data, 21);

    /*
    typedef struct {
      uint8_t state;
      uint8_t error;
      uint16_t batteryVoltage;
      uint16_t batteryCurrent;
      uint16_t yieldToday;
      uint16_t pvPower;
      uint16_t loadCurrent;
      uint16_t test1;
      uint16_t test2;
      uint16_t test3;
    } __attribute__((packed)) VictronData;
    */

    uint8_t state = br.readUnsigned(8);
    uint8_t error = br.readUnsigned(8);
    int16_t batteryVoltage = br.readSigned(16);
    int16_t batteryCurrent = br.readSigned(16);
    uint16_t yieldToday = br.readUnsigned(16);
    uint16_t pvPower = br.readUnsigned(16);
    uint16_t loadCurrent = br.readUnsigned(16);

    _state = state != 0xFF ? state : 0;
    _error = error != 0xFF ? error : 0;

    _batteryVoltage =
        (batteryVoltage & 0x7FFF) != 0x7FFF
            ? static_cast<float>(batteryVoltage & 0x7FFF) / 100
            : NAN;  // 10 mV increments
    _batteryCurrent =
        (batteryCurrent & 0x7FFF) != 0x7FFF
            ? static_cast<float>(batteryCurrent & 0x7FFF) / 10
            : NAN;  // 0.1 A increments
    _yieldToday = yieldToday != 0xFFFF
                      ? static_cast<float>(yieldToday) / 100
                      : NAN;  // 10 mV increments
    _pvPower = pvPower != 0xFFFF ? static_cast<float>(pvPower)
                                        : NAN;  // W
    _loadCurrent = (loadCurrent & 0x1FF) != 0x1FF
                       ? static_cast<float>(loadCurrent & 0x1FF) / 10
                       : NAN;  // 0.1 A increments

    Log.notice(F("VIC : Victron %s (%x) state=%d error=%d battVolt=%F V "
                 "battCurrent=%F yieldToday=%F "
                 "load=%F pvPower=%F" CR),
               getDeviceName().c_str(), getModelNo(), getState(), getError(),
               getBatteryVoltage(), getBatteryCurrent(), getYieldToday(),
               getLoadCurrent(), getPvPower());
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

    if (!isnan(getBatteryVoltage()))
      doc["battery_voltage"] =
          serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));

    if (!isnan(getBatteryCurrent()))
      doc["battery_current"] =
          serialized(String(getBatteryCurrent(), DECIMALS_CURRENT));

    if (!isnan(getPvPower()))
      doc["pv_power"] = serialized(String(getPvPower(), DECIMALS_POWER));

    if (!isnan(getLoadCurrent()))
      doc["load_current"] =
          serialized(String(getLoadCurrent(), DECIMALS_CURRENT));
  }
};

#endif  // SRC_VICTRON_SOLAR_HPP_
