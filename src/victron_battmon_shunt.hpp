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

#ifndef SRC_VICTRON_BATTMON_SHUNT_HPP_
#define SRC_VICTRON_BATTMON_SHUNT_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronShunt : public VictronDevice {
  /*
   * Used for the following model numbers:
   * 0xA389: "SmartShunt 500A/50mV"
   * 0xA38A: "SmartShunt 1000A/50mV"
   * 0xA38B: "SmartShunt 2000A/50mV"
   */
 private:
  typedef struct {
    uint16_t remainingMins;
    uint16_t batteryVoltage;
    uint16_t alarm;
    uint16_t aux;
    uint8_t batteryCurrent[3];  // 24 bits
    // uint32_t consumedSoc;
    uint8_t consumedAh[3];  // 20 bits, 4 bits to soc
    uint8_t soc;            // 10 bits
  } __attribute__((packed)) VictronData;

  uint16_t _remaningMins;
  uint16_t _alarm;
  float _batteryVoltage;
  float _aux;
  uint8_t _auxMode;
  float _batteryCurrent;
  float _consumedAh;
  float _soc;

 public:
  VictronShunt(const uint8_t* data, uint16_t model) {
    VictronShunt::VictronData* _data = (VictronShunt::VictronData*)data;

    setBaseData("Shunt", model, data);

    _remaningMins = _data->remainingMins != 0xFFFF ? _data->remainingMins : 0;
    _batteryVoltage =
        (_data->batteryVoltage & 0x7FFF) != 0x7FFF
            ? static_cast<float>(_data->batteryVoltage & 0x7FFF) / 100
            : NAN;  // 10 mV increments
    _alarm = _data->alarm != 0xFFFF ? _data->alarm : 0;

    uint32_t bc =
        _create24bitUnsigned(_data->batteryCurrent[0], _data->batteryCurrent[1],
                             _data->batteryCurrent[2]);

    _auxMode =
        bc & 0x03;  // 0=StarterVoltage, 1=MidPointVoltage, 2=Temperature, 3=Off

    switch (_auxMode) {
      case 0:  // Aux mode
        _aux = static_cast<float>(static_cast<int16_t>(_data->aux)) /
               100;  // 10 mV increments
        break;
      case 1:                                         // Mid mode
        _aux = static_cast<float>(_data->aux) / 100;  // 10 mV increments
        break;
      case 2:  // Temperature
        _aux =
            static_cast<float>(_data->aux) / 100 - 273.15;  // Kelvin to Celcius
        break;
      case 3:  // Disabled
        _aux = NAN;
        break;
    }

    bc = bc >> 2;
    _batteryCurrent = bc != 0x3FFFFF
                          ? static_cast<float>(_22bitTo32bitSigned(bc)) / 1000
                          : NAN;

    uint32_t ca =
        _create24bitUnsigned(_data->consumedAh[0], _data->consumedAh[1],
                             _data->consumedAh[2]) &
        0xFFFFF;
    uint16_t soc = static_cast<uint16_t>(_data->soc) << 4 |
                   static_cast<uint16_t>(_data->consumedAh[2] & 0xf0) >> 4;

    soc = soc & 0x3ff;
    _consumedAh = ca != 0xFFFFF ? -static_cast<float>(ca) / 10 : NAN;
    _soc =
        soc != 0x3FF ? static_cast<float>(soc) / 10 : NAN;  // 0.1% increments

    // if (_soc > 100) _soc = 100.0;

    Log.notice(
        F("VIC : Victron %s (%x) remaningMins=%d V battVoltage=%F alarm=%d "
          "aux=%F consumedAh=%F auxMode=%d battCurrent=%F soc=%F" CR),
        getDeviceName().c_str(), getModelNo(), getRemaningMins(),
        getBatteryVoltage(), getAlarm(), getAux(), getConsumedAh(),
        getAuxMode(), getBatteryCurrent(), getSoc());
  }

  uint16_t getRemaningMins() { return _remaningMins; }
  uint16_t getAlarm() { return _alarm; }
  uint8_t getAuxMode() { return _auxMode; }

  float getBatteryVoltage() { return _batteryVoltage; }
  float getAux() { return _aux; }
  float getBatteryCurrent() { return _batteryCurrent; }
  float getConsumedAh() { return _consumedAh; }
  float getSoc() { return _soc; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    doc["alarm"] = getAlarm();

    if (!isnan(getBatteryVoltage()))
      doc["battery_voltage"] =
          serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));

    if (!isnan(getBatteryCurrent()))
      doc["battery_current"] = serialized(
          String(getBatteryCurrent(), 3));  // Higher resolution than others.

    if (!isnan(getConsumedAh())) doc["consumed_ah"] = getConsumedAh();

    if (!isnan(getSoc())) doc["soc"] = serialized(String(getSoc(), 1));

    doc["remaning_mins"] = getRemaningMins();

    switch (getAuxMode()) {
      case 0:
        doc["aux_mode"] = "aux voltage";
        doc["aux_voltage"] = serialized(String(getAux(), DECIMALS_VOLTAGE));
        break;
      case 1:
        doc["aux_mode"] = "mid voltage";
        doc["mid_voltage"] = serialized(String(getAux(), DECIMALS_VOLTAGE));
        break;
      case 2:
        doc["aux_mode"] = "temperature";
        doc["temperature"] = serialized(String(getAux(), DECIMALS_TEMP));
        break;
      case 3:
        doc["aux_mode"] = "disabled";
        // Disabled
        break;
    }
  }
};

#endif  // SRC_VICTRON_BATTMON_SHUNT_HPP_
