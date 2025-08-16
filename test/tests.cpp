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
#include <AUnit.h>
#include <Arduino.h>

#include <config.hpp>
#include <display.hpp>
#include <helper.hpp>
#include <log.hpp>
#include <main.hpp>
#include <serialws2.hpp>
#include <webserver.hpp>
#include <wificonnection.hpp>

using aunit::Printer;
using aunit::TestRunner;
using aunit::Verbosity;

SerialDebug mySerial;
VictronReceiverConfig myConfig("", "");
WifiConnection myWifi(&myConfig, "", "", "", "", "");
VictronReceiverWebServer myWebServer(&myConfig);
SerialWebSocket mySerialWebSocket;
Display myDisplay;
RunMode runMode = RunMode::receiverMode;

void setup() {
  Log.notice("Victron Receiver - Unit Test Build");
  delay(4000);
  Printer::setPrinter(&EspSerial);

  // TestRunner::exclude("bitreader");
  // TestRunner::exclude("data_*");
  // TestRunner::exclude("dcdc_*");
  // TestRunner::exclude("battmon_*");
  // TestRunner::exclude("shunt_*");
  // TestRunner::exclude("inverter_*");
  // TestRunner::exclude("ac_*");
  // TestRunner::exclude("solar_*");
  // TestRunner::exclude("battprot_*");
}

void loop() {
  TestRunner::run();
  delay(10);
}

// EOF
