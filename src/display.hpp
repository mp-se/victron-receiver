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
#ifndef SRC_DISPLAY_HPP_
#define SRC_DISPLAY_HPP_

#include <config.hpp>
#include <main.hpp>

#include "TFT_eSPI.h"

enum FontSize { FONT_9 = 9, FONT_12 = 12, FONT_18 = 18, FONT_24 = 24 };

class Display {
 private:
  TFT_eSPI* _tft = NULL;
  FontSize _fontSize = FontSize::FONT_9;

 public:
  Display();
  void setup();

  void clear();
  void setFont(FontSize f);
  void printLine(int l, const String& text);
  void printLineCentered(int l, const String& text);
};

extern Display myDisplay;

#endif  // SRC_DISPLAY_HPP_

// EOF
