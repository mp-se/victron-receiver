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


#ifndef SRC_VICTRON_COMMON_HPP_
#define SRC_VICTRON_COMMON_HPP_

#include <Arduino.h>
#include <ArduinoJson.h>

#include <bitreader.hpp>
#include <cstdio>
#include <log.hpp>

enum VictronDeviceType {
  Test = 0x00,
  SolarCharger = 0x01,    // Supported
  BatteryMonitor = 0x02,  // Supported
  Inverter = 0x03,
  DcDcConverter = 0x04,  // Supported
  SmartLithium = 0x05,
  InverterRS = 0x06,
  GxDevice = 0x07,
  AcCharger = 0x08,       // Supported
  BatteryProtect = 0x09,  // Supported
  LynxSmartBMS = 0x0a,
  MultiRS = 0x0b,
  VeBus = 0x0c,
  DcEnergyMeter = 0x0d
};

namespace VictronDeviceState {
enum State {
  Off = 0,
  LowPower = 1,
  Fault = 2,
  Bulk = 3,
  Absorption = 4,
  Float = 5,
  Storage = 6,
  EqualizeManual = 7,
  Inverting = 9,
  PowerSupply = 11,
  StartingUp = 245,
  RepeatedAbsorption = 246,
  Recondition = 247,
  BatterySafe = 248,
  Active = 249,
  ExternalControl = 252
};
}  // namespace VictronDeviceState

namespace VictronAlarmReason {
enum AlarmReason {
  NoAlarm = 0,
  LowVoltage = 1,
  HighVoltage = 2,
  LowSoc = 4,
  LowStarterVoltage = 8,
  HighStarterVoltage = 16,
  LowTemperature = 32,
  HighTemperature = 64,
  MidVoltage = 128,
  Overload = 256,
  DcRipple = 512,
  LowVACOut = 1024,
  HighVACOut = 2048,
  ShortCircuit = 4096,
  BmsLockout = 8192,
};
}  // namespace VictronAlarmReason

namespace VictronChargerError {
enum ChargerError {
  NoError = 0,
  TemperatureBatteryHigh = 1,
  VoltageHigh = 2,
  RemoteTemperatureA = 3,
  RemoteTemperatureB = 4,
  RemoteTemperatureC = 5,
  RemoteBatteryA = 6,
  RemoteBatteryB = 7,
  RemoteBatteryC = 8,
  HighRipple = 11,
  TemperatureBatteryLow = 14,
  TemperatureCharger = 17,
  OverCurrent = 18,
  BulkTime = 20,
  CurrentSensor = 21,
  InternalTemperatureA = 22,
  InternalTemperatureB = 23,
  Fan = 24,
  Overheated = 26,
  ShortCircuit = 27,
  ConverterIssue = 28,
  OverCharge = 29,
  InputVoltage = 33,
  InputCurrent = 34,
  InputPower = 35,
  InputShutdownVoltage = 38,
  InputShutdownCurrent = 39,
  InputShutdownFailure = 40,
  InverterShutdown41 = 41,
  InverterShutdown42 = 42,
  InverterShutdown43 = 43,
  InverterOverload = 50,
  InverterTemperature = 51,
  InverterPeakCurrent = 52,
  InverterOutputVoltageA = 53,
  InverterOutputVoltageB = 54,
  InverterSelfTestA = 55,
  InverterSelfTestB = 56,
  InverterAC = 57,
  InverterSelfTestC = 58,
  Communication = 65,
  Synchronisation = 66,
  Bms = 67,
  NetworkA = 68,
  NetworkB = 69,
  NetworkC = 70,
  NetworkD = 71,
  PvInputShutdown80 = 80,
  PvInputShutdown81 = 81,
  PvInputShutdown82 = 82,
  PvInputShutdown83 = 83,
  PvInputShutdown84 = 84,
  PvInputShutdown85 = 85,
  PvInputShutdown86 = 86,
  PvInputShutdown87 = 87,
  CpuTemperature = 114,
  CalibrationLost = 116,
  Firmware = 117,
  Settings = 119,
  TesterFail = 121,
  InternalDcVoltageA = 200,
  InternalDcVoltageB = 201,
  SelfTest = 202,
  InternalSupplyA = 203,
  InternalSupplyB = 205,
  InternalSupplyC = 212,
  InternalSupplyD = 215,
};
}  // namespace VictronChargerError

// Must use the "packed" attribute to make sure the compiler doesn't add any
// padding to deal with word alignment.
typedef struct {
  uint16_t vendorID;
  uint8_t beaconType;
  uint8_t unknown;
  uint16_t model;
  uint8_t victronRecordType;
  uint16_t nonceDataCounter;
  uint8_t encryptKeyMatch;
  uint8_t victronEncryptedData[21];
  uint8_t nullPad;
} __attribute__((packed)) VictronManufacturerData;

class VictronDevice {
 protected:
  String _deviceName = "";
  uint16_t _modelNo = 0;
  uint8_t _data[21];

  void setBaseData(const char* deviceName, uint16_t modelNo,
                   const uint8_t* data) {
    _deviceName = deviceName;
    _modelNo = modelNo;
    memcpy(&_data[0], data, sizeof(_data));
  }

  String deviceStateToString(uint8_t state);
  String deviceChargerErrorToString(uint8_t error);
  String offReasonToString(uint32_t offReason);
  String deviceAlarmReasonToString(uint16_t alarm);

 public:
  VictronDevice() {}
  virtual ~VictronDevice() {}

  virtual const String& getDeviceName() { return _deviceName; }
  virtual uint16_t getModelNo() { return _modelNo; }

  virtual void toJson(JsonObject& doc) {
    doc["name"] = _deviceName;

    char buf[10];
    snprintf(&buf[0], sizeof(buf), "0x%04X", _modelNo);
    doc["model"] = buf;

    String s;

    for (int i = 0; i < sizeof(_data); i++) {
      snprintf(&buf[0], sizeof(buf), "0x%02X", _data[i]);
      if (i > 0)
        s += "," + String(buf);
      else
        s += String(buf);
    }
    doc["decrypted_data"] = s;
  }
};

class VictronUnknown : public VictronDevice {
 private:
  uint16_t _vendorID;
  uint8_t _beaconType;
  uint8_t _victronRecordType;

 public:
  VictronUnknown(const uint8_t* data, uint16_t model, uint16_t vendorID,
                 uint8_t beaconType, uint8_t victronRecordType) {
    setBaseData("Unknown", model, reinterpret_cast<const uint8_t*>(data));
    _vendorID = vendorID;
    _beaconType = beaconType;
    _victronRecordType = victronRecordType;
  }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);
    String s;
    char buf[12];

    for (int i = 0; i < sizeof(_data); i++) {
      snprintf(&buf[0], sizeof(buf), "0x%02X", _data[i]);
      if (i > 0)
        s += "," + String(buf);
      else
        s += String(buf);
    }
    doc["decrypted_data"] = s;

    snprintf(&buf[0], sizeof(buf), "0x%04X", _vendorID);
    doc["vendor_id"] = buf;
    snprintf(&buf[0], sizeof(buf), "0x%02X", _beaconType);
    doc["beacon_type"] = buf;
    snprintf(&buf[0], sizeof(buf), "0x%02X", _victronRecordType);
    doc["victron_record_type"] = buf;
  }
};

#endif  // SRC_VICTRON_COMMON_HPP_
