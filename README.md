![download](https://img.shields.io/github/downloads/mp-se/victron-receiver/total) 
![release](https://img.shields.io/github/v/release/mp-se/victron-receiver?label=latest%20release)
![issues](https://img.shields.io/github/issues/mp-se/victron-receiver)
![pr](https://img.shields.io/github/issues-pr/mp-se/victron-receiver)

# Overview

This is a project for reading Victron Instant Readouts over Bluetooth and pushing this into Home Assistant using an ESP32 board, WiFi and MQTT.

# Features

* Collect data from Victron Devices that supports the Instant Readout option. 
* Identify victron devices and add the mac adress to the configuration automatically.
* Web interface for easy configuration.
* MQTT integration with Home Assistant for all identified values. 
* Firmware update and Serial Logging via web interface. 

# Victron Devices Supported

Currently there is support for the following Victron Devices mainly since these are the onces I'm using. But there is a feature to capture data from any Victron Device that support the Instant Readout and then I can add support for those.

* Battery Monitor: Smart Battery Monitor + SmartShunt
* DC-DC Charger: Orion Smart DC-DC Charger
* AC Charger: Blue Smart IP65 Charger
* Solar Charger: BlueSolar MPPT

The following types are **NOT** yet supported due to lack of test data.

* Inverter
* SmartLithium
* InverterRS
* GxDevice
* SmartBatteryProtect
* LynxSmartBMS
* MultiRS
* VeBus
* DcEnergyMeter

Support for other devices can be done on request with a sample of the data (can be recorded in the software). You need to add the encryption key so the device is registered, then it will show up as an unknown device in the web interface. Open an issue in github and provide the captured data as well as what the actual readings is in the app so I can validate the result.

# Hardware

Currently I'm using the following development board for running my instance but on request I can add support for more. A display is optional so basically you only need an ESP32s3 or ESP32c3 board with BLE5 support. 

* Waveshare ESP32 S3 with TFT (https://www.waveshare.com/product/mcu-tools/development-boards/esp32/esp32-s3-touch-lcd-2.8.htm)

Other options are:
* Lolin ESP32 PRO with Lolin TFT
* Lolin ESP32 S3 PRO with Lolin TFT

# Flashing

Currently I use VSCode and PlatformIO to build and flash the device. Pre-built binaries are available and can be flashed using esptool. On request more options can be added. 

An option is to use python and esptool for flashing. Run the commands from the root directory for this project.

- Install python3 from python.org
- run> pip install esptool
- run> python3 flash.py target port

Example: python3 flash.py esp32s3w COM10

# Software Setup

All the configuration is done using a web interface running on the device but after flashing there is a need to setup the wifi support. After installation the device will create an SSID called Victron with the password 'instant1'. Join this network and then navigate to http://192.168.4.1 to open up the user interface. Under WIFI you can scan for existing networks, select the one you want and enter the SSID. Once the wifi is settings is saved you can reset the device and it should connect to the network. If you have an display the IP adress is shown on the bottom of the screen.

Next you need to add your victron devices under the Device -> Security section. The mac adress and encryption key can be found under the settings for the device. Press the 3 dots in the upper corner and select Product Info. If the device can be found and data decrypted it will show up in the user interface on the home screen. 

Finally you need to enter the adress and credentials for your Home Assistant MQTT server.

# Home Assistant

The data posted to MQTT will depend on the victron device and each sensor value will be posted on one topic where the structure is as follows:

```
victron_instant/[device_mac_adress]/[attribute] = [value]
```

For a Smart Battery Monitor this is the data being posted:

```
victron_instant/[device_mac_adress]/name = [The name you entered in the user interface]
victron_instant/[device_mac_adress]/model = [The Victron model ID]
victron_instant/[device_mac_adress]/battery_voltage = [Value from device]
victron_instant/[device_mac_adress]/temperature = [Temperature in C]
```

# Thanks

Thanks to the following projects for providing examples and data for making the implementation much easier. 

* https://github.com/hoberman/Victron_BLE_Advertising_example
* https://github.com/keshavdv/victron-ble

