/*
MIT License

Copyright (c) 2024 Magnus

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

#ifndef SRC_BLESCANNER_HPP_
#define SRC_BLESCANNER_HPP_

#include <ArduinoJson.h>

#include <ArduinoLog.hpp>
#include <config.hpp>
#include <victron.hpp>

#undef LOG_LEVEL_ERROR
#undef LOG_LEVEL_INFO

#include <NimBLEAdvertisedDevice.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <NimBLEUtils.h>

class BleDeviceCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
};

class VictronBleData {
 private:
  bool _updated = false;
  struct tm _timeinfoUpdated;
  uint32_t _timeUpdated = 0;
  uint32_t _timePushed = 0;
  String _json;
  String _mac;
  String _name;

 public:
  void clearUpdate() { _updated = false; }

  void setUpdated() {
    _updated = true;
    _timeUpdated = millis();
    getLocalTime(&_timeinfoUpdated);
  }

  void setPushed() {
    _updated = false;
    _timePushed = millis();
  }

  String& getName() { return _name; }
  void setName(String& n) { _name = n; }
  String& getMacAdress() { return _mac; }
  void setMacAdress(String& m) { _mac = m; }
  String& getJson() { return _json; }
  void setJson(String& j) { _json = j; }
  bool getUpdated() { return _updated; }
  const tm& getTimeUpdated() { return _timeinfoUpdated; }
  uint32_t getUpdateAge() { return (millis() - _timeUpdated) / 1000; }
  uint32_t getPushAge() { return (millis() - _timePushed) / 1000; }
};

class BleScanner {
 public:
  BleScanner();
  void init();
  void deInit();

  bool scan();

  void setScanTime(int scanTime) { _scanTime = scanTime; }
  void setAllowActiveScan(bool activeScan) { _activeScan = activeScan; }

  int findVictronBleData(String mac) {
    for (int i = 0; i < MAX_VICTRON_DEVICES; i++)
      if (_victron[i].getMacAdress() == mac || _victron[i].getMacAdress() == "")
        return i;
    return -1;
  }
  VictronBleData& getVictronBleData(int idx) { return _victron[idx]; }

 private:
  int _scanTime = 5;
  bool _activeScan = false;
  BLEScan* _bleScan = nullptr;
  BleDeviceCallbacks* _deviceCallbacks = nullptr;
  VictronBleData _victron[MAX_VICTRON_DEVICES];
};

extern BleScanner bleScanner;

#endif  // SRC_BLESCANNER_HPP_
