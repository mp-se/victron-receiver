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
#include <ImprovWiFi.h>

#include <blescanner.hpp>
#include <config.hpp>
#include <display.hpp>
#include <helper.hpp>
#include <led.hpp>
#include <log.hpp>
#include <looptimer.hpp>
#include <main.hpp>
#include <pushtarget.hpp>
#include <serialws.hpp>
#include <uptime.hpp>
#include <utils.hpp>
#include <webserver.hpp>
#include <wificonnection.hpp>

constexpr auto CFG_APPNAME = "victron";
constexpr auto CFG_FILENAME = "/victron.json";
constexpr auto CFG_AP_SSID = "Victron";
constexpr auto CFG_AP_PASS = "instant1";

#if !defined(USER_SSID)
#define USER_SSID ""
#define USER_PASS ""
#endif

void controller();
void renderDisplayHeader();
void renderDisplayFooter();
void renderDisplayLogs();
void checkForImprovWifi();

SerialDebug mySerial(115200L, false);
VictronReceiverConfig myConfig(CFG_APPNAME, CFG_FILENAME);
WifiConnection myWifi(&myConfig, CFG_AP_SSID, CFG_AP_PASS, CFG_APPNAME,
                      USER_SSID, USER_PASS);
VictronReceiverWebServer myWebServer(&myConfig);
SerialWebSocket mySerialWebSocket;
Display myDisplay;

// Define constats for this program
uint32_t loopMillis = 0;
RunMode runMode = RunMode::receiverMode;

struct LogEntry {
  char s[60] = "";
};

const auto maxLogEntries = 9;
LogEntry logEntryList[maxLogEntries];
int logIndex = 0;
bool logUpdated = true;

void setup() {
  // delay(4000);
  Log.notice(F("Main: Started setup for %s." CR), myConfig.getID());
  printBuildOptions();
  detectChipRevision();

  Log.notice(F("Main: Initialize display." CR));
  myDisplay.setup();
  myDisplay.setFont(FontSize::FONT_12);
  myDisplay.printLineCentered(1, "Victron BLE Receiver");
  myDisplay.printLineCentered(3, "Starting");

  myConfig.checkFileSystem();
  myWifi.init();
  myConfig.setWifiScanAP(true);
  checkResetReason();
  myConfig.loadFile();

  // No stored config, check if we can get config via improve (serial)
  if (!myWifi.hasConfig()) {
    Log.notice(F("Main: Waiting for remote WIFI setup." CR));
    myDisplay.printLineCentered(3, "Waiting for remote WIFI setup");
    checkForImprovWifi();  // Will return once timeout occurs or configuration
                           // is completed
    myDisplay.printLineCentered(4, "");
    myDisplay.printLineCentered(5, "");
  }

  // No stored config, move to portal
  if (!myWifi.hasConfig()) {
    Log.notice(
        F("Main: No wifi configuration detected, entering wifi setup." CR));
    runMode = RunMode::wifiSetupMode;
  }

  // Double reset, go to portal.
  if (myWifi.isDoubleResetDetected()) {
    Log.notice(F("Main: Double reset detected, entering wifi setup." CR));
    runMode = RunMode::wifiSetupMode;
  }

  // Do this setup for all modes exect wifi setup
  switch (runMode) {
    case RunMode::wifiSetupMode:
      Log.notice(F("Main: Entering WIFI Setup." CR));
      myDisplay.printLineCentered(3, "Entering WIFI Setup");
      myWifi.startAP();
      break;

    case RunMode::receiverMode:
      Log.notice(F("Main: Connecting to WIFI." CR));
      myDisplay.printLineCentered(3, "Connecting to WIFI");

      if (strlen(myConfig.getWifiDirectSSID())) {
        myDisplay.printLineCentered(4, "Creating AP");
        Log.notice(F("Main: Connecting to WIFI and creating AP." CR));
        myWifi.connect(false, WIFI_AP_STA);
        myWifi.setAP(myConfig.getWifiDirectSSID(),
                     myConfig.getWifiDirectPass());
        myWifi.startAP(WIFI_AP_STA);
      } else {
        myWifi.connect(false, WIFI_AP);
      }
      break;
  }

  // Do this setup for configuration mode
  switch (runMode) {
    case RunMode::receiverMode:
      if (myWifi.isConnected()) {
        Log.notice(F("Main: Activating web server." CR));
        ledOn(LedColor::BLUE);  // Blue or slow flashing to indicate config mode
        Log.notice(F("Main: Synchronizing time." CR));
        myDisplay.printLineCentered(3, "Synchronizing time");
        myDisplay.printLineCentered(4, "");
        myWifi.timeSync(myConfig.getTimezone());

        case RunMode::wifiSetupMode:
          Log.notice(F("Main: Initializing the web server." CR));
          myWebServer.setupWebServer();  // Takes less than 4ms, so skip
                                         // this measurement
          mySerialWebSocket.begin(myWebServer.getWebServer(), &Serial);
          mySerial.begin(&mySerialWebSocket);
      } else {
        Log.error(F("Main: Failed to connect with WIFI." CR));
        ledOn(LedColor::RED);  // Red or fast flashing to indicate connection
                               // error
        myDisplay.printLineCentered(5, "Failed to connect with WIFI");
      }
      break;

    default:
      break;
  }

  if (runMode == RunMode::receiverMode) {
    Log.notice(F("Main: Initialize ble scanner." CR));
    bleScanner.setScanTime(myConfig.getBleScanTime());
    bleScanner.setAllowActiveScan(myConfig.getBleActiveScan());
    bleScanner.init();
  }

  Log.notice(F("Main: Startup completed." CR));
  myDisplay.printLineCentered(3, "Startup completed");
  myDisplay.setFont(FontSize::FONT_9);
  delay(1000);
  myDisplay.clear();
  renderDisplayHeader();
  renderDisplayFooter();
  loopMillis = millis();
}

