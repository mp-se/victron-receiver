/*
MIT License

Copyright (c) 2024 Paul Andrews

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
#include "ImprovWiFi.h"

#include <WiFi.h>

#include <string>

ImprovWiFi::ImprovWiFi(const char *firmwareName, const char *version,
                       const char *chip, const char *deviceName)
    : firmwareName(firmwareName),
      version(version),
      chip(chip),
      deviceName(deviceName),
      pendingResponse(improv::Command::UNKNOWN),
      infoCallback([](const char *) {}),
      debugCallback([](const char *) {}),
      wifiCallback([this](const char *ssid, const char *password) {
        infoCallback("Bad callback");
      }) {}

void ImprovWiFi::setWiFiCallback(
    std::function<void(const char *, const char *)> callback) {
  wifiCallback = callback;
}

void ImprovWiFi::setInfoCallback(std::function<void(const char *)> callback) {
  infoCallback = callback;
}
void ImprovWiFi::setDebugCallback(std::function<void(const char *)> callback) {
  debugCallback = callback;
}

void ImprovWiFi::loop() {
  if (pendingResponse == improv::Command::WIFI_SETTINGS) {
    if (WiFi.status() == WL_CONNECTED) {
      sendProvisioned(pendingResponse);
      pendingResponse = improv::Command::UNKNOWN;
    }

    if (millis() - pendingCmdReceived > 10000) {
      infoCallback("WiFI Timed Out");
      infoConfigCompleted = true;
      pendingResponse = improv::Command::UNKNOWN;
      set_state(improv::STATE_STOPPED);
      set_error(improv::Error::ERROR_UNABLE_TO_CONNECT);
    }

    return;
  }

  if (Serial.available() > 0) {
    uint8_t b = Serial.read();
    if (x_position < sizeof(x_buffer)) {
      if (improv::parse_improv_serial_byte(
              x_position, b, x_buffer,
              [this](improv::ImprovCommand cmd) -> bool {
                return this->onCommandCallback(cmd);
              },
              [this](improv::Error error) { this->onErrorCallback(error); })) {
        x_buffer[x_position++] = b;
      } else {
        snprintf(msgBuf, sizeof(msgBuf), "Extra byte %02x", b);
        debugCallback(msgBuf);
        x_position = 0;
      }
    } else {
      debugCallback("Buffer overflow");
      x_position = 0;
    }
  }
}

bool ImprovWiFi::onCommandCallback(improv::ImprovCommand cmd) {
  char statusBuf[20];
  snprintf(statusBuf, sizeof(statusBuf), "Got command %d", cmd.command);

  debugCallback(statusBuf);
  switch (cmd.command) {
    case improv::Command::GET_CURRENT_STATE: {
      if ((WiFi.status() == WL_CONNECTED)) {
        infoCallback("Improv Connected");
        sendProvisioned(improv::Command::GET_CURRENT_STATE);
      } else {
        infoCallback("Provisioning");
        infoConfigInitiated = true;
        set_state(improv::State::STATE_AUTHORIZED);
      }

      break;
    }

    case improv::Command::WIFI_SETTINGS: {
      if (cmd.ssid.length() == 0) {
        set_error(improv::Error::ERROR_INVALID_RPC);
        break;
      }

      set_state(improv::STATE_PROVISIONING);

      pendingResponse = cmd.command;
      pendingCmdReceived = millis();
      infoCallback("Connect WiFi");
      wifiCallback(cmd.ssid.c_str(), cmd.password.c_str());
      WiFi.begin(cmd.ssid.c_str(), cmd.password.c_str());
      break;
    }

    case improv::Command::GET_DEVICE_INFO: {
      std::vector<std::string> infos = {// Firmware name
                                        firmwareName,
                                        // Firmware version
                                        version,
                                        // Hardware chip/variant
                                        chip,
                                        // Device name
                                        deviceName};
      std::vector<uint8_t> data =
          improv::build_rpc_response(improv::GET_DEVICE_INFO, infos, false);
      send_response(data);
      break;
    }

    case improv::Command::GET_WIFI_NETWORKS: {
      getAvailableWifiNetworks();
      break;
    }

    default: {
      set_error(improv::ERROR_UNKNOWN_RPC);
      return false;
    }
  }

  return true;
}

void ImprovWiFi::sendProvisioned(improv::Command responseTo) {
  set_state(improv::STATE_PROVISIONED);

  uint32_t ip = WiFi.localIP();
  char szRet[30];
  snprintf(szRet, sizeof(szRet), "http://%u.%u.%u.%u/", ip & 0xff,
           (ip >> 8) & 0xff, (ip >> 16) & 0xff, ip >> 24);

  std::vector<std::string> urls = {szRet};

  std::vector<uint8_t> data =
      improv::build_rpc_response(responseTo, urls, false);

  send_response(data);

  infoCallback("Provisioned");
  infoConfigCompleted = true;
}

void ImprovWiFi::getAvailableWifiNetworks() {
  int networkNum = WiFi.scanNetworks();

  for (int id = 0; id < networkNum; ++id) {
    std::vector<uint8_t> data = improv::build_rpc_response(
        improv::GET_WIFI_NETWORKS,
        {WiFi.SSID(id), String(WiFi.RSSI(id)),
         (WiFi.encryptionType(id) == WIFI_AUTH_OPEN ? "NO" : "YES")},
        false);
    send_response(data);
    delay(1);
  }
  // final response
  std::vector<uint8_t> data = improv::build_rpc_response(
      improv::GET_WIFI_NETWORKS, std::vector<std::string>{}, false);
  send_response(data);
}

void ImprovWiFi::onErrorCallback(improv::Error err) {
  snprintf(msgBuf, sizeof(msgBuf), "Improv Err %d", err);
  infoCallback(msgBuf);
}

void ImprovWiFi::set_state(improv::State state) {
  // Need to send line-feed as first byte so that client can flush input buffers
  // up until this packet
  uint8_t data[] = {10,
                    'I',
                    'M',
                    'P',
                    'R',
                    'O',
                    'V',
                    improv::IMPROV_SERIAL_VERSION,
                    improv::TYPE_CURRENT_STATE,
                    1,
                    state,
                    0};

  uint8_t checksum = 0x00;
  for (int i = 1; i < sizeof(data) - 1; i++)  // skip first and last bytes
    checksum += data[i];
  data[sizeof(data) - 1] = checksum;

  Serial.write(data, sizeof(data));

  debugCallback("Sent state");
}

void ImprovWiFi::send_response(std::vector<uint8_t> &response) {
  // Need to send line-feed as first byte so that client can flush input buffers
  // up until this packet
  std::vector<uint8_t> data = {10,
                               'I',
                               'M',
                               'P',
                               'R',
                               'O',
                               'V',
                               improv::IMPROV_SERIAL_VERSION,
                               improv::TYPE_RPC_RESPONSE,
                               (uint8_t)response.size()};
  data.insert(data.end(), response.begin(), response.end());

  uint8_t checksum = 0x00;
  for (int i = 1; i < data.size(); i++) checksum += data[i];
  data.push_back(checksum);

  Serial.write(data.data(), data.size());

  debugCallback("Response sent");
}

void ImprovWiFi::set_error(improv::Error error) {
  // Need to send line-feed as first byte so that client can flush input buffers
  // up until this packet
  uint8_t data[] = {10,
                    'I',
                    'M',
                    'P',
                    'R',
                    'O',
                    'V',
                    improv::IMPROV_SERIAL_VERSION,
                    improv::TYPE_ERROR_STATE,
                    1,
                    error,
                    0};

  uint8_t checksum = 0x00;
  for (int i = 1; i < sizeof(data) - 1; i++)  // skip first and last bytes
    checksum += data[i];
  data[sizeof(data) - 1] = checksum;

  Serial.write(data, sizeof(data));

  debugCallback("Error sent");
}

// EOF
