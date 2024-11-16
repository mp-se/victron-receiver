/*
MIT License

Copyright (c) 2022-2024 Magnus

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

#include <victron_battmon.hpp>
#include <testdata.hpp>

test(battmon_test1) {
    VictronTestData testData = { "Battery Monitor", 0xA3A5, BatteryMonitor, { 0xFF,0xFF,0x45,0x04,0x00,0x00,0xFA,0x73,0xFE,0xFF,0x7F,0xFF,0xFF,0xFF,0xFF,0x00,0x24,0x68,0x00,0x00,0x00 } };
    VictronBatteryMonitor v(&testData.decrypted[0], testData.model);
    uint16_t data;

    assertEqual(v.getDeviceName(), "Battery Monitor");
    assertEqual(v.getModelNo(), 0xA3A5);
    data = v.getBatteryVoltage() * 100;
    assertEqual(data, 1093);
    data = v.getTemperatureC() * 100;
    assertEqual(data, 2374);
}

// EOF
