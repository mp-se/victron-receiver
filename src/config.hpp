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

#ifndef SRC_CONFIG_HPP_
#define SRC_CONFIG_HPP_

#include <baseconfig.hpp>
#include <utils.hpp>

constexpr auto MAX_VICTRON_DEVICES = 12;

struct VictronConfig {
  String name;  // Own name of device
  String mac;   // BLE mac adress as unique identifier
  String key;   // Encryption key for the 21 bytes to decrypt
};

class VictronReceiverConfig : public BaseConfig {
 private:
  String _timezone = "CET-1CEST,M3.5.0,M10.5.0/3";  // Stockholm TZ
  bool _bleActiveScan = false;
  int _bleScanTime = 4;      // seconds
  int _pushResendTime = 30;  // seconds
  VictronConfig _victron[MAX_VICTRON_DEVICES];

  void formatFileSystem();

 public:
  VictronReceiverConfig(String baseMDNS, String fileName);

  const char* getTimezone() const { return _timezone.c_str(); }
  void setTimezone(String s) {
    _timezone = s;
    _saveNeeded = true;
  }

  int getBleScanTime() const { return _bleScanTime; }
  void setBleScanTime(int v) {
    _bleScanTime = v;
    _saveNeeded = true;
  }

  int getPushResendTime() const { return _pushResendTime; }
  void setPushResendTime(int t) {
    _pushResendTime = t;
    _saveNeeded = true;
  }

  bool getBleActiveScan() const { return _bleActiveScan; }
  void setBleActiveScan(bool b) {
    _bleActiveScan = b;
    _saveNeeded = true;
  }

  VictronConfig getVictronConfig(int idx) const {
    if (idx >= MAX_VICTRON_DEVICES) return VictronConfig();

    return _victron[idx];
  }
  VictronConfig findVictronConfig(String mac) {
    for (int i = 0; i < MAX_VICTRON_DEVICES; i++) {
      if (_victron[i].mac.equalsIgnoreCase(mac)) return _victron[i];
    }

    return VictronConfig();
  }
  void setVictronConfig(VictronConfig c, int idx) {
    if (idx >= MAX_VICTRON_DEVICES) return;

    _victron[idx] = c;
    _saveNeeded = true;
  }

  // IO functions
  void createJson(JsonObject& doc) const override;
  void parseJson(JsonObject& doc) override;
};

extern VictronReceiverConfig myConfig;

#endif  // SRC_CONFIG_HPP_

// EOF
