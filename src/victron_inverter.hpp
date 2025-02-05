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
  uint8_t _state;
  uint16_t _alarm;
  float _batteryVoltage;
  float _acPower;
  float _acVoltage;
  float _acCurrent;

 public:
  VictronInverter(const uint8_t* data, uint16_t model) {
    setBaseData("Inverter", model, data);

    BitReader br(data, 21);

    /*
    typedef struct {
      uint8_t state;
      uint16_t alarm;
      int16_t batteryVoltage;
      uint16_t acPower;
      uint16_t acVoltage;
      uint16_t acCurrent;
    } __attribute__((packed)) VictronData;
    */

    uint8_t state = br.readUnsigned(8);
    uint16_t alarm = br.readUnsigned(16);
    int16_t batteryVoltage = br.readSigned(16);
    uint16_t acPower = br.readUnsigned(16);
    int16_t acVoltage = br.readSigned(16);
    uint16_t acCurrent = br.readUnsigned(16);

    _state = state != 0xFF ? state : 0;
    _alarm = alarm != 0xFFFF ? alarm : 0;

    _batteryVoltage =
        (batteryVoltage & 0x7FFF) != 0x7FFF
            ? static_cast<float>(batteryVoltage & 0x7FFF) / 100
            : NAN;  // 10 mV increments
    _acPower =
        acPower != 0xFFFF ? static_cast<float>(acPower) : NAN;
    _acVoltage = (acVoltage & 0x7FFF) != 0x7FFF
                     ? static_cast<float>(acVoltage & 0x7FFF) / 100
                     : NAN;  // 10 mV increments
    _acCurrent = (acCurrent & 0x7FF) != 0x7FF
                     ? static_cast<float>(acCurrent & 0x7FF) / 10
                     : NAN;
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

    if (!isnan(getBatteryVoltage()))
      doc["battery_voltage"] =
          serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));

    if (!isnan(getAcPower()))
      doc["ac_power"] = serialized(String(getAcPower(), DECIMALS_POWER));

    if (!isnan(getAcVoltage()))
      doc["ac_voltage"] = serialized(String(getAcVoltage(), DECIMALS_VOLTAGE));

    if (!isnan(getAcCurrent()))
      doc["ac_current"] = serialized(String(getAcCurrent(), DECIMALS_CURRENT));
  }
};

#endif  // SRC_VICTRON_INVERTER_HPP_
