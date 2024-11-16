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

#include <victron_solar.hpp>
#include <testdata.hpp>

test(solar_test1) {
    VictronTestData testData = {"Solar Charger", 0xA042, SolarCharger, {0x04, 0x00, 0x6c, 0x05, 0x0e, 0x00,
                                          0x03, 0x00, 0x13, 0x00, 0x00, 0xfe,
                                          0x40, 0x9a, 0xc0, 0x69, 0x00, 0x00,
                                          0x00, 0x00, 0x00}};
    VictronSolarCharger v(&testData.decrypted[0], testData.model);
    uint16_t data;

    assertEqual(v.getDeviceName(), "Solar Charger");

    assertEqual(v.getModelNo(), 0xA042);

    assertEqual(v.getError(), 0);
    assertEqual(v.getState(), 4);

    data = v.getBatteryVoltage() * 100;
    assertEqual(data, 1388);

    data = v.getBatteryCurrent() * 100;
    assertEqual(data, 140);

    data = v.getYieldToday() * 100;
    assertEqual(data, 3);

    data = v.getLoadCurrent() * 100;
    assertEqual(data, 0);

    data = v.getPvPower() * 100;
    assertEqual(data, 1900);

/*

    data = v.getOffReasons();
    assertEqual(data, 128);


    assertEqual(isnan(v.getOutputVoltage()), true);*/
}

// EOF
