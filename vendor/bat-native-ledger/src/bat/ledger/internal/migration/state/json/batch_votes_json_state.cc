/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/batch_votes_json_state.h"

namespace brave_rewards {

const char kPublisherKey[] = "publisher";
const char kBatchVotesInfoKey[] = "batchVotesInfo";

BatchVotesJsonState::BatchVotesJsonState() = default;

BatchVotesJsonState::~BatchVotesJsonState() = default;

ledger::BatchVotesPtr BatchVotesJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* publisher = dictionary->FindStringKey(kPublisherKey);
  if (!publisher) {
    return nullptr;
  }

  auto* batch_votes = dictionary->FindListKey(kBatchVotesInfoKey);
  if (!batch_votes) {
    return nullptr;
  }

  auto state = ledger::BatchVotes::New();

  state->publisher = *publisher;

  for (const auto& batch_vote : *batch_votes) {
    state->batch_votes.push_back(batch_vote);
  }

  return state;
}

}  // namespace brave_rewards
