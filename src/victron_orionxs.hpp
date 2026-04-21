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

#ifndef SRC_VICTRON_ORIONXS_HPP_
#define SRC_VICTRON_ORIONXS_HPP_

#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronOrionXS : public VictronDevice {
  /*
   * Used for the following model numbers:
   * 0xA3F0: "Orion XS 12/12-50A DC-DC Battery Charger.",
   */
 private:
  uint8_t _state;
  uint8_t _error;
  float _inputVoltage, _inputCurrent;
  float _outputVoltage, _outputCurrent;
  uint32_t _offReason;

 public:
  VictronOrionXS(const uint8_t* data, uint16_t model) {
    setBaseData("Orion XS", model, data);

    BitReader br(data, 21);

    // Charge State:   0 - Off
    //                 3 - Bulk
    //                 4 - Absorption
    //                 5 - Float
    _state = br.readUnsigned(8);
    _error = br.readUnsigned(8);

    uint16_t inputVoltage = br.readUnsigned(16);
    uint16_t inputCurrent = br.readUnsigned(16);
    uint16_t outputVoltage = br.readUnsigned(16);
    uint16_t outputCurrent = br.readUnsigned(16);
    _offReason = br.readUnsigned(32);

    _outputVoltage = outputVoltage != 0X7FFF
                         ? static_cast<float>(outputVoltage) / 100
                         : NAN;  // 10 mV increments
    _outputCurrent = outputCurrent != 0x7FFF
                         ? static_cast<float>(outputCurrent) / 10
                         : NAN;  // 0.1 A increments
    _inputVoltage = inputVoltage != 0xFFFF
                        ? static_cast<float>(inputVoltage) / 100
                        : NAN;  // 10 mV increments
    _inputCurrent = inputCurrent != 0xFFFF
                        ? static_cast<float>(inputCurrent) / 10
                        : NAN;  // 0.1 A increments

    Log.notice(
        F("VIC : Victron %s (%x) input=%F V, %F A output=%F V, %F A state=%d "
          "error=%d, off=%d" CR),
        getDeviceName().c_str(), getModelNo(), getInputVoltage(),
        getInputCurrent(), getOutputVoltage(), getOutputCurrent(), getState(),
        getError(), getOffReasons());
  }

  float getInputVoltage() { return _inputVoltage; }
  float getOutputVoltage() { return _outputVoltage; }
  float getInputCurrent() { return _inputCurrent; }
  float getOutputCurrent() { return _outputCurrent; }

  uint8_t getState() { return _state; }
  uint8_t getError() { return _error; }

  uint32_t getOffReasons() { return _offReason; }

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

    doc["off_reason"] = getOffReasons();
    doc["off_reason_message"] = offReasonToString(getOffReasons());

    if (!isnan(getInputVoltage()))
      doc["input_voltage"] =
          serialized(String(getInputVoltage(), DECIMALS_VOLTAGE));

    if (!isnan(getOutputVoltage()))
      doc["output_voltage"] =
          serialized(String(getOutputVoltage(), DECIMALS_VOLTAGE));

    if (!isnan(getInputCurrent()))
      doc["input_current"] =
          serialized(String(getInputCurrent(), DECIMALS_CURRENT));

    if (!isnan(getOutputCurrent()))
      doc["output_current"] =
          serialized(String(getOutputCurrent(), DECIMALS_CURRENT));
  }
};

#endif  // SRC_VICTRON_ORIONXS_HPP_
