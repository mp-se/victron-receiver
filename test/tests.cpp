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

#include <AUnit.h>
#include <Arduino.h>
#include <config.hpp>
#include <helper.hpp>
#include <log.hpp>
#include <main.hpp>

using aunit::Printer;
using aunit::TestRunner;
using aunit::Verbosity;

SerialDebug mySerial;
VictronReceiverConfig myConfig("", "");
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
