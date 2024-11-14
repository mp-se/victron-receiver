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

#ifndef SRC_VICTRON_COMMON_HPP_
#define SRC_VICTRON_COMMON_HPP_

#include <Arduino.h>
#include <ArduinoJson.h>

enum VictronDeviceType {
  Test = 0x00,
  SolarCharger = 0x01,    // Supported
  BatteryMonitor = 0x02,  // Supported
  Inverter = 0x03,
  DcDcConverter = 0x04,  // Supported
  SmartLithium = 0x05,
  InverterRS = 0x06,
  GxDevice = 0x07,
  AcCharger = 0x08,  // Supported
  SmartBatteryProtect = 0x09,
  LynxSmartBMS = 0x0a,
  MultiRS = 0x0b,
  VeBus = 0x0c,
  DcEnergyMeter = 0x0d
};

enum VictronDeviceState {
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
  ExternalControl = 252
};

enum VictronChargerError {
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
