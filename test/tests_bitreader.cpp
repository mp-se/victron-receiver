/*
MIT License

Copyright (c) 2025 Magnus

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
