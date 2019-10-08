/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/grants_properties_json_state.h"
#include "bat/ledger/internal/migration/state/json/grant_response_json_state.h"

namespace brave_rewards {

const char kGrantsKey[] = "grants";

GrantsPropertiesJsonState::GrantsPropertiesJsonState() = default;

GrantsPropertiesJsonState::~GrantsPropertiesJsonState() = default;

ledger::GrantsPropertiesPtr GrantsPropertiesJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* grants = dictionary.FindListKey(kGrantsKey);
  if (!grants) {
    return nullptr;
  }

  auto state = ledger::GrantsProperties::New();

  for (const auto& grant_dictionary : grants) {
    GrantResponseJsonState grant_response_state();
    auto grant_response = grant_response_state.FromJson(grant_dictionary);

    state->grants.push_back(grant_response);
  }

  return state;
}

}  // namespace brave_rewards
