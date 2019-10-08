/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/migrate_json_state.h"
#include "bat/ledger/internal/migration/state/json/wallet_info_json_state.h"

namespace brave_rewards {

MigrateJsonState::MigrateJsonState() = default;

MigrateJsonState::~MigrateJsonState() = default;

bool MigrateJsonState::Migrate(
    const std::string& json) {
  WalletInfoJsonState wallet_info_state;
  auto wallet_info = wallet_info_state.FromJson(json);

  TransactionBallotJsonState transaction_ballot_state;
  auto transaction_ballot = transaction_ballot_state.FromJson(json);

  return true;
}

}  // namespace brave_rewards
