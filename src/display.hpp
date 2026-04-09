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

#ifndef SRC_DISPLAY_HPP_
#define SRC_DISPLAY_HPP_

#include <config.hpp>
#include <main.hpp>

#if defined(ENABLE_TFT)
#include "TFT_eSPI.h"
#endif

enum FontSize { FONT_9 = 9, FONT_12 = 12, FONT_18 = 18, FONT_24 = 24 };

class Display {
 private:
#if defined(ENABLE_TFT)
  TFT_eSPI* _tft = NULL;
#endif

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
