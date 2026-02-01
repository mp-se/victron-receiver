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
