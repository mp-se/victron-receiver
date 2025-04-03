![download](https://img.shields.io/github/downloads/mp-se/victron-receiver/total) 
![release](https://img.shields.io/github/v/release/mp-se/victron-receiver?label=latest%20release)
![issues](https://img.shields.io/github/issues/mp-se/victron-receiver)
![pr](https://img.shields.io/github/issues-pr/mp-se/victron-receiver)

# Overview

This is a project for reading Victron Instant Readouts over Bluetooth and pushing this into Home Assistant using an ESP32 board, WiFi and MQTT / Home Assistant REST API.

# Changes

* 0.5.0 - Added support for Battery Protect and Update UI dependecies

* 0.4.0 - Revert to Arduino 2.x for stability reasons. Changed packet parsing to simplify decoding and updated dependecies.

* 0.3.0 - Upgrade to latest libraries and Arduino 3.0.7, Added configuration backup feature.

* 0.2.5 - Version that correcly decode the first few devices (AC Charger, DCDC Charger, Battery Monitor, Smart Shunt & Solar Charger) with Home Assistant API integration (MQTT + REST API). 

# Features

* Collect data from Victron Devices that supports the Instant Readout option. 
* Identify victron devices and add the mac adress to the configuration automatically.
* Web interface for easy configuration.
* MQTT integration with Home Assistant for all identified values. 
* REST API integration with Home Assistant for all identified values. 
* Firmware update and Serial Logging via web interface. 

# Support

If you want to support my work you can do that through these options

[<img src="https://gravitymon.com/images/buymecoffee.png" height=40>](https://www.buymeacoffee.com/mpse/) [<img src="https://img.shields.io/static/v1?label=Sponsor&message=%E2%9D%A4&logo=GitHub&color=%23fe8e86" height=40>](https://github.com/sponsors/mp-se)

# Victron Devices Supported

Currently there is support for the following Victron Devices mainly since these are the onces I'm using. But there is a feature to capture data from any Victron Device that support the Instant Readout and then I can add support for those.

* Battery Monitor: Smart Battery Monitor 
    - Tested and working
* Battery Monitor: SmartShunt
    - Need help to validate the following values; State Of Charge, Battery Current, Consumed Ah
* DC-DC Charger: Orion Smart DC-DC Charger
    - Need help to validate data during charge cycle.
* AC Charger: Blue Smart IP65 Charger
    - Charger with one channel has been tested and validated.
* Solar Charger: BlueSolar MPPT
    - Need help to validate data during charge cycle.
* Smart Battery Protect
    - Tested and working

The following types can be supported if I can receive some test data.

* Inverter
* SmartLithium
* InverterRS
* GxDevice
* LynxSmartBMS
* MultiRS
* VeBus
* DcEnergyMeter

To get test data, register the device and add the encrytption key so data can be extrated. Then use the copy button in the we UI to copy data to the clipboard. When copying make a note of the values shown in the Victron App so that I can compare those with my code. Open an issue in github and provide the captured data as well as what the actual readings is in the app so I can validate the result.

# Hardware

Currently I'm using the following development board for running my instance but on request I can add support for more. A display is optional so basically you only need an ESP32s3 or ESP32c3 board with BLE support. 

* Waveshare ESP32 S3 with TFT (firmware32s3w_tft.bin) (https://www.waveshare.com/product/mcu-tools/development-boards/esp32/esp32-s3-touch-lcd-2.8.htm)

Other options are:
* Lolin ESP32 PRO with Lolin TFT (firmware32_tft.bin)
* Lolin ESP32 S3 PRO with Lolin TFT (firmware32s3_tft.bin)
* Lolin EP32 C3 without TFT (firmware32c3.bin)
* Lolin EP32 S3 without TFT (firmware32s3.bin)

# Flashing

Flashing can be done using the web-flasher, platformio or python (commandline). Easy option is to use the web-flasher here https://mp-se.github.io/victron-receiver/flash/index.html. This method will do a full erase before installation and this is a good option for first use. 

When the installation is completed you will be prompted for adding wifi credentials via the web interface. This is only active during the first 30 seconds 
after a boot and no wifi settings is defined. This will be shown on the TFT display. You can also set credentials using the AP approach described in 
the next chapter. 

## Flashing via commandline

Another option is to use python and esptool for flashing. Run the commands from the root directory for this project.

- Install python3 from python.org
- run> pip install esptool
- run> python3 flash.py target port

Example: python3 flash.py esp32s3w COM10

# Network / Software Setup

All the configuration is done using a web interface running on the device but after flashing there is a need to setup the wifi support. After installation the device will create an SSID called Victron with the password 'instant1'. Join this network and then navigate to http://192.168.4.1 to open up the user interface. Under WIFI you can scan for existing networks, select the one you want and enter the SSID. Once the wifi is settings is saved you can reset the device and it should connect to the network. If you have an display the IP adress is shown on the bottom of the screen.

Next you need to add your victron devices under the Device -> Security section. The mac adress and encryption key can be found under the settings for the device. Press the 3 dots in the upper corner and select Product Info. If the device can be found and data decrypted it will show up in the user interface on the home screen. 

Finally you need to enter the adress and credentials for your Home Assistant MQTT server or the Home Assistant REST API. 

# Home Assistant (MQTT)

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

When using each victron device will be an Entity in Home Assistant with all values connected to that entity.

# Home Assistant (REST API)

The data posted to MQTT will contain more information and will link status to the entity (device). This option is not avilable when using the REST API. 

Here you will get separate sensors but they will be named using the device name you set in the configuration.

If you have the sensors at a remote location this method will work with Nabu Casa public API's. This method will be slower than using MQTT, so MQTT is the prefered method if you have everything on the same network.

The naming of the sensors using the REST API is as follows. Units will be added for the Temperature, Voltage, Current and Power.

```
[device name from config]_[attribute from victron device]
```

When using the REST API each attribute will be a standalone sensor value and not grouped into entities.

# Thanks

Thanks to the following projects for providing examples and data for making the implementation much easier. 

* https://github.com/hoberman/Victron_BLE_Advertising_example
* https://github.com/keshavdv/victron-ble

* thomasjobs for testing and providing valueable test data.
