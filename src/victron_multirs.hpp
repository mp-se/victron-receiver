/*
MIT License

Copyright (c) 2025 Magnus

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

#ifndef SRC_VICTRON_MULTIRS_HPP_
#define SRC_VICTRON_MULTIRS_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronMultiRS : public VictronDevice {
  /*
   * Used for the following model numbers:
   *
   * TODO: Add other models that this should support...
   */
 private:
  uint8_t _state;
  uint8_t _error;
  float _batteryVoltage, _batteryCurrent;
  uint8_t _activeAcIn;
  float _activeAcInPower, _acOutPower, _pvPower, _yieldToday;

 public:
  VictronMultiRS(const uint8_t* data, uint16_t model) {
    setBaseData("Multi RS", model, data);

    BitReader br(data, 21);

    _state = br.readUnsigned(8);  // 0xFF = N/A
    _error = br.readUnsigned(8);  // 0xFF = N/A
    uint16_t batteryCurrent = br.readSigned(16);
    int16_t batteryVoltage = br.readUnsigned(14);
    _activeAcIn = br.readUnsigned(2);
    int16_t activeAcInPower = br.readSigned(16);
    int16_t acOutPower = br.readSigned(16);
    uint16_t pvPower = br.readUnsigned(16);
    uint16_t yieldToday = br.readUnsigned(16);

    _batteryCurrent = (batteryCurrent) != 0x7FFF
                          ? static_cast<float>(batteryCurrent) / 10
                          : NAN;  // 0.1 A increments
    _batteryVoltage = (batteryVoltage & 0x7FFF) != 0x7FFF
                          ? static_cast<float>(batteryVoltage & 0x7FFF) / 100
                          : NAN;  // 10 mV increments

    _activeAcInPower = activeAcInPower != 0x7FFF
                           ? static_cast<float>(activeAcInPower)
                           : NAN;  // 1W increments

    _acOutPower = acOutPower != 0x7FFF ? static_cast<float>(acOutPower)
                                       : NAN;  // 1W increments

    _pvPower =
        pvPower != 0xFFFF ? static_cast<float>(pvPower) : NAN;  // 1W increments

    _yieldToday = yieldToday != 0xFFFF ? static_cast<float>(yieldToday) / 100
                                       : NAN;  // 0.01 kWh increments

    Log.notice(F("VIC : Victron %s (%x) state=%d error=%d battVolt=%F V "
                 "battCurrent=%F acIn=%d "
                 "acInPower=%F acOutPower=%F, pvPower=%F, yieldToday=%F" CR),
               getDeviceName().c_str(), getModelNo(), getState(), getError(),
               getBatteryVoltage(), getBatteryCurrent(), getActiveAcIn(),
               getActiveAcInPower(), getAcOutPower(), getPvPower(),
               getYieldToday());
  }

  uint8_t getState() { return _state; }
  uint8_t getError() { return _error; }

  float getBatteryVoltage() { return _batteryVoltage; }
  float getBatteryCurrent() { return _batteryCurrent; }
  uint8_t getActiveAcIn() { return _activeAcIn; }
  float getActiveAcInPower() { return _activeAcInPower; }
  float getAcOutPower() { return _acOutPower; }
  float getYieldToday() { return _yieldToday; }
  float getPvPower() { return _pvPower; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    if (getState() != 0xFF) {
      doc["state"] = getState();
      doc["state_message"] = deviceStateToString(getState());
    }

    if (getError() != 0xFF) {
      doc["error"] = getError();
      doc["error_message"] = deviceChargerErrorToString(getError());
    }

    if (!isnan(getBatteryVoltage()))
      doc["battery_voltage"] =
          serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));

    if (!isnan(getBatteryCurrent()))
      doc["battery_current"] =
          serialized(String(getBatteryCurrent(), DECIMALS_CURRENT));

    doc["active_ac_in"] = getActiveAcIn();

    if (!isnan(getPvPower()))
      doc["pv_power"] = serialized(String(getPvPower(), DECIMALS_POWER));

    if (!isnan(getActiveAcInPower()))
      doc["active_ac_in_power"] =
          serialized(String(getActiveAcInPower(), DECIMALS_POWER));

    if (!isnan(getAcOutPower()))
      doc["ac_out_power"] = serialized(String(getAcOutPower(), DECIMALS_POWER));

    if (!isnan(getYieldToday()))
      doc["yield_today"] = serialized(String(getYieldToday(), DECIMALS_POWER));
  }
};

#endif  // SRC_VICTRON_MULTIRS_HPP_
