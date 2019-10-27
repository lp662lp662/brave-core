/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/json_helper.h"

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"

namespace ledger {

base::DictionaryValue* JsonHelper::FromJson(
    const std::string& json) {
  auto value = base::JSONReader::Read(json);

  base::DictionaryValue* dictionary = nullptr;
  value->GetAsDictionary(&dictionary);
  return dictionary;
}

}  // namespace ledger
