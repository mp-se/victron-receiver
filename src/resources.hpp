/*
GNU GENERAL PUBLIC LICENSE
Version 3, 29 June 2007

Copyright (C) 2024-2026 Magnus
https://github.com/mp-se/victron-receiver

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
