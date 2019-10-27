/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/grant_response_json_state.h"

namespace brave_rewards {

const char kProtocolVersionKey[] = "protocolVersion";
const char kMinimumReconcileTimestampKey[] = "minimumReconcileTimestamp";
const char kTypeKey[] = "type";

GrantResponseJsonState::GrantResponseJsonState() = default;

GrantResponseJsonState::~GrantResponseJsonState() = default;

ledger::GrantResponsePtr GrantResponseJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* protocol_version = dictionary->FindIntKey(kProtocolVersionKey);
  if (!protocol_version) {
    return nullptr;
  }

  auto* minimum_reconcile_timestamp =
      dictionary->FindIntKey(kMinimumReconcileTimestampKey);
  if (!minimum_reconcile_timestamp) {
    return nullptr;
  }

  auto* type = dictionary->FindIntKey(kTypeKey);
  if (!type) {
    return nullptr;
  }

  auto state = ledger::GrantsProperties::New();

  state->protocol_version = *protocol_version;
  state->minimum_reconcile_timestamp = *minimum_reconcile_timestamp;
  state->type = *type;

  return state;
}

}  // namespace brave_rewards
