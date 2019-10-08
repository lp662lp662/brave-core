/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/wallet_properties_json_state.h"
#include "bat/ledger/internal/migration/state/json/grant_json_state.h"

namespace brave_rewards {

const char kParametersKey[] = "parameters";
const char kAdFreeKey[] = "adFree";
const char kChoicesKey[] = "choices";
const char kFeesKey[] = "fees";
const char kBatKey[] = "BAT";
const char kGrantsKey[] = "grants";
const char kProbiKey[] = "probi";
const char kAltCurrencyKey[] = "altcurrency";
const char kExpiryTimeKey[] = "expiryTime";
const char kTypeKey[] = "type";

WalletPropertiesJsonState::WalletPropertiesJsonState() = default;

WalletPropertiesJsonState::~WalletPropertiesJsonState() = default;

ledger::WalletPropertiesPtr WalletPropertiesJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* choices_dictionary =
      dictionary->FindPath(kParametersKey, kAdFreeKey, kChoicesKey);
  if (!choices_dictionary) {
    return nullptr;
  }

  auto* choices = choices_dictionary.FindListKey(kBatKey);
  if (!choices) {
    return nullptr;
  }

  auto* fees_dictionary =
      dictionary->FindPath(kParametersKey, kAdFreeKey, kFeesKey);
  if (!fees_dictionary) {
    return nullptr;
  }

  auto* fee = fees_dictionary.FindDoubleKey(kBatKey);
  if (!fee) {
    return nullptr;
  }

  auto* grants = dictionary->FindListKey(kGrantsKey);
  if (!grants) {
    return nullptr;
  }

  auto state = ledger::WalletProperties::New();

  for (const auto& choice : *choices) {
    state->parameters_choices.push_back(choice);
  }

  state->fee_amount = *fee;

  for (const auto& grant_dictionary : *grants) {
    GrantJsonState grant_state;
    auto* grant = grant_state.FromJson(grant_dictionary);

    state->grants.push_back(grant);
  }

  return state;
}

}  // namespace brave_rewards
