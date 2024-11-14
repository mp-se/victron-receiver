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

test(ac_test1) {
    VictronTestData testData = { "Smart AC Charger", 0xA339, AcCharger, { 0x06,0x00,0x2C,0x05,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x86,0xCA,0x00,0x00,0x00 } };
    VictronSmartAcCharger v(&testData.decrypted[0], testData.model);
    uint16_t data;

    assertEqual(v.getDeviceName(), "Smart AC Charger");
    assertEqual(v.getModelNo(), 0xA339);
    assertEqual(v.getError(), 0);
    assertEqual(v.getState(), 6);
    data = v.getVoltage1() * 100;
    assertEqual(data, 1324);
    data = v.getCurrent1() * 100;
    assertEqual(data, 0);
    data = v.getVoltage2() * 100;
    assertEqual(data, 0);
    data = v.getCurrent2() * 100;
    assertEqual(data, 0);
    data = v.getVoltage3() * 100;
    assertEqual(data, 0);
    data = v.getCurrent3() * 100;
    assertEqual(data, 0);
    data = v.getTemperature() * 100;
    assertEqual(data, 0);
    data = v.getCurrentAC() * 100;
    assertEqual(data, 0);
}

test(ac_test2) {
    VictronTestData testData = { "Smart AC Charger", 0xA339, AcCharger, { 0x04,0x00,0xA0,0x25,0x04,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x7C,0x00,0x00,0x00,0x00 } };
    VictronSmartAcCharger v(&testData.decrypted[0], testData.model);
    uint16_t data;

    assertEqual(v.getDeviceName(), "Smart AC Charger");
    assertEqual(v.getModelNo(), 0xA339);
    assertEqual(v.getError(), 0);
    assertEqual(v.getState(), 4);
    data = v.getVoltage1() * 100;
    assertEqual(data, 1440);
    data = v.getCurrent1() * 100;
    assertEqual(data, 330);
    data = v.getVoltage2() * 100;
    assertEqual(data, 0);
    data = v.getCurrent2() * 100;
    assertEqual(data, 0);
    data = v.getVoltage3() * 100;
    assertEqual(data, 0);
    data = v.getCurrent3() * 100;
    assertEqual(data, 0);
    data = v.getTemperature() * 100;
    assertEqual(data, 0);
    data = v.getCurrentAC() * 100;
    assertEqual(data, 0);
}

// EOF
