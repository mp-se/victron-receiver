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
    setBaseData("Shunt", model, data);

    BitReader br(data, 21);

    uint16_t remainingMins = br.readUnsigned(16);
    int16_t batteryVoltage = br.readSigned(16);
    _alarm = br.readUnsigned(16);
    uint16_t aux = br.readUnsigned(16);
    uint16_t auxMode = br.readUnsigned(2);
    int32_t batteryCurrent = br.readSigned(22);
    int32_t consumedAh = br.readSigned(20);
    uint16_t soc = br.readUnsigned(10);

    _remaningMins = remainingMins != 0xFFFF ? remainingMins : 0;
    _batteryVoltage = (batteryVoltage) != 0x7FFF
                          ? static_cast<float>(batteryVoltage) / 100
                          : NAN;  // 10 mV increments

    _auxMode =
        auxMode &
        0x03;  // 0=StarterVoltage, 1=MidPointVoltage, 2=Temperature, 3=Off

    switch (_auxMode) {
      case 0:  // Aux mode
        _aux =
            static_cast<float>(br.convert(aux, 16)) / 100;  // 10 mV increments
        break;
      case 1:                                  // Mid mode
        _aux = static_cast<float>(aux) / 100;  // 10 mV increments
        break;
      case 2:                                           // Temperature
        _aux = static_cast<float>(aux) / 100 - 273.15;  // Kelvin to Celcius
        break;
      case 3:  // Disabled
        _aux = NAN;
        break;
    }

    _batteryCurrent = batteryCurrent != 0x3FFFFF
                          ? static_cast<float>(batteryCurrent) / 1000
                          : NAN;
    _consumedAh =
        consumedAh != 0xFFFFF ? static_cast<float>(consumedAh) / 10 : NAN;
    _soc =
        soc != 0x3FF ? static_cast<float>(soc) / 10 : NAN;  // 0.1% increments

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
    doc["alarm_message"] = deviceAlarmReasonToString(getAlarm());

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
