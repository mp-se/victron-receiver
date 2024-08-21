/*
MIT License

Copyright (c) 2024 Magnus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the = "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED = "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#ifndef SRC_RESOURCES_HPP_
#define SRC_RESOURCES_HPP_

// Common strings used in json formats.
constexpr auto PARAM_APP_VER = "app_ver";
constexpr auto PARAM_APP_BUILD = "app_build";
constexpr auto PARAM_PLATFORM = "platform";
constexpr auto PARAM_FORMAT_POST = "http_post_format";
constexpr auto PARAM_FORMAT_POST2 = "http_post2_format";
constexpr auto PARAM_FORMAT_GET = "http_get_format";
constexpr auto PARAM_FORMAT_INFLUXDB = "influxdb2_format";
constexpr auto PARAM_FORMAT_MQTT = "mqtt_format";
constexpr auto PARAM_PUSH_FORMAT = "push_format";
constexpr auto PARAM_PUSH_RETURN_CODE = "push_return_code";
constexpr auto PARAM_PUSH_ENABLED = "push_enabled";
constexpr auto PARAM_TOTAL_HEAP = "total_heap";
constexpr auto PARAM_FREE_HEAP = "free_heap";
constexpr auto PARAM_IP = "ip";
constexpr auto PARAM_WIFI_SETUP = "wifi_setup";
constexpr auto PARAM_BLE_ACTIVE_SCAN = "ble_active_scan";
constexpr auto PARAM_BLE_SCAN_TIME = "ble_scan_time";
constexpr auto PARAM_PUSH_RESEND_TIME = "push_resend_time";
constexpr auto PARAM_TIMEZONE = "timezone";
constexpr auto PARAM_TEMP = "temp";
constexpr auto PARAM_UPDATE_TIME = "update_time";
constexpr auto PARAM_PUSH_TIME = "push_time";
constexpr auto PARAM_UPTIME_SECONDS = "uptime_seconds";
constexpr auto PARAM_UPTIME_MINUTES = "uptime_minutes";
constexpr auto PARAM_UPTIME_HOURS = "uptime_hours";
constexpr auto PARAM_UPTIME_DAYS = "uptime_days";

constexpr auto PARAM_VICTRON_CONFIG = "victron_config";
constexpr auto PARAM_MAC = "mac";
constexpr auto PARAM_KEY = "key";
constexpr auto PARAM_NAME = "name";
constexpr auto PARAM_DATA = "data";
constexpr auto PARAM_VICTRON_DEVICE = "victron_device";

#endif  // SRC_RESOURCES_HPP_
