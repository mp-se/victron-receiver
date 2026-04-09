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

#include <Wire.h>

#include <display.hpp>
#include <fonts.hpp>
#include <log.hpp>

Display::Display() {
#if defined(ENABLE_TFT)
  _tft = new TFT_eSPI();
#endif
}

void Display::setup() {
#if defined(ENABLE_TFT)
  if (!_tft) return;

  _tft->init();
  _tft->setSwapBytes(true);
  _tft->setRotation(
      1);  // TODO(mpse): Allow rotation to be set in configuration
  clear();
  setFont(FontSize::FONT_9);
#endif
}

void Display::setFont(FontSize f) {
#if defined(ENABLE_TFT)
  if (!_tft) return;

  switch (f) {
    default:
    case FontSize::FONT_9:
      _tft->setFreeFont(FF17);
      break;
    case FontSize::FONT_12:
      _tft->setFreeFont(FF18);
      break;
    case FontSize::FONT_18:
      _tft->setFreeFont(FF19);
      break;
    case FontSize::FONT_24:
      _tft->setFreeFont(FF20);
      break;
  }
#endif
}

void Display::printLine(int l, const String& text) {
#if defined(ENABLE_TFT)
  if (!_tft) return;

  uint16_t h = _tft->fontHeight();
  _tft->fillRect(0, l * h, _tft->width(), h, TFT_BLACK);
  _tft->drawString(text.c_str(), 0, l * h, GFXFF);
#endif
}

void Display::printLineCentered(int l, const String& text) {
#if defined(ENABLE_TFT)
  if (!_tft) return;

  uint16_t h = _tft->fontHeight();
  uint16_t w = _tft->textWidth(text);
  _tft->fillRect(0, l * h, _tft->width(), h, TFT_BLACK);
  _tft->drawString(text.c_str(), (_tft->width() - w) / 2, l * h, GFXFF);
#endif
}

void Display::clear() {
#if defined(ENABLE_TFT)
  if (!_tft) return;

  _tft->fillScreen(TFT_BLACK);
  delay(1);
#endif
}

// EOF
