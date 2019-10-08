/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_JSON_STATE_H_
#define BRAVELEDGER_JSON_STATE_H_

#include <string>

#include "base/values.h"

namespace brave_rewards {

template <class T>
class JsonState {
 public:
  JsonState();
  virtual ~JsonState();

  virtual T FromJson(
      const std::string& json) const = 0;

 protected:
  base::DictionaryValue* GetAsDictionary(
      const std::string& json) const;
};

}  // namespace brave_rewards

#endif  // BRAVELEDGER_JSON_STATE_H_
