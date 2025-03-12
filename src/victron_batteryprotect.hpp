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

#ifndef SRC_VICTRON_BATTERYPROTECT_HPP_
#define SRC_VICTRON_BATTERYPROTECT_HPP_

#include <bitreader.hpp>
#include <log.hpp>
#include <main.hpp>
#include <victron_common.hpp>

class VictronBatteryProtect : public VictronDevice {
 private:
    uint8_t _state;
    uint8_t _error;
    float _batteryVoltage;
    bool _switchState;

 public:
    VictronBatteryProtect(const uint8_t* data, uint16_t model) {
        setBaseData("Smart BatteryProtect", model, data);
        
        BitReader br(data, 21);
        
        // First byte is state
        _state = br.readUnsigned(8);
        
        // Second byte is error
        _error = br.readUnsigned(8);
        
        // Skip next 5 bytes (reserved/unknown)
        br.readUnsigned(40);
        
        // Battery voltage
        uint32_t voltage = br.readUnsigned(16);
        _batteryVoltage = voltage / 100.0f;
        
        // Skip next 10 bytes
        br.readUnsigned(80);
        
        // Switch state might be in the last byte
        uint32_t status = br.readUnsigned(8);
        _switchState = (status & 0x01) != 0;
    }

    uint8_t getState() { return _state; }
    uint8_t getError() { return _error; }
    float getBatteryVoltage() { return _batteryVoltage; }
    bool getSwitchState() { return _switchState; }

    String getStateString() {
        switch(_state) {
            case 0: return "Off";
            case 1: return "Low voltage";
            case 2: return "High voltage";
            case 3: return "High temperature";
            case 4: return "On";
            default: return "Unknown";
        }
    }

    String getErrorString() {
        switch(_error) {
            case 0: return "No error";
            case 1: return "Battery voltage too low";
            case 2: return "Battery voltage too high";
            case 3: return "Temperature too high";
            default: return "Unknown error";
        }
    }

    void toJson(JsonObject& doc) override {
        VictronDevice::toJson(doc);
        doc["state"] = _state;
        doc["state_message"] = getStateString();
        doc["error"] = _error;
        doc["error_message"] = getErrorString();
        doc["battery_voltage"] = _batteryVoltage;
        doc["switch_state"] = _switchState;
    }
};

#endif  // SRC_VICTRON_BATTERYPROTECT_HPP_ 