LoopTimer mainLoop(2000);

void loop() {
  myUptime.calculate();
  controller();

  myWebServer.loop();
  myWifi.loop();

  switch (runMode) {
    case RunMode::receiverMode:
      if (!myWifi.isConnected()) {
        Log.warning(F("Loop: Wifi was disconnected, trying to reconnect." CR));
        myWifi.connect();
      }
      controller();
      break;

    case RunMode::wifiSetupMode:
      break;
  }

  if (logUpdated) {
    renderDisplayLogs();
    logUpdated = false;
  }

  if (mainLoop.hasExipred()) {
    mainLoop.reset();
    renderDisplayFooter();
  }
}

void addLogEntry(tm timeinfo, String name) {
  snprintf(&logEntryList[logIndex].s[0], sizeof(LogEntry::s),
           "%02d:%02d:%02d %s", timeinfo.tm_hour, timeinfo.tm_min,
           timeinfo.tm_sec, name.c_str());

  if (++logIndex >= maxLogEntries) logIndex = 0;
  logUpdated = true;
}

void controller() {
  bleScanner.scan();
  // bleScanner.waitForScan();

  VictronReceiverPush push(&myConfig);

  // Process gravitymon from BLE
  for (int i = 0; i < MAX_VICTRON_DEVICES; i++) {
    VictronBleData &vbd = bleScanner.getVictronBleData(i);

    if (vbd.getUpdated() && (vbd.getPushAge() > myConfig.getPushResendTime())) {
      addLogEntry(vbd.getTimeUpdated(), vbd.getName());

      JsonDocument doc;
      DeserializationError err = deserializeJson(doc, vbd.getJson());

      if (!err) {
        Log.notice(F("Main: Name=%s" CR), vbd.getName());

        JsonObject obj = doc.as<JsonObject>();
        push.sendAll(vbd.getName(), vbd.getMacAdress(), obj);
        vbd.setPushed();
      }
    }
  }
}

void renderDisplayHeader() {
  myDisplay.printLineCentered(0, "Victron BLE Receiver");
}

void renderDisplayFooter() {
  char info[80];

  switch (runMode) {
    case RunMode::receiverMode:
      if (strlen(myConfig.getWifiDirectSSID())) {
        snprintf(&info[0], sizeof(info), "%s - %s",
                 WiFi.localIP().toString().c_str(),
                 myConfig.getWifiDirectSSID());
      } else {
        snprintf(&info[0], sizeof(info), "%s, rssi %d",
                 WiFi.localIP().toString().c_str(), WiFi.RSSI());
      }
      break;

    case RunMode::wifiSetupMode:
      snprintf(&info[0], sizeof(info), "Wifi Setup - 192.168.4.1");
      break;
  }

  myDisplay.printLineCentered(10, &info[0]);
}

void renderDisplayLogs() {
  for (int i = 0, j = logIndex; i < maxLogEntries; i++) {
    j--;
    if (j < 0) j = maxLogEntries - 1;
    myDisplay.printLine(i + 1, &logEntryList[j].s[0]);
  }
}

// Code for handling wifi setup over serial connection with esp-web-tools
constexpr auto IMPROVE_TIMEOUT_SECONDS = 30;

LoopTimer improveTimeout(IMPROVE_TIMEOUT_SECONDS * 1000);

void improveSetWifiCredentials(const char *ssid, const char *password) {
  myConfig.setWifiSSID(ssid, 0);
  myConfig.setWifiPass(password, 0);
  myConfig.saveFile();
}

void improveInfo(const char *info) { myDisplay.printLineCentered(4, info); }

void improveDebug(const char *debug) { myDisplay.printLineCentered(5, debug); }

void checkForImprovWifi() {
  improveTimeout.reset();

#if defined(ESP32S3)
  ImprovWiFi improvWiFi(CFG_APPNAME, CFG_APPVER, "ESP32S3", myConfig.getMDNS());
#elif defined(ESP32C3)
  ImprovWiFi improvWiFi(CFG_APPNAME, CFG_APPVER, "ESP32C3", myConfig.getMDNS());
#elif defined(ESP32)
  ImprovWiFi improvWiFi(CFG_APPNAME, CFG_APPVER, "ESP32", myConfig.getMDNS());
#else
#error "Undefined target"
#endif

  improvWiFi.setWiFiCallback(improveSetWifiCredentials);
  improvWiFi.setInfoCallback(improveInfo);
  // improvWiFi.setDebugCallback(improveDebug);

  LoopTimer update(500);

  // Run for 10 seconds
  while (!improveTimeout.hasExipred() || improvWiFi.isConfigInitiated()) {
    if (improvWiFi.isConfigCompleted()) return;

    if (update.hasExipred()) {
      update.reset();

      char buf[80] = "";

      if (!improvWiFi.isConfigInitiated())
        int time = IMPROVE_TIMEOUT_SECONDS - (improveTimeout.getTimePassed() / 1000);
        Log.notice(F("Main: Waiting for remote WIFI setup, waiting for %d." CR), time);
        snprintf(
            buf, sizeof(buf), "Waiting for %d s", time);

      myDisplay.printLineCentered(6, buf);
    }

    improvWiFi.loop();
    delay(1);
  }
}

// EOF
