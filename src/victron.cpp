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
  switch (state) {
    case VictronDeviceState::State::Off:
      return "Off";
    case VictronDeviceState::State::LowPower:
      return "Low Power";
    case VictronDeviceState::State::Fault:
      return "Fault";
    case VictronDeviceState::State::Bulk:
      return "Bulk";
    case VictronDeviceState::State::Absorption:
      return "Absorption";
    case VictronDeviceState::State::Float:
      return "Float";
    case VictronDeviceState::State::Storage:
      return "Storage";
    case VictronDeviceState::State::EqualizeManual:
      return "Equalize Manual";
    case VictronDeviceState::State::Inverting:
      return "Inverting";
    case VictronDeviceState::State::PowerSupply:
      return "Power Supply";
    case VictronDeviceState::State::StartingUp:
      return "Starting Up";
    case VictronDeviceState::State::RepeatedAbsorption:
      return "Repeated Absorption";
    case VictronDeviceState::State::Recondition:
      return "Recondition";
    case VictronDeviceState::State::BatterySafe:
      return "Battery Safe";
    case VictronDeviceState::State::Active:
      return "Active";
    case VictronDeviceState::State::ExternalControl:
      return "External Control";
  }

  return "";
}

String VictronDevice::deviceAlarmReasonToString(uint16_t alarm) {
  switch (alarm) {
    case VictronAlarmReason::AlarmReason::NoAlarm:
      return "No alarm";
    case VictronAlarmReason::AlarmReason::LowVoltage:
      return "Low voltage";
    case VictronAlarmReason::AlarmReason::HighVoltage:
      return "High voltage";
    case VictronAlarmReason::AlarmReason::LowSoc:
      return "Low SOC";
    case VictronAlarmReason::AlarmReason::LowStarterVoltage:
      return "Low starter voltage";
    case VictronAlarmReason::AlarmReason::HighStarterVoltage:
      return "High starter voltage";
    case VictronAlarmReason::AlarmReason::LowTemperature:
      return "Low temperature";
    case VictronAlarmReason::AlarmReason::HighTemperature:
      return "High temperature";
    case VictronAlarmReason::AlarmReason::MidVoltage:
      return "Mid voltage";
    case VictronAlarmReason::AlarmReason::Overload:
      return "Overload";
    case VictronAlarmReason::AlarmReason::DcRipple:
      return "DC ripple";
    case VictronAlarmReason::AlarmReason::LowVACOut:
      return "Low VAC out";
    case VictronAlarmReason::AlarmReason::HighVACOut:
      return "High VAC out";
    case VictronAlarmReason::AlarmReason::ShortCircuit:
      return "Short circuit";
    case VictronAlarmReason::AlarmReason::BmsLockout:
      return "BMS lockout";
  }

  return "";
}

