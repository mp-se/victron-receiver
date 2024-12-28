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
#ifndef SRC_IMPROVWIFI_H_
#define SRC_IMPROVWIFI_H_

#include <improv.h>

#include <vector>

class ImprovWiFi {
 public:
  ImprovWiFi(const char *firmwareName, const char *version, const char *chip,
             const char *deviceName);

  void loop();

  void setWiFiCallback(
      std::function<void(const char *, const char *)> callback);
  void setInfoCallback(std::function<void(const char *)> callback);
  void setDebugCallback(std::function<void(const char *)> callback);

  bool isConfigInitiated() { return infoConfigInitiated; }
  bool isConfigCompleted() { return infoConfigCompleted; }

 private:
  void getAvailableWifiNetworks();
  void sendProvisioned(improv::Command responseTo);

  void set_state(improv::State state);
  void send_response(std::vector<uint8_t> &response);
  void set_error(improv::Error error);

  bool onCommandCallback(improv::ImprovCommand cmd);
  void onErrorCallback(improv::Error err);

  char msgBuf[16];
  uint8_t x_buffer[50];
  uint8_t x_position = 0;
  const char *firmwareName, *version, *chip, *deviceName;

  improv::Command pendingResponse;
  int64_t pendingCmdReceived;

  bool infoConfigInitiated = false, infoConfigCompleted = false;

  std::function<void(const char *)> infoCallback;
  std::function<void(const char *)> debugCallback;
  std::function<void(const char *, const char *)> wifiCallback;
};

#endif  // SRC_IMPROVWIFI_H_
