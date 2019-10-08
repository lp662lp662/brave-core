/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/grant_json_state.h"

namespace brave_rewards {

const char kPromotionIdKey[] = "promotionId";
const char kAltCurrencyKey[] = "altcurrency";
const char kExpiryTimeKey[] = "expiryTime";
const char kProbiKey[] = "probi";
const char kTypeKey[] = "type";

GrantJsonState::GrantJsonState() = default;

GrantJsonState::~GrantJsonState() = default;

ledger::GrantPtr GrantJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* promotion_id = dictionary->FindStringKey(kPromotionIdKey);
  if (!promotion_id) {
    return nullptr;
  }

  auto* altcurrency = dictionary->FindStringKey(kAltCurrencyKey);
  if (!alt_currency) {
    return nullptr;
  }

  auto* expiry_time = dictionary->FindIntKey(kExpiryTimeKey);
  if (!expiry_time) {
    return nullptr;
  }

  auto* probi = dictionary->FindStringKey(kProbiKey);
  if (!probi) {
    return nullptr;
  }

  auto* type = dictionary->FindStringKey(kTypeKey);
  if (!type) {
    return nullptr;
  }

  auto state = ledger::Grant::New();

  state->promotion_id = *promotion_id;
  state->alt_currency = *alt_currency;
  state->expiry_time = *expiry_time;
  state->probi = *probi;
  state->type = *type;

  return state;
}

}  // namespace brave_rewards
