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

#include <basewebserver.hpp>
#include <config.hpp>
#include <log.hpp>
#include <main.hpp>
#include <resources.hpp>

VictronReceiverConfig::VictronReceiverConfig(String baseMDNS, String fileName)
    : BaseConfig(baseMDNS, fileName) {}

void VictronReceiverConfig::createJson(JsonObject& doc) const {
  // Call base class functions
  createJsonBase(doc);
  createJsonWifi(doc);
  createJsonPush(doc);

  doc[PARAM_TIMEZONE] = getTimezone();
  doc[PARAM_BLE_ACTIVE_SCAN] = getBleActiveScan();
  doc[PARAM_BLE_SCAN_TIME] = getBleScanTime();
  doc[PARAM_PUSH_RESEND_TIME] = getPushResendTime();

  JsonArray devices = doc[PARAM_VICTRON_CONFIG].to<JsonArray>();

  for (int i = 0; i < MAX_VICTRON_DEVICES; i++) {
    VictronConfig vc = getVictronConfig(i);
    devices[i][PARAM_MAC] = vc.mac;
    devices[i][PARAM_KEY] = vc.key;
    devices[i][PARAM_NAME] = vc.name;
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
