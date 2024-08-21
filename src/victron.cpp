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

#include <victron.hpp>

String VictronDevice::deviceStateToString(uint8_t state) {
  switch ((VictronDeviceState)state) {
    case VictronDeviceState::Off:
      return "Off";
    case VictronDeviceState::LowPower:
      return "Low Power";
    case VictronDeviceState::Fault:
      return "Fault";
    case VictronDeviceState::Bulk:
      return "Bulk";
    case VictronDeviceState::Absorption:
      return "Absorption";
    case VictronDeviceState::Float:
      return "Float";
    case VictronDeviceState::Storage:
      return "Storage";
    case VictronDeviceState::EqualizeManual:
      return "Equalize Manual";
    case VictronDeviceState::Inverting:
      return "Inverting";
    case VictronDeviceState::PowerSupply:
      return "Power Supply";
    case VictronDeviceState::StartingUp:
      return "Starting Up";
    case VictronDeviceState::RepeatedAbsorption:
      return "Repeated Absorption";
    case VictronDeviceState::Recondition:
      return "Recondition";
    case VictronDeviceState::BatterySafe:
      return "Battery Safe";
    case VictronDeviceState::ExternalControl:
      return "External Control";
  }

  return "Unknown";
}

String VictronDevice::deviceChargerErrorToString(uint8_t error) {
  switch ((VictronChargerError)error) {
    case VictronChargerError::NoError:
      return "0 - No error";
    case VictronChargerError::TemperatureBatteryHigh:
      return "1 - Battery temperature too high";
    case VictronChargerError::VoltageHigh:
      return "2 - Battery voltage too high";
    case VictronChargerError::RemoteTemperatureA:
      return "3 - Remote temperature sensor failure (auto-reset)";
    case VictronChargerError::RemoteTemperatureB:
      return "4 - Remote temperature sensor failure (auto-reset)";
    case VictronChargerError::RemoteTemperatureC:
      return "5 - Remote temperature sensor failure (not auto-reset)";
    case VictronChargerError::RemoteBatteryA:
      return "6 - Remote battery voltage sense failure";
    case VictronChargerError::RemoteBatteryB:
      return "7 - Remote battery voltage sense failure";
    case VictronChargerError::RemoteBatteryC:
      return "8 - Remote battery voltage sense failure";
    case VictronChargerError::HighRipple:
      return "11 - Battery high ripple voltage";
    case VictronChargerError::TemperatureBatteryLow:
      return "14 - Battery temperature too low";
    case VictronChargerError::TemperatureCharger:
      return "17 - Charger temperature too high";
    case VictronChargerError::OverCurrent:
      return "18 - Charger over current";
    case VictronChargerError::BulkTime:
      return "20 - Bulk time limit exceeded";
    case VictronChargerError::CurrentSensor:
      return "21 - Current sensor issue (sensor bias/sensor broken)";
    case VictronChargerError::InternalTemperatureA:
      return "22 - Internal temperature sensor failure";
    case VictronChargerError::InternalTemperatureB:
      return "23 - Internal temperature sensor failure";
    case VictronChargerError::Fan:
      return "24 - Fan failure";
    case VictronChargerError::Overheated:
      return "26 - Terminals overheated";
    case VictronChargerError::ShortCircuit:
      return "27 - Charger short circuit";
    case VictronChargerError::ConverterIssue:
      return "28 - Power stage issue Converter issue (dual converter models "
             "only)";
    case VictronChargerError::OverCharge:
      return "29 - Over-Charge protection";
    case VictronChargerError::InputVoltage:
      return "33 - Input voltage too high (solar panel) PV over-voltage";
    case VictronChargerError::InputCurrent:
      return "34 - Input current too high (solar panel) PV over-current";
    case VictronChargerError::InputPower:
      return "35 - PV over-power";
    case VictronChargerError::InputShutdownVoltage:
      return "38 - Input shutdown (due to excessive battery voltage)";
    case VictronChargerError::InputShutdownCurrent:
      return "39 - Input shutdown (due to current flow during off mode)";
    case VictronChargerError::InputShutdownFailure:
      return "40 - PV Input failed to shutdown";
    case VictronChargerError::InverterShutdown41:
      return "41 - Inverter shutdown (PV isolation)";
    case VictronChargerError::InverterShutdown42:
      return "42 - Inverter shutdown (PV isolation)";
    case VictronChargerError::InverterShutdown43:
      return "43 - Inverter shutdown (Ground Fault)";
    case VictronChargerError::InverterOverload:
      return "50 - Inverter overload";
    case VictronChargerError::InverterTemperature:
      return "51 - Inverter temperature too high";
    case VictronChargerError::InverterPeakCurrent:
      return "52 - Inverter peak current";
    case VictronChargerError::InverterOutputVoltageA:
      return "53 - Inverter output voltage";
    case VictronChargerError::InverterOutputVoltageB:
      return "54 - Inverter output voltage";
    case VictronChargerError::InverterSelfTestA:
      return "55 - Inverter self test failed";
    case VictronChargerError::InverterSelfTestB:
      return "56 - Inverter self test failed";
    case VictronChargerError::InverterAC:
      return "57 - Inverter ac voltage on output";
    case VictronChargerError::InverterSelfTestC:
      return "58 - Inverter self test failed";
    case VictronChargerError::Communication:
      return "65 - Communication warning Lost communication with one of "
             "devices";
    case VictronChargerError::Synchronisation:
      return "66 - Incompatible device Synchronised charging device "
             "configuration issue";
    case VictronChargerError::Bms:
      return "67 - BMS Connection lost";
    case VictronChargerError::NetworkA:
      return "68 - Network misconfigured";
    case VictronChargerError::NetworkB:
      return "69 - Network misconfigured";
    case VictronChargerError::NetworkC:
      return "70 - Network misconfigured";
    case VictronChargerError::NetworkD:
      return "71 - Network misconfigured";
    case VictronChargerError::PvInputShutdown80:
      return "80 - PV Input shutdown";
    case VictronChargerError::PvInputShutdown81:
      return "81 - PV Input shutdown";
    case VictronChargerError::PvInputShutdown82:
      return "82 - PV Input shutdown";
    case VictronChargerError::PvInputShutdown83:
      return "83 - PV Input shutdown";
    case VictronChargerError::PvInputShutdown84:
      return "84 - PV Input shutdown";
    case VictronChargerError::PvInputShutdown85:
      return "85 - PV Input shutdown";
    case VictronChargerError::PvInputShutdown86:
      return "86 - PV Input shutdown";
    case VictronChargerError::PvInputShutdown87:
      return "87 - PV Input shutdown";
    case VictronChargerError::CpuTemperature:
      return "114 - CPU temperature too high";
    case VictronChargerError::CalibrationLost:
      return "116 - Factory calibration data lost";
    case VictronChargerError::Firmware:
      return "117 - Invalid/incompatible firmware";
    case VictronChargerError::Settings:
      return "119 - Settings data lost";
    case VictronChargerError::TesterFail:
      return "121 - Tester fail";
    case VictronChargerError::InternalDcVoltageA:
      return "200 - Internal DC voltage error";
    case VictronChargerError::InternalDcVoltageB:
      return "201 - Internal DC voltage error";
    case VictronChargerError::SelfTest:
      return "202 - PV residual current sensor self-test failure Internal "
             "GFCI sensor error";
    case VictronChargerError::InternalSupplyA:
      return "203 - Internal supply voltage error";
    case VictronChargerError::InternalSupplyB:
      return "205 - Internal supply voltage error";
    case VictronChargerError::InternalSupplyC:
      return "212 - Internal supply voltage error";
    case VictronChargerError::InternalSupplyD:
      return "215 - Internal supply voltage error";
  }

  return "Unknown error " + String(error);
}

String VictronDevice::offReasonToString(uint32_t offReason) {
  String s;

  if (offReason & 0x00000001) {
    s += "No input power. ";
  }
  if (offReason & 0x00000002) {
    s += "Switched off switch. ";
  }
  if (offReason & 0x00000004) {
    s += "Switched off register. ";
  }
  if (offReason & 0x00000008) {
    s += "Remote input. ";
  }
  if (offReason & 0x00000010) {
    s += "Protection active. ";
  }
  if (offReason & 0x00000001) {
    s += "No input power. ";
  }
  if (offReason & 0x00000020) {
    s += "Pay as you go out of credit. ";
  }
  if (offReason & 0x00000040) {
    s += "BMS. ";
  }
  if (offReason & 0x00000080) {
    s += "Engine shutdown. ";
  }
  if (offReason & 0x00000100) {
    s += "Analysing input voltage. ";
  }

  return s;
}

// EOF
