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
#include <basewebserver.hpp>
#include <config.hpp>
#include <log.hpp>
#include <main.hpp>
#include <resources.hpp>

VictronReceiverConfig::VictronReceiverConfig(String baseMDNS, String fileName)
    : BaseConfig(baseMDNS, fileName, JSON_BUFFER_SIZE_XL) {}

void VictronReceiverConfig::createJson(JsonObject& doc) {
  // Call base class functions
  createJsonBase(doc);
  createJsonWifi(doc);
  createJsonPush(doc);

  doc[PARAM_TIMEZONE] = getTimezone();
  doc[PARAM_BLE_ACTIVE_SCAN] = getBleActiveScan();
  doc[PARAM_BLE_SCAN_TIME] = getBleScanTime();
  doc[PARAM_PUSH_RESEND_TIME] = getPushResendTime();

  JsonArray devices = doc.createNestedArray(PARAM_VICTRON_CONFIG);

  for (int i = 0; i < MAX_VICTRON_DEVICES; i++) {
    VictronConfig vc = getVictronConfig(i);
    JsonObject n = devices.createNestedObject();
    n[PARAM_MAC] = vc.mac;
    n[PARAM_KEY] = vc.key;
    n[PARAM_NAME] = vc.name;
  }
}

void VictronReceiverConfig::parseJson(JsonObject& doc) {
  // Call base class functions
  parseJsonBase(doc);
  parseJsonWifi(doc);
  parseJsonPush(doc);

  if (!doc[PARAM_TIMEZONE].isNull()) setTimezone(doc[PARAM_TIMEZONE]);
  if (!doc[PARAM_BLE_ACTIVE_SCAN].isNull())
    setBleActiveScan(doc[PARAM_BLE_ACTIVE_SCAN].as<bool>());
  if (!doc[PARAM_BLE_SCAN_TIME].isNull())
    setBleScanTime(doc[PARAM_BLE_SCAN_TIME].as<int>());
  if (!doc[PARAM_PUSH_RESEND_TIME].isNull())
    setPushResendTime(doc[PARAM_PUSH_RESEND_TIME].as<int>());

  if (!doc[PARAM_VICTRON_CONFIG].isNull()) {
    JsonArray devices = doc[PARAM_VICTRON_CONFIG].as<JsonArray>();
    int i = 0;

    for (JsonVariant v : devices) {
      VictronConfig cfg;
      cfg.mac = v[PARAM_MAC].as<String>();
      cfg.key = v[PARAM_KEY].as<String>();
      cfg.name = v[PARAM_NAME].as<String>();
      setVictronConfig(cfg, i++);
    }
  }
}

// EOF
