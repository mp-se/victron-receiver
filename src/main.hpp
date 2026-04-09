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

#ifndef SRC_MAIN_HPP_
#define SRC_MAIN_HPP_

#include <stdlib.h>

#include <espframework.hpp>

enum RunMode {
  receiverMode = 0,
  wifiSetupMode = 1,
};
extern RunMode runMode;

#define ENABLE_BLE

constexpr auto DECIMALS_TEMP = 2;
constexpr auto DECIMALS_VOLTAGE = 2;
constexpr auto DECIMALS_CURRENT = 2;
constexpr auto DECIMALS_POWER = 2;

#endif  // SRC_MAIN_HPP_
