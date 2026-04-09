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

class BleData {
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
  void stop();
  bool isScanning();

  void setScanTime(int scanTime) { _scanTime = scanTime; }
  void setAllowActiveScan(bool activeScan) { _activeScan = activeScan; }

  int findBleData(String mac) {
    for (int i = 0; i < MAX_VICTRON_DEVICES; i++) {
      if (_victron[i].getMacAdress().equalsIgnoreCase(mac) ||
          _victron[i].getMacAdress() == "")
        return i;
    }
    return -1;
  }
  BleData& getBleData(int idx) { return _victron[idx]; }

 private:
  int _scanTime = 5;
  bool _activeScan = false;
  BLEScan* _bleScan = nullptr;
  BleDeviceCallbacks* _deviceCallbacks = nullptr;
  BleData _victron[MAX_VICTRON_DEVICES];
};

extern BleScanner myBleScanner;

#endif  // SRC_BLESCANNER_HPP_
