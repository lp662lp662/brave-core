/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_JSON_STATE_H_
#define BRAVELEDGER_JSON_STATE_H_

#include <string>

#include "base/values.h"

namespace ledger {

template <class T>
class JsonState {
 public:
  virtual bool FromJson(
      const std::string& json,
      T*) const = 0;

  virtual bool FromDict(
      const base::DictionaryValue* dictionary,
      T*) const = 0;
};

}  // namespace ledger

#endif  // BRAVELEDGER_JSON_STATE_H_
