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
#include <Wire.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

#include <blescanner.hpp>
#include <config.hpp>
#include <helper.hpp>
#include <main.hpp>
#include <pushtarget.hpp>
#include <resources.hpp>
#include <templating.hpp>
#include <testdata.hpp>
#include <uptime.hpp>
#include <webserver.hpp>

VictronReceiverWebServer::VictronReceiverWebServer(WebConfig *config)
    : BaseWebServer(config) {}

void VictronReceiverWebServer::webHandleConfigRead(
    AsyncWebServerRequest *request) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/config(read)." CR));
  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_L);
  JsonObject obj = response->getRoot().as<JsonObject>();
  myConfig.createJson(obj);
  response->setLength();
  request->send(response);
}

void VictronReceiverWebServer::webHandleConfigWrite(
    AsyncWebServerRequest *request, JsonVariant &json) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/config(write)." CR));
  JsonObject obj = json.as<JsonObject>();
  myConfig.parseJson(obj);
  obj.clear();
  myConfig.saveFile();

  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_S);
  obj = response->getRoot().as<JsonObject>();
  obj[PARAM_SUCCESS] = true;
  obj[PARAM_MESSAGE] = "Configuration updated";
  response->setLength();
  request->send(response);
}

void VictronReceiverWebServer::webHandleFactoryDefaults(
    AsyncWebServerRequest *request) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/factory." CR));
  myConfig.saveFileWifiOnly();
  LittleFS.remove(ERR_FILENAME);
  LittleFS.end();
  Log.notice(F("WEB : Deleted files in filesystem, rebooting." CR));

  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_S);
  JsonObject obj = response->getRoot().as<JsonObject>();
  obj[PARAM_SUCCESS] = true;
  obj[PARAM_MESSAGE] = "Factory reset completed, rebooting";
  response->setLength();
  request->send(response);
  delay(500);
  ESP_RESET();
}

void VictronReceiverWebServer::webHandleStatus(AsyncWebServerRequest *request) {
  Log.notice(F("WEB : webServer callback for /api/status(get)." CR));

  // Fallback since sometimes the loop() does not always run after firmware
  // update...
  if (_rebootTask) {
    Log.notice(F("WEB : Rebooting using fallback..." CR));
    delay(500);
    ESP_RESET();
  }

  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_L);
  JsonObject obj = response->getRoot().as<JsonObject>();

  obj[PARAM_ID] = myConfig.getID();
  obj[PARAM_TEMP_FORMAT] = String(myConfig.getTempFormat());
  obj[PARAM_APP_VER] = CFG_APPVER;
  obj[PARAM_APP_BUILD] = CFG_GITREV;
  obj[PARAM_MDNS] = myConfig.getMDNS();
#if defined(ESP32S3)
  obj[PARAM_PLATFORM] = "esp32s3";
#elif defined(ESP32C3)
  obj[PARAM_PLATFORM] = "esp32c3";
#elif defined(ESP32)
  obj[PARAM_PLATFORM] = "esp32";
#else
#error "Unsupported platform"
#endif
  obj[PARAM_RSSI] = WiFi.RSSI();
  obj[PARAM_SSID] = WiFi.SSID();
  obj[PARAM_TOTAL_HEAP] = ESP.getHeapSize();
  obj[PARAM_FREE_HEAP] = ESP.getFreeHeap();
  obj[PARAM_IP] = WiFi.localIP().toString();
  obj[PARAM_WIFI_SETUP] = (runMode == RunMode::wifiSetupMode) ? true : false;

  obj[PARAM_UPTIME_SECONDS] = myUptime.getSeconds();
  obj[PARAM_UPTIME_MINUTES] = myUptime.getMinutes();
  obj[PARAM_UPTIME_HOURS] = myUptime.getHours();
  obj[PARAM_UPTIME_DAYS] = myUptime.getDays();

  JsonArray devices = obj.createNestedArray(PARAM_VICTRON_DEVICE);

  for (int i = 0; i < MAX_VICTRON_DEVICES; i++) {
    VictronBleData vbd = bleScanner.getVictronBleData(i);
    if (vbd.getMacAdress() != "") {
      JsonObject n = devices.createNestedObject();
      n[PARAM_NAME] = vbd.getName();
      n[PARAM_DATA] = vbd.getJson();
      n[PARAM_MAC] = vbd.getMacAdress();
      n[PARAM_UPDATE_TIME] = vbd.getUpdateAge();
      n[PARAM_PUSH_TIME] = vbd.getPushAge();
    }
  }

#if defined(ENABLE_SIMULATION)
  for (int i = 0; i < getNoTestData(); i++) {
    VictronBleSimulationData vbd = createDeviceFromTestData(i);
    JsonObject n = devices.createNestedObject();
    n[PARAM_NAME] = vbd.getName();
    n[PARAM_DATA] = vbd.getJson();
    n[PARAM_MAC] = "00:00:00:00:00";
    n[PARAM_UPDATE_TIME] = 0;
    n[PARAM_PUSH_TIME] = 0;
  }
#endif

  response->setLength();
  request->send(response);
}

bool VictronReceiverWebServer::setupWebServer() {
  Log.notice(F("WEB : Configuring web server." CR));

  BaseWebServer::setupWebServer();
  MDNS.addService("victron-receiver", "tcp", 80);

  // Static content
  Log.notice(F("WEB : Setting up handlers for gravmon gateway web server." CR));

  AsyncCallbackJsonWebHandler *handler;
  handler = new AsyncCallbackJsonWebHandler(
      "/api/config",
      std::bind(&VictronReceiverWebServer::webHandleConfigWrite, this,
                std::placeholders::_1, std::placeholders::_2),
      JSON_BUFFER_SIZE_L);
  _server->addHandler(handler);
  _server->on("/api/config", HTTP_GET,
              std::bind(&VictronReceiverWebServer::webHandleConfigRead, this,
                        std::placeholders::_1));
  _server->on("/api/factory", HTTP_GET,
              std::bind(&VictronReceiverWebServer::webHandleFactoryDefaults,
                        this, std::placeholders::_1));
  _server->on("/api/status", HTTP_GET,
              std::bind(&VictronReceiverWebServer::webHandleStatus, this,
                        std::placeholders::_1));
  _server->addHandler(handler);

  Log.notice(F("WEB : Web server started." CR));
  return true;
}

void VictronReceiverWebServer::loop() { BaseWebServer::loop(); }

// EOF
