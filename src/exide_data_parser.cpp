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
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <exide_data_parser.hpp>
#include <log.hpp>

ExideDataParser::ExideInfo ExideDataParser::parseRealTimeData(uint8_t* data,
                                                              size_t len) {
  ExideInfo info = {0};
  info.valid = false;

  if (len < 10)
    return info;  // Minimum packet size: head(2), len(2), ser(2), flag(1),
                  // cmd(1), payload(min 1), ck(1)

  // Payload starts at offset 8
  uint8_t* pl = data + 8;
  size_t plen = (len >= 9) ? (len - 9) : 0;
  if (plen < 2) return info;

  uint8_t subCmd = pl[1];  // sub-command inside payload

  // Debug dump of payload
  // Log.verbose(F("PARSE_REALTIME: subCmd=%02X payload len=%u" CR), subCmd,
  //             (unsigned)plen);

  // Switch based on sub-command to avoid data overlap issues
  switch (subCmd) {
    case 0x02: {  // CELLVOL
      if (plen >= 18) {
        uint32_t sumMv = 0;
        for (int i = 0; i < 8; ++i) {
          sumMv += bytesToUint16(&pl[2 + i * 2]);
        }
        info.voltage = static_cast<float>(sumMv) * 0.001f;
        info.valid = true;
        // Log.verbose(F("EXID: PARSE_REALTIME [CELLVOL]: V=%F" CR),
        // info.voltage);
      }
      break;
    }
    case 0x03:
    case 0x80: {  // CELLTEM
      if (plen >= 18) {
        int tempSum = 0;
        int tempCount = 0;
        for (int i = 0; i < 8; ++i) {
          int16_t t = bytesToInt16(&pl[2 + i * 2]);
          if (t != 0) {
            // (t - 2731) / 10.0f
            tempSum += (t - 2731);
            tempCount++;
          }
        }
        if (tempCount > 0) {
          info.temperature = static_cast<float>(tempSum) / tempCount / 10.0f;
          info.valid = true;
          // Log.verbose(F("EXID: PARSE_REALTIME [CELLTEM]: Temp=%F C" CR),
          //             info.temperature);
        }
      }
      break;
    }
    case 0x05: {  // CYCLE
      if (plen >= 18) {
        info.cycleCount = bytesToUint16(&pl[14]);
        info.soc = pl[16];
        info.valid = true;
        // Log.verbose(F("EXID: PARSE_REALTIME [CYCLE]: Cycles=%d SOC=%d%%" CR),
        //             info.cycleCount, info.soc);
      }
      break;
    }
    case 0x90: {  // BATTERYSTATE as sub-command
      return parseBatteryState(data, len);
    }
      // default:
      //   Log.verbose(F("EXID: PARSE_REALTIME: Unhandled subCmd %02X" CR),
      //   subCmd); break;
  }

  return info;
}

ExideDataParser::ExideInfo ExideDataParser::parseVersionData(uint8_t* data,
                                                             size_t len) {
  ExideInfo info = {0};
  info.valid = false;

  if (len < 12) return info;

  // Parse version string from payload (offset 8)
  char versionStr[16];
  size_t copyLen =
      std::min(static_cast<size_t>(15), (len >= 9) ? (len - 9) : 0);
  memcpy(versionStr, &data[8], copyLen);
  versionStr[copyLen] = '\0';
  info.version = String(versionStr);
  info.valid = true;
  return info;
}

ExideDataParser::ExideInfo ExideDataParser::parseBatteryState(uint8_t* data,
                                                              size_t len) {
  ExideInfo info = {0};
  info.valid = false;

  if (len < 26) return info;  // expect payload >= 17 + 9

  uint8_t* pl = data + 8;
  size_t plen = (len >= 9) ? (len - 9) : 0;
  if (plen < 17) return info;

  // BatteryAttribute layout:
  // pl[2..3] chargeTime, pl[4..5] dischargeTime, pl[6..7] voltage, pl[8..11]
  // current, pl[13] bmsState1, pl[14] bmsState2, pl[15..16] customerCode

  // Debug dump payload
  // Log.notice(F("EXID: PARSE_BATTERYSTATE: payload len=%d:" CR),
  // (unsigned)plen);
  // {
  //   String hexDump;
  //   for (size_t i = 0; i < plen; ++i) {
  //     char buf[5];
  //     snprintf(buf, sizeof(buf), "%02X ", pl[i]);
  //     hexDump += buf;
  //   }
  //   Log.notice(F("%s" CR), hexDump.c_str());
  // }

  uint16_t voltageRaw = bytesToUint16(&pl[7]);
  int32_t currentRaw = bytesToInt32(&pl[9]);
  // Log.notice(
  //     F("EXID: PARSE_BATTERYSTATE: voltageRaw=%d mV, currentRaw=%d mA" CR),
  //     static_cast<unsigned>(voltageRaw), static_cast<int>(currentRaw));

  info.voltage = static_cast<float>(voltageRaw) * 0.001f;
  info.current = static_cast<float>(currentRaw) * 0.001f;

  // SOC is often in a separate CYCLE realtime packet; set to invalid here
  info.soc = 0;

  // Try parse temperature if available (pl[13..14] or elsewhere) - best-effort
  info.temperature = 0.0f;
  info.valid = true;
  return info;
}

int32_t ExideDataParser::bytesToInt32(uint8_t* bytes) {
  int32_t v = (static_cast<int32_t>(bytes[0]) << 24) |
              (static_cast<int32_t>(bytes[1]) << 16) |
              (static_cast<int32_t>(bytes[2]) << 8) |
              (static_cast<int32_t>(bytes[3]));
  return v;
}

uint16_t ExideDataParser::bytesToUint16(uint8_t* bytes) {
  return (bytes[0] << 8) | bytes[1];
}

int16_t ExideDataParser::bytesToInt16(uint8_t* bytes) {
  int16_t v =
      static_cast<int16_t>((static_cast<uint16_t>(bytes[0]) << 8) | bytes[1]);
  return v;
}

// EOF
