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

#ifndef SRC_PUSHTARGET_HPP_
#define SRC_PUSHTARGET_HPP_

#include <basepush.hpp>
#include <templating.hpp>

class VictronReceiverPush : public BasePush {
 private:
  VictronReceiverConfig* _victronReceiverConfig;
  String _baseTemplate;

 public:
  explicit VictronReceiverPush(VictronReceiverConfig* victronReceiverConfig);

  void sendAll(String name, String mac, JsonObject& doc);

  void clearTemplate() { _baseTemplate.clear(); }
  int getLastCode() { return _lastResponseCode; }
  bool getLastSuccess() { return _lastSuccess; }
};

#endif  // SRC_PUSHTARGET_HPP_

// EOF
