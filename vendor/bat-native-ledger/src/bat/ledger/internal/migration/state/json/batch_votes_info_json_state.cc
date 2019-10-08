/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/batch_votes_info_json_state.h"

namespace brave_rewards {

const char kSurveyorIdKey[] = "surveyorId";
const char kProofKey[] = "proof";

BatchVotesInfoJsonState::BatchVotesInfoJsonState() = default;

BatchVotesInfoJsonState::~BatchVotesInfoJsonState() = default;

ledger::BatchVotePtr BatchVotesInfoJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* surveyor_id = dictionary->FindStringKey(kSurveyorIdKey);
  if (!surveyor_id) {
    return nullptr;
  }

  auto* proof = dictionary->FindStringKey(kProofKey);
  if (!proof) {
    return nullptr;
  }

  auto state = ledger::BatchVote::New();

  state->surveyor_id = *surveyor_id;
  state->proof = *proof;

  return state;
}

}  // namespace brave_rewards
