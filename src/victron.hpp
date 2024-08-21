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

#ifndef SRC_VICTRON_HPP_
#define SRC_VICTRON_HPP_

#include <ArduinoJson.h>

#include <Log.hpp>
#include <main.hpp>

enum VictronDeviceType {
  Test = 0x00,
  SolarCharger = 0x01,
  BatteryMonitor = 0x02,
  Inverter = 0x03,
  DcDcConverter = 0x04,
  SmartLithium = 0x05,
  InverterRS = 0x06,
  GxDevice = 0x07,
  AcCharger = 0x08,
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

#define BLE_VICTRON_DEBUG

#if defined(BLE_VICTRON_DEBUG)
    String s;

    for (int i = 0; i < sizeof(_data); i++) {
      snprintf(&buf[0], sizeof(buf), "0x%02X", _data[i]);
      if (i > 0)
        s += "," + String(buf);
      else
        s += String(buf);
    }
    doc["decrypted_data"] = s;
#endif
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

class VictronSmartBatteryMonitor : public VictronDevice {
  /*
   * Used for the following model numbers:
   * 0xA3A4: "Smart Battery Sense",
   * 0xA3A5: "Smart Battery Sense",
   */
 private:
  typedef struct {
    uint16_t unused;
    int16_t batteryVoltage;
    uint16_t alarm;
    uint16_t temperature;
    uint8_t unused2[7];
  } __attribute__((packed)) VictronData;

  float _batteryVoltage;
  float _temperatureC;

 public:
  VictronSmartBatteryMonitor(const uint8_t* data, uint16_t model) {
    VictronSmartBatteryMonitor::VictronData* _data =
        (VictronSmartBatteryMonitor::VictronData*)data;
    uint32_t v;

    setBaseData("Smart Battery Monitor", model, data);

    _batteryVoltage = _data->batteryVoltage != 0x7FFF
                          ? static_cast<float>(_data->batteryVoltage) / 100
                          : 0;  // 10 mV increments
    _temperatureC = (static_cast<float>(_data->temperature) / 100) -
                    273.15;  // Value of the temperature (K)

    Log.notice(F("VIC : Victron %s (%x) volt=%F V temp=%F C" CR),
               getDeviceName().c_str(), getModelNo(), getBatteryVoltage(),
               getTemperatureC());
  }

  float getBatteryVoltage() { return _batteryVoltage; }
  float getTemperatureC() { return _temperatureC; }

  void toJson(JsonObject& doc) {
    VictronDevice::toJson(doc);

    doc["battery_voltage"] =
        serialized(String(getBatteryVoltage(), DECIMALS_VOLTAGE));
    doc["temperature"] = serialized(String(getTemperatureC(), DECIMALS_TEMP));
  }
};

class VictronSmartDcDcCharger : public VictronDevice {
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
  VictronSmartDcDcCharger(const uint8_t* data, uint16_t model) {
    VictronSmartDcDcCharger::VictronData* _data =
        (VictronSmartDcDcCharger::VictronData*)data;
    uint32_t v;

    setBaseData("Orion Smart DC-DC Charger", model, data);

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

    doc["input_voltage"] =
        serialized(String(getInputVoltage(), DECIMALS_VOLTAGE));
    doc["output_voltage"] =
        serialized(String(getOutputVoltage(), DECIMALS_VOLTAGE));
    doc["state_message"] = deviceStateToString(_state);
    doc["error"] = _error;
    doc["error_message"] = deviceChargerErrorToString(_error);
    doc["off_reason"] = _offReason;
    doc["off_reason_message"] = offReasonToString(_offReason);
  }
};

#endif  // SRC_VICTRON_HPP_
