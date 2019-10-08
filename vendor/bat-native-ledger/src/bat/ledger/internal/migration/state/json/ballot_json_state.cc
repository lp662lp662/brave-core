/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/wallet_info_json_state.h"

namespace brave_rewards {

const char kViewingIdKey[] = "viewingId";
const char kSurveyorIdKey[] = "surevyorId";
const char kPublisher[] = "publisher";
const char kOffset[] = "offset";
const char kPrepareBallot[] = "prepareBallot";
const char kDelayStamp[] = "delayStamp";

BallotJsonState::BallotJsonState() = default;

BallotJsonState::~BallotJsonState() = default;

ledger::BallotPtr BallotJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* viewing_id = dictionary->FindStringKey(kViewingIdKey);
  if (!viewing_id) {
    return nullptr;
  }

  auto* surveyor_id = dictionary->FindStringKey(kSurveyorIdKey);
  if (!surveyor_id) {
    return nullptr;
  }

  auto* publisher = dictionary->FindStringKey(kPublisher);
  if (!publisher) {
    return nullptr;
  }

  auto* offset = dictionary->FindIntKey(kOffset);
  if (!offset) {
    return nullptr;
  }

  auto* prepare_ballot = dictionary->FindStringKey(kPrepareBallot);
  if (!prepare_ballot) {
    return nullptr;
  }

  auto* delay_stamp = dictionary->FindIntKey(kDelayStamp);
  if (!delay_stamp) {
    return nullptr;
  }

  auto state = ledger::Ballot::New();

  state->viewing_id = *viewing_id;
  state->surveyor_id = *surveyor_id;
  state->publisher = *publisher;
  state->offset = *offset;
  state->prepare_ballot = *prepare_ballot;
  state->delay_stamp = *delay_stamp;

  return state;
}

}  // namespace brave_rewards
