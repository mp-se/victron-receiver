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

#include <AUnit.h>
#include <Arduino.h>
#include <bitreader.hpp>

test(bitereader_test1) {
  uint8_t data[] = { 0xff, 0x00, 0x7f, 0xff, 0x80, 0x7f };

  BitReader br(&data[0], sizeof(data));

  uint32_t u1, u2;
  int32_t s1, s2;

  u1 = br.readUnsigned(8);
  u2 = 255;
  assertEqual(u1, u2);

  u1 = br.readUnsigned(4);
  u2 = 0x0;
  assertEqual(u1, u2);

  u1 = br.readUnsigned(4);
  u2 = 0x0;
  assertEqual(u1, u2);

  s1 = br.readSigned(8);
  s2 = 127;
  assertEqual(u1, u2);

  s1 = br.readSigned(8);
  s2 = -127;
  assertEqual(u1, u2);

  s1 = br.readSigned(16);
  s2 = -127;
  assertEqual(u1, u2);
}

// EOF
