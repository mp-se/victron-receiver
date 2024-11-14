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

#ifndef SRC_VICTRON_INVERTER_HPP_
#define SRC_VICTRON_INVERTER_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronInverter : public VictronDevice {
  /*
   * Used for the following model numbers:
   *
   * This is not yet tested, lack data to test with
   */
 private:
  typedef struct {
    uint8_t state;
    uint16_t alarm;
    int16_t batteryVoltage;
    uint16_t acPower;
    uint16_t acVoltage;
    uint16_t acCurrent;
  } __attribute__((packed)) VictronData;

  uint8_t _state;
  uint16_t _alarm;
  float _batteryVoltage;
  float _acPower;
  float _acVoltage;
  float _acCurrent;

 public:
  VictronInverter(const uint8_t* data, uint16_t model) {
    VictronInverter::VictronData* _data =
        (VictronInverter::VictronData*)data;
    uint32_t v;

    setBaseData("Inverter", model, data);

    _state = _data->state != 0xFF ? _data->state : 0;
    _alarm = _data->alarm != 0xFFFF ? _data->alarm : 0;

    _batteryVoltage = (_data->batteryVoltage & 0x7FFF) != 0x7FFF
            ? static_cast<float>(_data->batteryVoltage & 0x7FFF) / 100
            : 0;  // 10 mV increments
    _acPower = _data->acPower != 0xFFFF
            ? static_cast<float>(_data->acPower)
            : 0;
    _acVoltage = (_data->acVoltage & 0x7FFF) != 0x7FFF
            ? static_cast<float>(_data->acVoltage & 0x7FFF) / 100
            : 0;  // 10 mV increments
    _acCurrent = (_data->acCurrent & 0x7FF) != 0x7FF
            ? static_cast<float>(_data->acCurrent & 0x7FF) / 10
            : 0; 
  }

  float getBatteryVoltage() { return _batteryVoltage; }
  float getAcPower() { return _acPower; }
  float getAcVoltage() { return _acVoltage; }
  float getAcCurrent() { return _acCurrent; }

  uint8_t getState() { return _state; }
  uint8_t getAlarm() { return _alarm; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    doc["state"] = getState();
    doc["state_message"] = deviceStateToString(getState());
    doc["alarm"] = getAlarm();

    doc["battery_voltage"] =
        serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));

    doc["ac_power"] =
        serialized(String(getAcPower(), DECIMALS_POWER));
    doc["ac_voltage"] =
        serialized(String(getAcVoltage(), DECIMALS_VOLTAGE));
    doc["ac_current"] =
        serialized(String(getAcCurrent(), DECIMALS_CURRENT));
  }
};

#endif  // SRC_VICTRON_INVERTER_HPP_
