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
  uint8_t _state;
  uint8_t _error;
  float _inputVoltage;
  float _outputVoltage;
  uint32_t _offReason;

 public:
  VictronDcDcCharger(const uint8_t* data, uint16_t model) {
    setBaseData("DC-DC Charger", model, data);

    BitReader br(data, 21);

    _state = br.readUnsigned(8);
    _error = br.readUnsigned(8);
    uint16_t inputVoltage = br.readUnsigned(16);
    int16_t outputVoltage = br.readSigned(16);
    _offReason = br.readUnsigned(32);

    _inputVoltage = inputVoltage != 0xFFFF
                        ? static_cast<float>(inputVoltage) / 100
                        : NAN;  // 10 mV increments
    _outputVoltage = outputVoltage != 0X7FFF
                         ? static_cast<float>(outputVoltage) / 100
                         : NAN;  // 10 mV increments

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
  }
};

#endif  // SRC_VICTRON_DCDC_HPP_
