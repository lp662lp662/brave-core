/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/json_state.h"

#include "base/json/json_reader.h"

namespace brave_rewards {

template <class T>
JsonState<T>::JsonState() = default;

template <class T>
JsonState<T>::~JsonState() = default;

template <class T>
base::DictionaryValue* JsonState<T>::GetAsDictionary(
    const std::string& json) const {
  auto value = base::JSONReader::Read(json);
  if (!value || !value->is_dict()) {
    return nullptr;
  }

  base::DictionaryValue* dictionary = nullptr;
  if (!value->GetAsDictionary(&dictionary)) {
    return nullptr;
  }

  return dictionary;
}

}  // namespace brave_rewards
