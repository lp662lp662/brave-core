/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/transaction_ballot_json_state.h"

namespace brave_rewards {

const char kPublisherKey[] = "publisher";
const char kOffsetKey[] = "offset";

TransactionBallotJsonState::TransactionBallotJsonState() = default;

TransactionBallotJsonState::~TransactionBallotJsonState() = default;

ledger::WalletPtr TransactionBallotJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* publisher = dictionary.FindStringKey(kPublisherKey);
  if (!publisher) {
    return nullptr;
  }

  auto* offset = dictionary.FindIntKey(kPublisherKey);
  if (!offset) {
    return nullptr;
  }

  auto state = ledger::TransactionBallot::New();

  state->publisher = *publisher;
  state->offset = *offset;

  return state;
}

}  // namespace brave_rewards
