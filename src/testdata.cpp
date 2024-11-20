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
#include <log.hpp>
#include <testdata.hpp>
#include <victron.hpp>

#if defined(ENABLE_SIMULATION)

VictronTestData testData[] = {
    // {"DC-DC Charger",
    //  0xA3C0,
    //  DcDcConverter,
    //  {0x00, 0x00, 0xF7, 0x04, 0xFF, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x37,
    //   0x40, 0x23, 0x09, 0x06, 0x00, 0x42, 0x10, 0x00, 0x00, 0x00}},
    {"DC-DC Charger",
     0xA3C0,
     DcDcConverter,
     {0x00, 0x00, 0x1D, 0x05, 0xFF, 0x7F, 0x08, 0x00, 0x00, 0x00, 0x00,
      0x0B, 0x00, 0x40, 0xFE, 0x00, 0x35, 0x91, 0x00, 0x00, 0x00}},
    // {"Battery Monitor",
    //  0xA3A5,
    //  BatteryMonitor,
    //  {0xFF, 0xFF, 0x45, 0x04, 0x00, 0x00, 0xFA, 0x73, 0xFE, 0xFF, 0x7F,
    //   0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x24, 0x68, 0x00, 0x00, 0x00}},
    {"Battery Monitor",
     0xA3A5,
     BatteryMonitor,
     {0xFF, 0xFF, 0x19, 0x05, 0x00, 0x00, 0xC0, 0x6C, 0xFE, 0xFF, 0x7F,
      0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xB7, 0xE6, 0x00, 0x00, 0x00}},
    // {"AC Charger", 0xA339, AcCharger, {0x06, 0x00, 0x2C, 0x05, 0x00, 0xFF,
    // 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x86, 0xCA,
    // 0x00, 0x00, 0x00}},
    {"AC Charger", 0xA331, AcCharger, {0x06, 0x00, 0x3D, 0xA5, 0x01, 0xFF,
                                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                       0xFF, 0x00, 0x00, 0x00, 0xA1, 0x41,
                                       0x00, 0x00, 0x00}},
    // {"Shunt", 0xA389, BatteryMonitor, {0xff, 0xff, 0xe5, 0x04, 0x00, 0x00,
    // 0x00, 0x00, 0x03, 0x00, 0x00, 0xf4, 0x01, 0x40, 0xdf, 0x03, 0x00, 0x00,
    // 0x00, 0x00, 0x00}},
    {"Shunt", 0xA389, BatteryMonitor, {0x38, 0x09, 0x1B, 0x05, 0x00, 0x00,
                                       0x01, 0x05, 0xCC, 0xCB, 0xFF, 0x08,
                                       0x00, 0x50, 0xFE, 0x00, 0x68, 0xE5,
                                       0x00, 0x00, 0x00}},
    // {"Solar Charger", 0xA042, SolarCharger, {0x04, 0x00, 0x6c, 0x05, 0x0e,
    // 0x00,
    //                                          0x03, 0x00, 0x13, 0x00, 0x00,
    //                                          0xfe,
    //                                          0x40, 0x9a, 0xc0, 0x69, 0x00,
    //                                          0x00,
    //                                          0x00, 0x00, 0x00}},
    {"Solar Charger", 0xA042, SolarCharger, {0x00, 0x00, 0x8E, 0x05, 0x00, 0x00,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0xFE,
                                             0xFF, 0x40, 0xFE, 0x00, 0x08, 0xE7,
                                             0x00, 0x00, 0x00}}};

int getNoTestData() { return sizeof(testData) / sizeof(VictronTestData); }

VictronBleSimulationData createDeviceFromTestData(int idx) {
  Log.notice(F("SIM : Parsing test data for %d." CR), idx);

  DynamicJsonDocument doc(2000);
  JsonObject obj = doc.createNestedObject();

  switch (testData[idx].recordType) {
    case VictronDeviceType::BatteryMonitor: {
      // Smart Battery is handled separately from other battery monitors
      if (testData[idx].model == 0xA3A4 || testData[idx].model == 0xA3A5) {
        VictronBatteryMonitor vbm(&testData[idx].decrypted[0],
                                  testData[idx].model);
        vbm.toJson(obj);
      } else {
        VictronShunt vbm(&testData[idx].decrypted[0], testData[idx].model);
        vbm.toJson(obj);
      }
    } break;

    case VictronDeviceType::DcDcConverter: {
      VictronDcDcCharger vdc(&testData[idx].decrypted[0], testData[idx].model);
      vdc.toJson(obj);
    } break;

    case VictronDeviceType::AcCharger: {
      VictronAcCharger vbm(&testData[idx].decrypted[0], testData[idx].model);
      vbm.toJson(obj);
    } break;

    case VictronDeviceType::SolarCharger: {
      VictronSolarCharger vbm(&testData[idx].decrypted[0], testData[idx].model);
      vbm.toJson(obj);
    } break;

    default: {
      Log.notice(
          F("VIC : Unknown device found. Creating dump of data for analysis "
            "and implementation." CR));
      VictronUnknown vu(&testData[idx].decrypted[0], testData[idx].model, 0, 0,
                        testData[idx].recordType);
      vu.toJson(obj);
    } break;
  }

  String j;
  serializeJson(obj, j);

  VictronBleSimulationData vbsd(testData[idx].name, j);
  return vbsd;
}
#else

int getNoTestData() { return 0; }

VictronBleSimulationData createDeviceFromTestData(int idx) {
  VictronBleSimulationData vbsd("", "");
  return vbsd;
}

#endif  // ENABLE_SIMULATION

// EOF