String VictronDevice::deviceChargerErrorToString(uint8_t error) {
  switch (error) {
    case VictronChargerError::ChargerError::NoError:
      return "0 - No error";
    case VictronChargerError::ChargerError::TemperatureBatteryHigh:
      return "1 - Battery temperature too high";
    case VictronChargerError::ChargerError::VoltageHigh:
      return "2 - Battery voltage too high";
    case VictronChargerError::ChargerError::RemoteTemperatureA:
      return "3 - Remote temperature sensor failure (auto-reset)";
    case VictronChargerError::ChargerError::RemoteTemperatureB:
      return "4 - Remote temperature sensor failure (auto-reset)";
    case VictronChargerError::ChargerError::RemoteTemperatureC:
      return "5 - Remote temperature sensor failure (not auto-reset)";
    case VictronChargerError::ChargerError::RemoteBatteryA:
      return "6 - Remote battery voltage sense failure";
    case VictronChargerError::ChargerError::RemoteBatteryB:
      return "7 - Remote battery voltage sense failure";
    case VictronChargerError::ChargerError::RemoteBatteryC:
      return "8 - Remote battery voltage sense failure";
    case VictronChargerError::ChargerError::HighRipple:
      return "11 - Battery high ripple voltage";
    case VictronChargerError::ChargerError::TemperatureBatteryLow:
      return "14 - Battery temperature too low";
    case VictronChargerError::ChargerError::TemperatureCharger:
      return "17 - Charger temperature too high";
    case VictronChargerError::ChargerError::OverCurrent:
      return "18 - Charger over current";
    case VictronChargerError::ChargerError::BulkTime:
      return "20 - Bulk time limit exceeded";
    case VictronChargerError::ChargerError::CurrentSensor:
      return "21 - Current sensor issue (sensor bias/sensor broken)";
    case VictronChargerError::ChargerError::InternalTemperatureA:
      return "22 - Internal temperature sensor failure";
    case VictronChargerError::ChargerError::InternalTemperatureB:
      return "23 - Internal temperature sensor failure";
    case VictronChargerError::ChargerError::Fan:
      return "24 - Fan failure";
    case VictronChargerError::ChargerError::Overheated:
      return "26 - Terminals overheated";
    case VictronChargerError::ChargerError::ShortCircuit:
      return "27 - Charger short circuit";
    case VictronChargerError::ChargerError::ConverterIssue:
      return "28 - Power stage issue Converter issue (dual converter models "
             "only)";
    case VictronChargerError::ChargerError::OverCharge:
      return "29 - Over-Charge protection";
    case VictronChargerError::ChargerError::InputVoltage:
      return "33 - Input voltage too high (solar panel) PV over-voltage";
    case VictronChargerError::ChargerError::InputCurrent:
      return "34 - Input current too high (solar panel) PV over-current";
    case VictronChargerError::ChargerError::InputPower:
      return "35 - PV over-power";
    case VictronChargerError::ChargerError::InputShutdownVoltage:
      return "38 - Input shutdown (due to excessive battery voltage)";
    case VictronChargerError::ChargerError::InputShutdownCurrent:
      return "39 - Input shutdown (due to current flow during off mode)";
    case VictronChargerError::ChargerError::InputShutdownFailure:
      return "40 - PV Input failed to shutdown";
    case VictronChargerError::ChargerError::InverterShutdown41:
      return "41 - Inverter shutdown (PV isolation)";
    case VictronChargerError::ChargerError::InverterShutdown42:
      return "42 - Inverter shutdown (PV isolation)";
    case VictronChargerError::ChargerError::InverterShutdown43:
      return "43 - Inverter shutdown (Ground Fault)";
    case VictronChargerError::ChargerError::InverterOverload:
      return "50 - Inverter overload";
    case VictronChargerError::ChargerError::InverterTemperature:
      return "51 - Inverter temperature too high";
    case VictronChargerError::ChargerError::InverterPeakCurrent:
      return "52 - Inverter peak current";
    case VictronChargerError::ChargerError::InverterOutputVoltageA:
      return "53 - Inverter output voltage";
    case VictronChargerError::ChargerError::InverterOutputVoltageB:
      return "54 - Inverter output voltage";
    case VictronChargerError::ChargerError::InverterSelfTestA:
      return "55 - Inverter self test failed";
    case VictronChargerError::ChargerError::InverterSelfTestB:
      return "56 - Inverter self test failed";
    case VictronChargerError::ChargerError::InverterAC:
      return "57 - Inverter ac voltage on output";
    case VictronChargerError::ChargerError::InverterSelfTestC:
      return "58 - Inverter self test failed";
    case VictronChargerError::ChargerError::Communication:
      return "65 - Communication warning Lost communication with one of "
             "devices";
    case VictronChargerError::ChargerError::Synchronisation:
      return "66 - Incompatible device Synchronised charging device "
             "configuration issue";
    case VictronChargerError::ChargerError::Bms:
      return "67 - BMS Connection lost";
    case VictronChargerError::ChargerError::NetworkA:
      return "68 - Network misconfigured";
    case VictronChargerError::ChargerError::NetworkB:
      return "69 - Network misconfigured";
    case VictronChargerError::ChargerError::NetworkC:
      return "70 - Network misconfigured";
    case VictronChargerError::ChargerError::NetworkD:
      return "71 - Network misconfigured";
    case VictronChargerError::ChargerError::PvInputShutdown80:
      return "80 - PV Input shutdown";
    case VictronChargerError::ChargerError::PvInputShutdown81:
      return "81 - PV Input shutdown";
    case VictronChargerError::ChargerError::PvInputShutdown82:
      return "82 - PV Input shutdown";
    case VictronChargerError::ChargerError::PvInputShutdown83:
      return "83 - PV Input shutdown";
    case VictronChargerError::ChargerError::PvInputShutdown84:
      return "84 - PV Input shutdown";
    case VictronChargerError::ChargerError::PvInputShutdown85:
      return "85 - PV Input shutdown";
    case VictronChargerError::ChargerError::PvInputShutdown86:
      return "86 - PV Input shutdown";
    case VictronChargerError::ChargerError::PvInputShutdown87:
      return "87 - PV Input shutdown";
    case VictronChargerError::ChargerError::CpuTemperature:
      return "114 - CPU temperature too high";
    case VictronChargerError::ChargerError::CalibrationLost:
      return "116 - Factory calibration data lost";
    case VictronChargerError::ChargerError::Firmware:
      return "117 - Invalid/incompatible firmware";
    case VictronChargerError::ChargerError::Settings:
      return "119 - Settings data lost";
    case VictronChargerError::ChargerError::TesterFail:
      return "121 - Tester fail";
    case VictronChargerError::ChargerError::InternalDcVoltageA:
      return "200 - Internal DC voltage error";
    case VictronChargerError::ChargerError::InternalDcVoltageB:
      return "201 - Internal DC voltage error";
    case VictronChargerError::ChargerError::SelfTest:
      return "202 - PV residual current sensor self-test failure Internal "
             "GFCI sensor error";
    case VictronChargerError::ChargerError::InternalSupplyA:
      return "203 - Internal supply voltage error";
    case VictronChargerError::ChargerError::InternalSupplyB:
      return "205 - Internal supply voltage error";
    case VictronChargerError::ChargerError::InternalSupplyC:
      return "212 - Internal supply voltage error";
    case VictronChargerError::ChargerError::InternalSupplyD:
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
