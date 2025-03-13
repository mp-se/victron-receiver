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

#ifndef SRC_VICTRON_AC_HPP_
#define SRC_VICTRON_AC_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronAcCharger : public VictronDevice {
  /*
   * Used for the following model numbers:
   * 0xA339: "Blue Smart IP65 Charger",
   *
   * TODO: Add other models that this should support...
   */
 private:
  uint8_t _state;
  uint8_t _error;
  float _voltage1, _voltage2, _voltage3;
  float _current1, _current2, _current3;
  float _temperatureC;
  float _currentAC;

 public:
  VictronAcCharger(const uint8_t* data, uint16_t model) {
    setBaseData("AC Charger", model, data);

    BitReader br(data, 21);

    _state = br.readUnsigned(8);
    _error = br.readUnsigned(8);
    uint16_t voltage1 = br.readUnsigned(13);
    uint16_t current1 = br.readUnsigned(11);
    uint16_t voltage2 = br.readUnsigned(13);
    uint16_t current2 = br.readUnsigned(11);
    uint16_t voltage3 = br.readUnsigned(13);
    uint16_t current3 = br.readUnsigned(11);
    uint16_t temperature = br.readUnsigned(7);
    uint16_t currentAC = br.readUnsigned(9);

    _voltage1 = (voltage1) != 0x1FFF ? static_cast<float>(voltage1) / 100
                                     : NAN;  // 10 mV increments
    _current1 = (current1) != 0x7FF ? static_cast<float>(current1) / 10
                                    : NAN;  // 10 mV increments
    _voltage2 = (voltage2) != 0x1FFF ? static_cast<float>(voltage2) / 100
                                     : NAN;  // 10 mV increments
    _current2 = (current2) != 0x7FF ? static_cast<float>(current2) / 10
                                    : NAN;  // 10 mV increments
    _voltage3 = (voltage3) != 0x1FFF ? static_cast<float>(voltage3) / 100
                                     : NAN;  // 10 mV increments
    _current3 = (current3) != 0x7FF ? static_cast<float>(current3) / 10
                                    : NAN;  // 10 mV increments

    _temperatureC =
        (temperature & 0x7F) != 0x7F ? temperature & 0x7F - 40 : NAN;
    _currentAC = (currentAC) != 0x1FF ? static_cast<float>(currentAC) / 10
                                      : NAN;  // 10 mV increments

    Log.notice(
        F("VIC : Victron %s (%x) battVolt1=%F V battCurr1=%F battVolt2=%F V "
          "battCurr2=%F battVolt3=%F V battCurr3=%F temp=%F acCurrent=%F" CR),
        getDeviceName().c_str(), getModelNo(), getVoltage1(), getCurrent1(),
        getVoltage2(), getCurrent2(), getVoltage3(), getCurrent3(),
        getTemperature(), getCurrentAC());
  }

  uint8_t getState() { return _state; }
  uint8_t getError() { return _error; }
  float getVoltage1() { return _voltage1; }
  float getCurrent1() { return _current1; }
  float getVoltage2() { return _voltage2; }
  float getCurrent2() { return _current2; }
  float getVoltage3() { return _voltage3; }
  float getCurrent3() { return _current3; }
  float getTemperature() { return _temperatureC; }
  float getCurrentAC() { return _currentAC; }

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

    if (!isnan(getVoltage1()))
      doc["battery_voltage1"] =
          serialized(String(getVoltage1(), DECIMALS_VOLTAGE));

    if (!isnan(getCurrent1()))
      doc["battery_current1"] =
          serialized(String(getCurrent1(), DECIMALS_CURRENT));

    if (!isnan(getVoltage2()))
      doc["battery_voltage2"] =
          serialized(String(getVoltage2(), DECIMALS_VOLTAGE));

    if (!isnan(getCurrent2()))
      doc["battery_current2"] =
          serialized(String(getCurrent2(), DECIMALS_CURRENT));

    if (!isnan(getVoltage3()))
      doc["battery_voltage3"] =
          serialized(String(getVoltage3(), DECIMALS_VOLTAGE));

    if (!isnan(getCurrent3()))
      doc["battery_current3"] =
          serialized(String(getCurrent3(), DECIMALS_CURRENT));

    if (!isnan(getTemperature()))
      doc["temperature"] =
          serialized(String(getTemperature(), DECIMALS_CURRENT));

    if (!isnan(getCurrentAC()))
      doc["ac_current"] = serialized(String(getCurrentAC(), DECIMALS_CURRENT));
  }
};

#endif  // SRC_VICTRON_AC_HPP_
