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

#ifndef SRC_VICTRON_DCDC_HPP_
#define SRC_VICTRON_DCDC_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronDcDcCharger : public VictronDevice {
  /*
   * Used for the following model numbers:
   * 0xA3C0: "Orion Smart 12V|12V-18A Isolated DC-DC Charger",
   * 0xA3C8: "Orion Smart 12V|12V-30A Isolated DC-DC Charger",
   * 0xA3D0: "Orion Smart 12V|12V-30A Non-isolated DC-DC Charger",
   * 0xA3C1: "Orion Smart 12V|24V-10A Isolated DC-DC Charger",
   * 0xA3C9: "Orion Smart 12V|24V-15A Isolated DC-DC Charger",
   * 0xA3D1: "Orion Smart 12V|24V-15A Non-isolated DC-DC Charger",
   * 0xA3C2: "Orion Smart 24V|12V-20A Isolated DC-DC Charger",
   * 0xA3CA: "Orion Smart 24V|12V-30A Isolated DC-DC Charger",
   * 0xA3D2: "Orion Smart 24V|12V-30A Non-isolated DC-DC Charger",
   * 0xA3C3: "Orion Smart 24V|24V-12A Isolated DC-DC Charger",
   * 0xA3CB: "Orion Smart 24V|24V-17A Isolated DC-DC Charger",
   * 0xA3D3: "Orion Smart 24V|24V-17A Non-isolated DC-DC Charger",
   * 0xA3C4: "Orion Smart 24V|48V-6A Isolated DC-DC Charger",
   * 0xA3CC: "Orion Smart 24V|48V-8.5A Isolated DC-DC Charger",
   * 0xA3C5: "Orion Smart 48V|12V-20A Isolated DC-DC Charger",
   * 0xA3CD: "Orion Smart 48V|12V-30A Isolated DC-DC Charger",
   * 0xA3C6: "Orion Smart 48V|24V-12A Isolated DC-DC Charger",
   * 0xA3CE: "Orion Smart 48V|24V-16A Isolated DC-DC Charger",
   * 0xA3C7: "Orion Smart 48V|48V-6A Isolated DC-DC Charger",
   * 0xA3CF: "Orion Smart 48V|48V-8.5A Isolated DC-DC Charger",
   */
 private:
  typedef struct {
    uint8_t state;
    uint8_t error;
    uint16_t inputVoltage;
    int16_t outputVoltage;
    uint32_t offReason;
  } __attribute__((packed)) VictronData;

  uint8_t _state;
  uint8_t _error;
  float _inputVoltage;
  float _outputVoltage;
  uint32_t _offReason;

 public:
  VictronDcDcCharger(const uint8_t* data, uint16_t model) {
    VictronDcDcCharger::VictronData* _data =
        (VictronDcDcCharger::VictronData*)data;
    uint32_t v;

    setBaseData("Smart DC-DC Charger", model, data);

    _inputVoltage = _data->inputVoltage != 0xFFFF
                        ? static_cast<float>(_data->inputVoltage) / 100
                        : 0;  // 10 mV increments
    _outputVoltage = _data->outputVoltage != 0X7FFF
                         ? static_cast<float>(_data->outputVoltage) / 100
                         : 0;  // 10 mV increments
    _state = _data->state != 0xFF ? _data->state : 0;
    _error = _data->error != 0xFF ? _data->error : 0;
    _offReason = _data->offReason != 0xFFFFFFFF ? _data->offReason : 0;

    Log.notice(F("VIC : Victron %s (%x) input=%F V output=%F V state=%d "
                 "error=%d, off=%d" CR),
               getDeviceName().c_str(), getModelNo(), getInputVoltage(),
               getOutputVoltage(), getState(), getError(), getOffReasons());
  }

  float getInputVoltage() { return _inputVoltage; }
  float getOutputVoltage() { return _outputVoltage; }

  uint8_t getState() { return _state; }
  uint8_t getError() { return _error; }

  uint32_t getOffReasons() { return _offReason; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    doc["state"] = getState();
    doc["state_message"] = deviceStateToString(getState());
    doc["error"] = getError();
    doc["error_message"] = deviceChargerErrorToString(getError());
    doc["off_reason"] = getOffReasons();
    doc["off_reason_message"] = offReasonToString(getOffReasons());
    doc["input_voltage"] =
        serialized(String(getInputVoltage(), DECIMALS_VOLTAGE));
    doc["output_voltage"] =
        serialized(String(getOutputVoltage(), DECIMALS_VOLTAGE));
  }
};

#endif  // SRC_VICTRON_DCDC_HPP_
