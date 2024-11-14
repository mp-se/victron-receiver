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
  typedef struct {
    uint8_t state;
    uint8_t error;
    uint8_t channel1[3];
    uint8_t channel2[3];
    uint8_t channel3[3];
    uint8_t temperature;
    uint8_t currentAC;
    uint8_t unused[7];
  } __attribute__((packed)) VictronData;

  uint8_t _state;
  uint8_t _error;
  float _voltage1, _voltage2, _voltage3;
  float _current1, _current2, _current3;
  float _temperatureC;
  float _currentAC;

 public:
  VictronAcCharger(const uint8_t* data, uint16_t model) {
    VictronAcCharger::VictronData* _data =
        (VictronAcCharger::VictronData*)data;

    setBaseData("Smart AC Charger", model, data);

    _state = _data->state != 0xFF ? _data->state : 0;
    _error = _data->error != 0xFF ? _data->error : 0;

    uint32_t ch1 = static_cast<uint32_t>(_data->channel1[0]) |
                   static_cast<uint32_t>(_data->channel1[1]) << 8 |
                   static_cast<uint32_t>(_data->channel1[2]) << 16;
    uint16_t ch1V = ch1 & 0x1FFF;
    uint16_t ch1A = ch1 >> 13 & 0x7FF;

    _voltage1 = (ch1V) != 0x1FFF ? static_cast<float>(ch1V) / 100
                                 : 0;  // 10 mV increments
    _current1 = (ch1A) != 0x7FF ? static_cast<float>(ch1A) / 10
                                : 0;  // 10 mV increments

    uint32_t ch2 = static_cast<uint32_t>(_data->channel2[0]) |
                   static_cast<uint32_t>(_data->channel2[1]) << 8 |
                   static_cast<uint32_t>(_data->channel2[2]) << 16;
    uint16_t ch2V = ch2 & 0x1FFF;
    uint16_t ch2A = ch2 >> 13 & 0x7FF;

    _voltage2 = (ch2V) != 0x1FFF ? static_cast<float>(ch2V) / 100
                                 : 0;  // 10 mV increments
    _current2 = (ch2A) != 0x7FF ? static_cast<float>(ch2A) / 10
                                : 0;  // 10 mV increments

    uint32_t ch3 = static_cast<uint32_t>(_data->channel3[0]) |
                   static_cast<uint32_t>(_data->channel3[1]) << 8 |
                   static_cast<uint32_t>(_data->channel3[2]) << 16;
    uint16_t ch3V = ch3 & 0x1FFF;
    uint16_t ch3A = ch3 >> 13 & 0x7FF;

    _voltage3 = (ch3V) != 0x1FFF ? static_cast<float>(ch3V) / 100
                                 : 0;  // 10 mV increments
    _current3 = (ch3A) != 0x7FF ? static_cast<float>(ch3A) / 10
                                : 0;  // 10 mV increments

    _temperatureC = (_data->temperature & 0x7F) != 0x7F
                        ? _data->temperature & 0x7F - 40
                        : 0;

    uint16_t cur = static_cast<uint16_t>(_data->temperature & 0x01) |
                   static_cast<uint16_t>(_data->currentAC)
                       << 1;  // Borrow one unit from tempC
    _currentAC = (cur & 0x1FF) != 0x1FF ? cur : 0;

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

    doc["state"] = getState();
    doc["state_message"] = deviceStateToString(getState());
    doc["error"] = getError();
    doc["error_message"] = deviceChargerErrorToString(getError());
    doc["battery_voltage1"] =
        serialized(String(getVoltage1(), DECIMALS_VOLTAGE));
    doc["battery_current1"] =
        serialized(String(getCurrent1(), DECIMALS_CURRENT));
    doc["battery_voltage2"] =
        serialized(String(getVoltage2(), DECIMALS_VOLTAGE));
    doc["battery_current2"] =
        serialized(String(getCurrent2(), DECIMALS_CURRENT));
    doc["battery_voltage3"] =
        serialized(String(getVoltage3(), DECIMALS_VOLTAGE));
    doc["battery_current3"] =
        serialized(String(getCurrent3(), DECIMALS_CURRENT));
    doc["temperature"] = serialized(String(getTemperature(), DECIMALS_CURRENT));
    doc["ac_current"] = serialized(String(getCurrentAC(), DECIMALS_CURRENT));
  }
};

#endif  // SRC_VICTRON_AC_HPP_
