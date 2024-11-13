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

#include <victron.hpp>
#include <testdata.hpp>

test(shunt_test1) {
    VictronTestData testData = { "Smart Shunt", 0xA339, BatteryMonitor, { 0xff,0xff,0xe5,0x04,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0xf4,0x01,0x40,0xdf,0x03,0x00,0x00,0x00,0x00,0x00 } };
    VictronSmartShunt v(&testData.decrypted[0], testData.model);
    uint16_t data;

    assertEqual(v.getDeviceName(), "Smart Shunt");
    assertEqual(v.getModelNo(), 0xA339);

    assertEqual(v.getAlarm(), 0);
    assertEqual(v.getAuxMode(), 3);
    assertEqual(v.getRemaningMins(), 0);

    data = v.getSoc() * 10;
    assertEqual(data, 892); // TODO: Not sure this is the correct value. Example states 500...

    data = v.getBatteryCurrent() * 1000;
    assertEqual(data, 0);

    data = v.getBatteryVoltage() * 100;
    assertEqual(data, 1253);

    data = v.getAux() * 100;
    assertEqual(data, 0);

    data = -v.getConsumedAh() * 10;
    assertEqual(data, 500);
}

// EOF
