/*
MIT License

Copyright (c) 2026 Magnus

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

#include <Arduino.h>

#include <blescanner.hpp>
#include <cstdio>
#include <exide_client.hpp>
#include <log.hpp>
#include <string>
#include <vector>

ExideClient::~ExideClient() {
  // Ensure all clients are cleaned up
  for (auto& kv : devices) {
    if (kv.second.pClient) {
      if (kv.second.pNotifyChar) kv.second.pNotifyChar->unsubscribe();
      if (kv.second.pClient->isConnected()) kv.second.pClient->disconnect();
      NimBLEDevice::deleteClient(kv.second.pClient);
    }
  }
}

void ExideClient::processAdvertisedDevice(
    const NimBLEAdvertisedDevice* advertisedDevice) {
  if (!advertisedDevice) return;

  // Only consider devices advertising service or accept all by design decision
  if (!advertisedDevice->isAdvertisingService(NimBLEUUID(EXIDE_SERVICE_UUID)))
    return;

  NimBLEAddress addr = advertisedDevice->getAddress();
  auto it = devices.find(addr);
  if (it == devices.end()) {
    ExideDevice d = {};
    d.address = addr;
    d.lastRssi = advertisedDevice->getRSSI();
    d.lastSeen = millis();
    devices[addr] = d;
    Log.notice(F("EXID: Stored new Exide device: %s RSSI=%d" CR),
               addr.toString().c_str(), advertisedDevice->getRSSI());
  } else {
    it->second.lastSeen = millis();
    it->second.lastRssi = advertisedDevice->getRSSI();
  }
}

bool ExideClient::isEligible(const ExideDevice& dev) const {
  if (dev.lastRssi < _rssiThreshold) return false;
  if (dev.backoffUntil && millis() < dev.backoffUntil) return false;
  return true;
}

void ExideClient::loop() {
  // Process devices sequentially: find the most recently seen idle device
  ExideDevice* candidate = nullptr;
  uint32_t newest = 0;
  uint32_t now = millis();
  for (auto& kv : devices) {
    ExideDevice& d = kv.second;
    // Only consider for connection if enough time has passed since last poll
    if (d.state == State::Idle && isEligible(d) && d.lastSeen > newest) {
      if (now - d.lastPolledMs >= _minPollInterval) {
        newest = d.lastSeen;
        candidate = &d;
      } else {
        // Log.notice(F("EXID: Skipping %s, polled too recently (%lu ms ago)"
        // CR),
        //             d.address.toString().c_str(), now - d.lastPolledMs);
      }
    }
    // If a device is Active check if we should disconnect after active duration
    if (d.state == State::Active) {
      if (millis() - d.activeSince > _activeDurationMs) {
        // Log.verbose(F("EXID: Active duration elapsed for %s,
        // disconnecting\n"),
        //             d.address.toString().c_str());
        disconnectDevice(d, "active timeout");
      } else {
        // Periodically resend data request (alternating between 0x80 and 0x90)
        // if we don't have a plausible reading yet
        if (millis() - d.lastCmdSentMs > 3000) {
          // Always resend command if needed; event system will handle validity
          // Log.verbose(
          //     F("EXID: Resending CMD 0x%02X to %s to request live data" CR),
          //     d.nextCmd, d.address.toString().c_str());
          sendCommand(d, d.nextCmd);
          d.lastCmdSentMs = millis();
          d.nextCmd = (d.nextCmd == 0x80) ? 0x90 : 0x80;
        }
      }
    }
  }

  if (candidate) {
    // Log.notice(F("EXID: Attempting connection to %s" CR),
    //             candidate->address.toString().c_str());
    connectToDevice(*candidate);
  }
}

void ExideClient::connectToDevice(ExideDevice& dev) {
  // On-demand client creation
  if (dev.pClient) {
    // Log.verbose(F("EXID: Client already exists for device %s — disconnecting
    // "
    //               "and recreating" CR),
    //             dev.address.toString().c_str());
    disconnectDevice(dev, "recreate");
  }

  NimBLEClient* pClient = NimBLEDevice::createClient();
  dev.pClient = pClient;

  // Mark poll time
  dev.lastPolledMs = millis();

  // Try connecting
  Log.notice(F("EXID: Connecting to %s..." CR), dev.address.toString().c_str());
  dev.state = State::Connecting;
  uint32_t start = millis();
  bool ok = pClient->connect(dev.address);
  if (!ok) {
    Log.warning(F("EXID: Connection failed"));
    scheduleBackoff(dev, _initialBackoffMs);
    NimBLEDevice::deleteClient(pClient);
    dev.pClient = nullptr;
    return;
  }

  // Log.verbose(F("EXID: Connected, discovering service..." CR));
  dev.state = State::Discovering;

  NimBLERemoteService* pService =
      pClient->getService(NimBLEUUID(EXIDE_SERVICE_UUID));
  if (!pService) {
    Log.warning(F("EXID: Expected service not found"));
    disconnectDevice(dev, "service missing");
    return;
  }

  dev.pWriteChar = pService->getCharacteristic(NimBLEUUID(EXIDE_WRITE_UUID));
  dev.pNotifyChar = pService->getCharacteristic(NimBLEUUID(EXIDE_NOTIFY_UUID));
  if (!dev.pWriteChar || !dev.pNotifyChar) {
    Log.warning(F("EXID: Required characteristics not found"));
    disconnectDevice(dev, "chars missing");
    return;
  }

  // Debug: print write characteristic UUID and properties
  // if (dev.pWriteChar) {
  //   Log.verbose(F("EXID: WriteChar UUID=%s" CR),
  //               dev.pWriteChar->getUUID().toString().c_str());
  // }
  // if (dev.pNotifyChar) {
  //   Log.verbose(F("EXID: NotifyChar UUID=%s" CR),
  //               dev.pNotifyChar->getUUID().toString().c_str());
  // }

  // Subscribe to notifications with a lambda that forwards to onNotify
  dev.pNotifyChar->subscribe(
      true, [&dev, this](NimBLERemoteCharacteristic* pChar, uint8_t* pData,
                         size_t length, bool isNotify) {
        this->onNotify(dev, pChar, pData, length, isNotify);
      });

  dev.state = State::Active;
  dev.activeSince = millis();
  dev.retryCount = 0;
  // Log.verbose(F("EXID: Device %s is now active" CR),
  //             dev.address.toString().c_str());

  // Send one-shot realtime (0x80) and battery state (0x90) requests to prompt
  // live notifications
  if (dev.pWriteChar && !dev.sentInitialCommand) {
    // Log.verbose(F("EXID: Sending initial CMDs: 0x80 and 0x90"));
    sendCommand(dev, 0x80);
    delay(80);  // brief pause to avoid coalescing
    sendCommand(dev, 0x90);
    dev.sentInitialCommand = true;
    dev.lastCmdSentMs = millis();
    dev.nextCmd = 0x80;
  }
}

void ExideClient::disconnectDevice(ExideDevice& dev, const char* reason) {
  Log.notice(F("EXID: Disconnecting %s: %s" CR), dev.address.toString().c_str(),
             reason);
  if (dev.pNotifyChar) {
    dev.pNotifyChar->unsubscribe();
    dev.pNotifyChar = nullptr;
  }
  if (dev.pClient) {
    if (dev.pClient->isConnected()) dev.pClient->disconnect();
    NimBLEDevice::deleteClient(dev.pClient);
    dev.pClient = nullptr;
  }

  dev.state = State::Backoff;
  uint32_t backoffMs = _initialBackoffMs * (1 << dev.retryCount);
  if (backoffMs > 120000) backoffMs = 120000;
  dev.retryCount++;
  scheduleBackoff(dev, backoffMs);
}

void ExideClient::scheduleBackoff(ExideDevice& dev, uint32_t backoffMs) {
  dev.backoffUntil = millis() + backoffMs;
  // Log.verbose(F("EXID: Backoff for %s: %u ms" CR),
  //             dev.address.toString().c_str(), (unsigned)backoffMs);
  dev.state =
      State::Idle;  // set to idle but backoffUntil prevents immediate retry
}

static inline int asciiHexValue(uint8_t c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
  if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
  return -1;
}

static inline int Asciitochar(uint8_t a, uint8_t b) {
  int hi = asciiHexValue(a);
  int lo = asciiHexValue(b);
  if (hi < 0 || lo < 0) return 0;
  return (hi << 4) | lo;
}

void ExideClient::onNotify(ExideDevice& dev, NimBLERemoteCharacteristic* pChar,
                           uint8_t* data, size_t length, bool isNotify) {
  if (length == 0) return;
  // Log.verbose(F("EXID: Notified (%u bytes) from %s" CR), (unsigned)length,
  //             dev.address.toString().c_str());

  // Dump raw for debugging
  // Log.verbose(F("EXID: RAW: "));
  // for (size_t i = 0; i < length; ++i) {
  //   EspSerial.printf("%02X ", data[i]);
  // }
  // EspSerial.println();

  for (size_t i = 0; i < length; i++) {
    uint8_t b = data[i];

    // Marker FE starts a new frame
    if (b == 0xFE || b == '^') {
      dev.asciiBuffer = "";       // Reset
      dev.protocolRecvState = 2;  // Collecting
      continue;
    }

    if (dev.protocolRecvState == 2) {
      int hv = asciiHexValue(b);
      if (hv >= 0) {
        dev.asciiBuffer += static_cast<char>(b);
      } else {
        // Terminator (0x00, 0x0E, 0x0D, etc)
        if (dev.asciiBuffer.length() >= 32) {
          processAsciiFrame(dev, dev.asciiBuffer);
        }
        dev.asciiBuffer = "";
        dev.protocolRecvState = 0;
      }
    }

    // Also handle binary protocol 0xAA 0x55
    // (Briefly, if we're not in an ASCII frame)
    if (dev.protocolRecvState != 2) {
      handleBinaryProtocol(dev, data, length);
      break;  // only process once
    }
  }
}

void ExideClient::processAsciiFrame(ExideDevice& dev, std::string& frame) {
  size_t len = frame.length();
  if (len < 32) return;

  Log.notice(F("EXID: Processing ASCII Frame (%d chars): %s" CR), (unsigned)len,
             frame.c_str());

  const unsigned char* bytes =
      reinterpret_cast<const unsigned char*>(frame.data());
  auto A = [&](int pairIdx) -> uint32_t {
    if (pairIdx * 2 + 1 >= static_cast<int>(len)) return 0;
    return static_cast<uint32_t>(
        Asciitochar(bytes[pairIdx * 2], bytes[pairIdx * 2 + 1]));
  };

  ExideDataParser::ExideInfo parsed = {0};
  parsed.valid = true;

  // Status block (16 pairs)
  uint32_t v1 = A(0) | (A(1) << 8) | (A(2) << 16) | (A(3) << 24);
  int32_t v2 =
      static_cast<int32_t>(A(4) | (A(5) << 8) | (A(6) << 16) | (A(7) << 24));
  uint32_t v3 = A(8) | (A(9) << 8) | (A(10) << 16) | (A(11) << 24);

  parsed.capacity = v3;
  uint16_t v4 = static_cast<uint16_t>(A(12) | (A(13) << 8));
  uint16_t v5 = static_cast<uint16_t>(A(14) | (A(15) << 8));

  parsed.voltage = static_cast<float>(v1) * 0.001f;
  parsed.current = static_cast<float>(v2) * 0.001f;
  parsed.cycleCount = v4;
  parsed.soc = static_cast<uint8_t>(v5 & 0xFF);

  // Bytes 16-17 might be temperature in Kelvin or Celsius * 10
  uint16_t v6 = static_cast<uint16_t>(A(16) | (A(17) << 8));
  if (v6 > 2000) {  // Likely Kelvin (2731 = 0C)
    parsed.temperature = (static_cast<float>(v6) - 2731.5f) * 0.1f;
  } else {
    parsed.temperature = static_cast<float>(v6) * 0.1f;
  }

  // Cell voltages (starting at pair 22 if present)
  if (len >= 60) {
    for (int c = 0; c < 16; ++c) {
      int startPair = 22 + c * 2;
      if (startPair + 1 < static_cast<int>(len / 2)) {
        uint16_t cv =
            static_cast<uint16_t>(A(startPair) | (A(startPair + 1) << 8));
        if (cv > 2000 && cv < 4500) {
          parsed.cellVoltages[c] = cv;
          parsed.cellCount = c + 1;
        }
      }
    }
  }

  if (parsed.voltage >= 5.0f && parsed.voltage <= 60.0f) {
    Log.notice(F("EXID: Voltage=%F V, Current=%F A, Capacity=%F Ah, SOC=%d %%, "
                 "Cycles=%d, "
                 "Temp=%F C" CR),
               parsed.voltage, parsed.current,
               static_cast<float>(parsed.capacity) / 1000, parsed.soc,
               parsed.cycleCount, parsed.temperature);
    if (parsed.cellCount > 0) {
      String cells;
      for (int i = 0; i < parsed.cellCount; ++i) {
        char buf[32];
        snprintf(buf, sizeof(buf), "[%d:%F V] ", i + 1,
                 parsed.cellVoltages[i] * 0.001f);
        cells += buf;
      }

      Log.notice(F("EXID: Cells: %s" CR), cells.c_str());
      storeData(dev, parsed);
    }
  } else {
    Log.warning(F("EXID: Frame implausible: %F V" CR), parsed.voltage);
  }
}

void ExideClient::storeData(ExideDevice& dev,
                            const ExideDataParser::ExideInfo& parsed) {
  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  String mac = dev.address.toString().c_str();
  String EXIDE_BATTERY_NAME =
      "Exide Battery " + mac.substring(mac.length() - 3);

  doc["name"] = EXIDE_BATTERY_NAME;
  doc["battery_voltage"] = serialized(String(parsed.voltage, 2));
  doc["battery_current"] = serialized(String(parsed.current, 2));
  doc["battery_capacity"] =
      serialized(String(static_cast<float>(parsed.capacity) / 1000, 2));
  doc["soc"] = parsed.soc;
  doc["temperature"] = serialized(String(parsed.temperature, 2));
  doc["cycles"] = parsed.cycleCount;

  // Add cellVoltages array
  if (parsed.cellCount > 0) {
    JsonArray cellVoltagesArray = doc["cellVoltages"].to<JsonArray>();
    for (int i = 0; i < parsed.cellCount; ++i) {
      cellVoltagesArray.add(serialized(String(parsed.cellVoltages[i] * 0.001f, 3)));
    }
  }

  int i = myBleScanner.findBleData(mac);
  if (i >= 0) {
    BleData& ebd = myBleScanner.getBleData(i);
    ebd.setMacAdress(mac);
    ebd.setName(EXIDE_BATTERY_NAME);
    String j;
    j.reserve(1000);
    serializeJson(doc, j);
    ebd.setJson(j);
    ebd.setUpdated();
  }
}

// If not ASCII hex, try binary commands (legacy binary protocol)
void ExideClient::handleBinaryProtocol(ExideDevice& dev, uint8_t* data,
                                       size_t length) {
  if (length >= 9 && data[0] == 0xAA && data[1] == 0x55) {
    size_t payloadLen = (length > 9) ? (length - 9) : 0;
    Log.notice(F("EXID: BINARY_PAYLOAD_LEN=%d " CR), (unsigned)payloadLen);
    // for (size_t i = 0; i < payloadLen; ++i) {
    //   Log.verbose(F("EXID: %02X " CR), data[8 + i]);
    // }
    // Log.verbose(F("EXID: " CR));
    uint8_t recvChecksum = data[length - 1];
    uint32_t sum = 0;
    for (size_t i = 0; i < length - 1; ++i) sum += data[i];
    uint8_t calc = static_cast<uint8_t>(sum ^ 0x33);
    // Log.verbose(F("EXID: CHECKSUM: recv=%02X calc=%02X" CR), recvChecksum,
    //             calc);
    uint8_t cmd = data[7];
    ExideDataParser::ExideInfo parsed = {0};

    switch (cmd) {
      case 0x13:
        parsed = ExideDataParser::parseVersionData(data, length);
        break;
      case 0x80:
        parsed = ExideDataParser::parseRealTimeData(data, length);
        break;
      case 0x90:
        parsed = ExideDataParser::parseBatteryState(data, length);
        break;
        // default:
        //   Log.notice(F("EXID: Unknown binary command %02X" CR), cmd);
        //   break;
    }

    if (parsed.valid) {
      Log.notice(F("EXID: Voltage=%F V, Current=%F A, Capacity=%F mAh, SOC=%d "
                   "%%, Cycles=%d, "
                   "Temp=%F C" CR),
                 parsed.voltage, parsed.current,
                 static_cast<float>(parsed.capacity) / 1000, parsed.soc,
                 parsed.cycleCount, parsed.temperature);
      if (parsed.cellCount > 0) {
        String cells;
        for (int i = 0; i < parsed.cellCount; ++i) {
          char buf[32];
          snprintf(buf, sizeof(buf), "[%d:%F V] ", i + 1,
                   parsed.cellVoltages[i] * 0.001f);
          cells += buf;
        }
        Log.notice(F("EXID: Cells: %s" CR), cells.c_str());
        storeData(dev, parsed);
      }
    } else {
      Log.warning(F("EXID: Binary parsed packet not marked valid" CR));
    }
  }
}

void ExideClient::sendCommand(ExideDevice& dev, uint8_t cmd, uint8_t* payload,
                              size_t payloadLen) {
  if (!dev.pWriteChar || !dev.pClient || !dev.pClient->isConnected()) {
    // Log.warning(
    //     F("ECID: Cannot send command — no write characteristic or not "
    //       "connected" CR));
    return;
  }

  size_t packetLen = 9 + payloadLen;
  std::vector<uint8_t> packet(static_cast<size_t>(packetLen));

  packet[0] = 0xAA;
  packet[1] = 0x55;
  packet[2] = static_cast<uint8_t>((packetLen >> 8) & 0xFF);
  packet[3] = static_cast<uint8_t>(packetLen & 0xFF);

  packet[4] = static_cast<uint8_t>((dev.serialNum >> 8) & 0xFF);
  packet[5] = static_cast<uint8_t>(dev.serialNum & 0xFF);
  dev.serialNum++;

  packet[6] = 0x80;  // flags (as per legacy client)
  packet[7] = cmd;

  if (payload && payloadLen > 0) memcpy(packet.data() + 8, payload, payloadLen);

  // checksum = (sum over bytes) ^ 0x33
  uint16_t s = 0;
  for (size_t i = 0; i < packetLen - 1; ++i) s += packet[i];
  packet[packetLen - 1] = static_cast<uint8_t>(s ^ 0x33);

  // Log.verbose(F("EXID: Sending single command 0x%02X to %s" CR), cmd,
  //             dev.address.toString().c_str());
  bool ok = false;
  // Try write with response first
  ok = dev.pWriteChar->writeValue(packet.data(), packetLen, true);
  if (!ok) {
    // Log.warning(
    //     F("EXID: Write with response failed, trying write without "
    //       "response..." CR));
    ok = dev.pWriteChar->writeValue(packet.data(), packetLen, false);
  }

  if (ok) {
    // Log.verbose(F("EXID: Command sent successfully" CR));
    dev.lastCmdSentMs = millis();
  } else {
    Log.warning(
        F("EXID: Failed to send command (both with/without response)" CR));
  }
}

// EOF
