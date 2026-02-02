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
#include <blescanner.hpp>
#include <config.hpp>
#include <cstdio>
#include <display.hpp>
#include <helper.hpp>
#include <led.hpp>
#include <log.hpp>
#include <looptimer.hpp>
#include <main.hpp>
#include <pushtarget.hpp>
#include <resources.hpp>
#if defined(ESPFWK_PSYCHIC_HTTP)
#include <serialws2.hpp>
#else
#include <serialws.hpp>
#endif
#include <exide_client.hpp>
#include <uptime.hpp>
#include <utils.hpp>
#include <webserver.hpp>
#include <wificonnection.hpp>

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

SerialDebug mySerial;
VictronReceiverConfig myConfig(CFG_APPNAME, CFG_FILENAME);
WifiConnection myWifi(&myConfig, CFG_AP_SSID, CFG_AP_PASS, CFG_APPNAME,
                      USER_SSID, USER_PASS);
VictronReceiverWebServer myWebServer(&myConfig);
SerialWebSocket mySerialWebSocket;
Display myDisplay;
ExideClient myExideClient;

// Define constats for this program
uint32_t loopMillis = 0;
RunMode runMode = RunMode::receiverMode;

struct LogEntry {
  char s[60] = "";
};

#define MAX_LOG_ENTRIES 9
LogEntry logEntryList[MAX_LOG_ENTRIES];
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
      myWifi.enableImprov(true);
      myWifi.startAP();
      break;

    case RunMode::receiverMode:
      Log.notice(F("Main: Connecting to WIFI." CR));
      myDisplay.printLineCentered(3, "Connecting to WIFI");
      myWifi.connect();
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
          myWebServer.setupWebServer(runMode == RunMode::wifiSetupMode,
                                     &mySerialWebSocket, &Serial);
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
    myBleScanner.setScanTime(myConfig.getBleScanTime());
    myBleScanner.setAllowActiveScan(myConfig.getBleActiveScan());
    myBleScanner.init();
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
LoopTimer pushLoop(30000);

void loop() {
  myUptime.calculate();

  myWebServer.loop();
  myWifi.loop();
  myExideClient.loop();

  switch (runMode) {
    case RunMode::receiverMode:
      myBleScanner.scan();
      if (!myWifi.isConnected()) {
        Log.warning(F("Loop: Wifi was disconnected, trying to reconnect." CR));
        myWifi.connect();
      }

      if (pushLoop.hasExpired()) {
        pushLoop.reset();
        controller();
      }
      break;

    case RunMode::wifiSetupMode:
      break;
  }

  if (logUpdated) {
    renderDisplayLogs();
    logUpdated = false;
  }

  if (mainLoop.hasExpired()) {
    mainLoop.reset();
    renderDisplayFooter();
  }
}

void addLogEntry(tm timeinfo, String name) {
  snprintf(&logEntryList[logIndex].s[0], sizeof(LogEntry::s),
           "%02d:%02d:%02d %s", timeinfo.tm_hour, timeinfo.tm_min,
           timeinfo.tm_sec, name.c_str());

  if (++logIndex >= MAX_LOG_ENTRIES) logIndex = 0;
  logUpdated = true;
}

void controller() {
  VictronReceiverPush push(&myConfig);

  // Process gravitymon from BLE
  for (int i = 0; i < MAX_VICTRON_DEVICES; i++) {
    BleData &vbd = myBleScanner.getBleData(i);

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

  // Send some data connected to the device itself
  JsonDocument doc2;

  doc2[PARAM_APP_VER] = CFG_APPVER;
  doc2[PARAM_APP_BUILD] = CFG_GITREV;
  doc2[PARAM_PUSH_RESEND_TIME] = myConfig.getPushResendTime();

  JsonObject obj2 = doc2.as<JsonObject>();
  push.sendAll("victron_receiver", myConfig.getMDNS(), obj2);
}

void renderDisplayHeader() {
  myDisplay.printLineCentered(0, "Victron BLE Receiver");
}

void renderDisplayFooter() {
  char info[80];

  switch (runMode) {
    case RunMode::receiverMode:
      snprintf(&info[0], sizeof(info), "%s, rssi %d%s",
               WiFi.localIP().toString().c_str(), WiFi.RSSI(),
               myWebServer.isSslEnabled() ? ", SSL" : "");
      break;

    case RunMode::wifiSetupMode:
      snprintf(&info[0], sizeof(info), "Wifi Setup - 192.168.4.1");
      break;
  }

  myDisplay.printLineCentered(10, &info[0]);
}

void renderDisplayLogs() {
  for (int i = 0, j = logIndex; i < MAX_LOG_ENTRIES; i++) {
    j--;
    if (j < 0) j = MAX_LOG_ENTRIES - 1;
    myDisplay.printLine(i + 1, &logEntryList[j].s[0]);
  }
}

// EOF
