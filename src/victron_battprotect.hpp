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

#ifndef SRC_VICTRON_BATTPROTECT_HPP_
#define SRC_VICTRON_BATTPROTECT_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronBatteryProtect : public VictronDevice {
  /*
   * Used for the following model numbers:
   *
   * TODO: Add other models that this should support...
   */
 private:
  uint8_t _state, _outputState;
  uint8_t _error;
  uint16_t _alarm;
  uint16_t _warning;
  float _inputVoltage, _outputVoltage;
  uint32_t _offReason;

 public:
  VictronBatteryProtect(const uint8_t* data, uint16_t model) {
    setBaseData("Battery Protect", model, data);

    BitReader br(data, 21);

    _state = br.readUnsigned(8);        // 0xFF = N/A
    _outputState = br.readUnsigned(8);  // 0xFF = N/A
    _error = br.readUnsigned(8);        // 0xFF = N/A
    _alarm = br.readUnsigned(16);
    _warning = br.readUnsigned(16);

    uint16_t inputVoltage = br.readSigned(16);     // 0x7FFF = N/A
    uint16_t outputVoltage = br.readUnsigned(16);  // 0xFFFF = N/A

    _offReason = br.readUnsigned(32);

    _inputVoltage = (inputVoltage) != 0x7FFF
                        ? static_cast<float>(inputVoltage) / 100
                        : NAN;  // 0.01 A increments

    _outputVoltage = (outputVoltage) != 0xFFFF
                         ? static_cast<float>(outputVoltage) / 100
                         : NAN;  // 0.01 A increments

    Log.notice(F("VIC : Victron %s (%x) state=%d error=%d out-state=%d "
                 "alarm=%d warning=%d inputVolt=%F V outputVolt=%F V"
                 "offReason=%d" CR),
               getDeviceName().c_str(), getModelNo(), getState(), getError(),
               getOutputState(), getAlarm(), getWarning(), getInputVoltage(),
               getOutputVoltage(), getOffReasons());
  }

  uint8_t getState() { return _state; }
  uint8_t getOutputState() { return _outputState; }
  uint8_t getError() { return _error; }

  uint16_t getAlarm() { return _alarm; }
  uint16_t getWarning() { return _warning; }

  float getInputVoltage() { return _inputVoltage; }
  float getOutputVoltage() { return _outputVoltage; }

  uint32_t getOffReasons() { return _offReason; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    if (getState() != 0xFF) {
      doc["state"] = getState();
      doc["state_message"] = deviceStateToString(getState());
    }

    if (getOutputState() != 0xFF) {
      doc["output_state"] = getOutputState();
      // doc["output_state_message"] = deviceStateToString(getOutputState()); //
      // TODO(mpse): Find what values this can be
    }

    if (getError() != 0xFF) {
      doc["error"] = getError();
      doc["error_message"] = deviceChargerErrorToString(getError());
    }

    if (!isnan(getInputVoltage()))
      doc["input_voltage"] =
          serialized(String(getInputVoltage(), DECIMALS_VOLTAGE));

    if (!isnan(getOutputVoltage()))
      doc["output_voltage"] =
          serialized(String(getOutputVoltage(), DECIMALS_VOLTAGE));

    doc["alarm"] = getAlarm();
    doc["alarm_message"] = deviceAlarmReasonToString(getAlarm());
    doc["warning"] = getWarning();
    doc["warning_message"] = deviceAlarmReasonToString(getWarning());
    doc["off_reason"] = getOffReasons();
    doc["off_reason_message"] = offReasonToString(getOffReasons());
  }
};

#endif  // SRC_VICTRON_BATTPROTECT_HPP_
