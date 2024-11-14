/*
MIT License

Copyright (c) 2024 Magnus

Based on code/ideas from these projects:
https://github.com/hoberman/Victron_BLE_Advertising_example
https://github.com/keshavdv/victron-ble

Victron docs on the manufacturer data in advertisement packets can be found at:
https://community.victronenergy.com/storage/attachments/48745-extra-manufacturer-data-2022-12-14.pdf

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

#include <aes/esp_aes.h>

#include <blescanner.hpp>
#include <config.hpp>
#include <string>
#include <utils.hpp>

BleScanner bleScanner;

void BleDeviceCallbacks::onResult(NimBLEAdvertisedDevice* advertisedDevice) {
  // See if we have manufacturer data and then look to see if it's coming from a
  // Victron device.
  if (advertisedDevice->haveManufacturerData() == true) {
    std::string manufacturer = advertisedDevice->getManufacturerData();

    // Now let's setup a pointer to a struct to get to the data more cleanly.
    const VictronManufacturerData* vicData =
        reinterpret_cast<const VictronManufacturerData*>(manufacturer.c_str());

    // ignore this packet if the Vendor ID isn't Victron.
    if (vicData->vendorID != 0x02e1) {
      return;
    }

    VictronConfig cfg = myConfig.findVictronConfig(
        advertisedDevice->getAddress().toString().c_str());

    // Convert the string to an array that we can use to decrypt the data
    uint8_t key[16];
    if (cfg.key.length() == 32) {
      for (int i = 0, j = 0; i < 32; i = i + 2, j++) {
        char s[3];
        char* p;

        s[0] = cfg.key.charAt(i);
        s[1] = cfg.key.charAt(i + 1);
        s[2] = 0;

        key[j] = strtol(&s[0], &p, 16);
      }
    } else {
      Log.warning(F("BLE : Could not find key for device %s" CR),
                  advertisedDevice->getAddress().toString().c_str());

      if (cfg.mac.length() == 0) {
        for (int i = 0; i < MAX_VICTRON_DEVICES; i++) {
          cfg = myConfig.getVictronConfig(i);

          if (cfg.mac.length() == 0) {
            cfg.mac = advertisedDevice->getAddress().toString().c_str();
            cfg.name = "Found_device";
            cfg.key = "";
            myConfig.setVictronConfig(cfg, i);
            Log.info(
                F("BLE : Device is from Vitron but not registered, adding MAC "
                  "to configuration." CR));
            myConfig.saveFile();
            return;
          }
        }
      }

      return;
    }

    // Check if the first byte in the encryption key matches the received data,
    // simple validation to check if we have the right key
    if (vicData->encryptKeyMatch != key[0]) {
      Log.warning(F("BLE : The stored encryption key does not match the device "
                    "%x vs %x" CR),
                  key[0], vicData->encryptKeyMatch);
      return;
    }

    constexpr auto keyBits = 128;  // Number of bits for AES-CTR decrypt.
    uint8_t decrypted[16];
    esp_aes_context ctx;

    esp_aes_init(&ctx);
    int status = esp_aes_setkey(&ctx, key, keyBits);

    if (status != 0) {
      Log.warning(F("BLE : Failed to set the aes key, error %d" CR), status);
      esp_aes_free(&ctx);
      return;
    }

    // Construct the 16-byte nonce counter array by piecing it together
    // byte-by-byte.
    uint8_t dataCounterLSB = (vicData->nonceDataCounter) & 0xff;
    uint8_t dataCounterMSB = ((vicData->nonceDataCounter) >> 8) & 0xff;
    u_int8_t nonceCounter[16] = {dataCounterLSB, dataCounterMSB, 0};
    u_int8_t stream_block[16] = {0};
    size_t nonce_offset = 0;

    // The number of encrypted bytes is given by the number of bytes in the
    // manufacture data as a whole minus the number of bytes (10) in the header
    // part of the data.
    status = esp_aes_crypt_ctr(
        &ctx, manufacturer.length() - 10, &nonce_offset, nonceCounter,
        stream_block, &vicData->victronEncryptedData[0], &decrypted[0]);

    if (status != 0) {
      Log.warning(F("BLE : Failed to do decryption, error %d" CR), status);
      esp_aes_free(&ctx);
      return;
    }

    esp_aes_free(&ctx);

    DynamicJsonDocument doc(2000);
    JsonObject obj = doc.createNestedObject();

    switch (vicData->victronRecordType) {
      case VictronDeviceType::BatteryMonitor: {
        if (vicData->model == 0xA3A4 || vicData->model == 0xA3A5) {
          VictronBatteryMonitor vbm(&decrypted[0], vicData->model);
          vbm.toJson(obj);
        } else {
          VictronShunt vbm(&decrypted[0], vicData->model);
          vbm.toJson(obj);
        }
      } break;

      case VictronDeviceType::DcDcConverter: {
        VictronDcDcCharger vdc(&decrypted[0], vicData->model);
        vdc.toJson(obj);
      } break;

      case VictronDeviceType::AcCharger: {
        VictronAcCharger vbm(&decrypted[0], vicData->model);
        vbm.toJson(obj);
      } break;

      case VictronDeviceType::SolarCharger: {
        VictronSolarCharger vbm(&decrypted[0], vicData->model);
        vbm.toJson(obj);
      } break;

      default: {
        Log.notice(
            F("VIC : Unknown device found. Creating dump of data for analysis "
              "and implementation." CR));
        VictronUnknown vu(&decrypted[0], vicData->model, vicData->vendorID,
                          vicData->beaconType, vicData->victronRecordType);
        vu.toJson(obj);
      } break;
    }

    int i = bleScanner.findVictronBleData(cfg.mac);
    if (i >= 0) {
      VictronBleData& vbd = bleScanner.getVictronBleData(i);
      vbd.setMacAdress(cfg.mac);
      vbd.setName(cfg.name);
      String j;
      serializeJson(obj, j);
      vbd.setJson(j);
      vbd.setUpdated();
    }
  }
}

BleScanner::BleScanner() { _deviceCallbacks = new BleDeviceCallbacks(); }

void BleScanner::init() {
  NimBLEDevice::init("");
  _bleScan = NimBLEDevice::getScan();
  _bleScan->setAdvertisedDeviceCallbacks(_deviceCallbacks);
  _bleScan->setMaxResults(0);
  _bleScan->setActiveScan(_activeScan);

  _bleScan->setInterval(
      97);  // Select prime numbers to reduce risk of frequency beat pattern
            // with ibeacon advertisement interval
  _bleScan->setWindow(37);  // Set to less or equal setInterval value. Leave
                            // reasonable gap to allow WiFi some time.
  scan();
}

void BleScanner::deInit() {
  waitForScan();
  NimBLEDevice::deinit();
}

bool BleScanner::scan() {
  if (!_bleScan) return false;

  if (_bleScan->isScanning()) return true;

  _bleScan->clearResults();

  for (int i = 0; i < MAX_VICTRON_DEVICES; i++) {
    _victron[i].clearUpdate();
  }

  Log.notice(F("BLE : Starting %s scan." CR),
             _activeScan ? "ACTIVE" : "PASSIVE");
  _bleScan->setActiveScan(_activeScan);

  if (_bleScan->start(_scanTime, nullptr, true)) {
    return true;
  }

  Log.error(F("BLE : Scan failed to start." CR));
  return false;
}

bool BleScanner::waitForScan() {
  if (!_bleScan) return false;

  while (_bleScan->isScanning()) {
    delay(100);
  }

  return true;
}

// EOF
