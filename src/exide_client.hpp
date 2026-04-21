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

#ifndef SRC_EXIDE_CLIENT_HPP_
#define SRC_EXIDE_CLIENT_HPP_

#include <NimBLEDevice.h>

#include <cstdint>
#include <exide_data_parser.hpp>
#include <map>
#include <string>
#include <vector>

class ExideClient {
 public:
  ExideClient() {}
  ~ExideClient();

  void processAdvertisedDevice(const NimBLEAdvertisedDevice* advertisedDevice);
  void loop();
  void setRssiThreshold(int rssi);

 private:
  const uint32_t _minPollInterval = 10UL * 60UL * 1000UL;  // 10 minutes

  // Device state machine for sequential processing
  enum class State {
    Idle,
    Connecting,
    Discovering,
    Subscribing,
    Active,
    Disconnecting,
    Backoff
  };

  struct ExideDevice {
    NimBLEAddress address;
    int lastRssi = -127;
    uint32_t lastSeen = 0;
    State state = State::Idle;
    uint32_t backoffUntil = 0;
    int retryCount = 0;

    // Active connection pointers (created on demand)
    NimBLEClient* pClient = nullptr;
    NimBLERemoteCharacteristic* pWriteChar = nullptr;
    NimBLERemoteCharacteristic* pNotifyChar = nullptr;

    uint32_t activeSince = 0;
    // Buffer for accumulating ASCII-encoded hex status strings from
    // notifications
    std::string asciiBuffer;
    // Receive state for simple SOI/INFO framing (1=SOI,2=INFO,3=EOI)
    uint8_t protocolRecvState = 1;  // starts as SOI
    std::vector<uint8_t> revBuf;  // raw bytes buffer used by the state machine
    int revIndex = 0;
    int end = 0;
    int mProtocolHead = 0xFE;  // observed head marker in notifications
    int mProtocolEnd = 0x00;   // default end marker

    // Serial number used for building write commands
    uint16_t serialNum = 0;
    // Whether we've sent the initial one-shot trigger command after subscribing
    bool sentInitialCommand = false;
    // Timestamp of last command sent (ms)
    uint32_t lastCmdSentMs = 0;  // used for resend/backoff of write commands
    // Which command to send next when resending (alternates 0x80/0x90)
    uint8_t nextCmd = 0x80;
    // Timestamp of last poll/connection (ms)
    uint32_t lastPolledMs = 0;
  };

  static constexpr const char* EXIDE_SERVICE_UUID =
      "0000ffe0-0000-1000-8000-00805f9b34fb";
  static constexpr const char* EXIDE_WRITE_UUID =
      "0000ffe1-0000-1000-8000-00805f9b34fb";
  static constexpr const char* EXIDE_NOTIFY_UUID =
      "0000ffe4-0000-1000-8000-00805f9b34fb";

  std::map<NimBLEAddress, ExideDevice> devices;

  // Configuration / policy
  int _rssiThreshold = -80;
  uint32_t _connectTimeoutMs = 10000;
  uint32_t _discoverTimeoutMs = 5000;
  uint32_t _activeDurationMs =
      8000;  // how long to stay connected to fetch data
  uint32_t _initialBackoffMs = 15000;
  int _maxRetries = 3;

  // Internal helpers
  bool isEligible(const ExideDevice& dev) const;
  void connectToDevice(ExideDevice& dev);
  void disconnectDevice(ExideDevice& dev, const char* reason);
  void scheduleBackoff(ExideDevice& dev, uint32_t backoffMs);
  void onNotify(ExideDevice& dev, NimBLERemoteCharacteristic* pChar,
                uint8_t* data, size_t length, bool isNotify);
  // Exide batteries might send data in either ASCII hex frames or a
  // binary protocol; handle both
  void processAsciiFrame(ExideDevice& dev, std::string& frame);
  void handleBinaryProtocol(ExideDevice& dev, uint8_t* data, size_t length);

  // Send a command (one-shot) to the device's write characteristic
  void sendCommand(ExideDevice& dev, uint8_t cmd, uint8_t* payload = nullptr,
                   size_t payloadLen = 0);
  void storeData(ExideDevice& dev, const ExideDataParser::ExideInfo& parsed);
};

extern ExideClient myExideClient;

#endif  // SRC_EXIDE_CLIENT_HPP_

// EOF
