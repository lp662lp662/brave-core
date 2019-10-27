/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/ballot_json_state.h"
#include "bat/ledger/internal/migration/state/json/json_helper.h"

namespace ledger {

namespace {

const char kViewingIdKey[] = "viewingId";
const char kSurveyorIdKey[] = "surveyorId";
const char kPublisher[] = "publisher";
const char kOffset[] = "offset";
const char kPrepareBallot[] = "prepareBallot";
const char kDelayStamp[] = "delayStamp";

}  // namespace

BallotJsonState::BallotJsonState() = default;

BallotJsonState::~BallotJsonState() = default;

bool BallotJsonState::FromJson(
    const std::string& json,
    Ballot* state) const {
  auto* dictionary = JsonHelper::FromJson(json);
  if (!dictionary) {
    return false;
  }

  return FromDict(dictionary, state);
}

bool BallotJsonState::FromDict(
    const base::DictionaryValue* dictionary,
    Ballot* state) const {
  DCHECK(dictionary);
  DCHECK(state);

  Ballot ballot;

  const auto* viewing_id = dictionary->FindStringKey(kViewingIdKey);
  if (!viewing_id) {
    return false;
  }
  ballot.viewing_id = *viewing_id;

  const auto* surveyor_id = dictionary->FindStringKey(kSurveyorIdKey);
  if (!surveyor_id) {
    return false;
  }
  ballot.surveyor_id = *surveyor_id;

  const auto* publisher = dictionary->FindStringKey(kPublisher);
  if (!publisher) {
    return false;
  }
  ballot.publisher = *publisher;

  auto offset = dictionary->FindIntKey(kOffset);
  if (!offset) {
    return false;
  }
  ballot.offset = *offset;

  const auto* prepare_ballot = dictionary->FindStringKey(kPrepareBallot);
  if (!prepare_ballot) {
    return false;
  }
  ballot.prepare_ballot = *prepare_ballot;

  auto delay_stamp = dictionary->FindIntKey(kDelayStamp);
  if (!delay_stamp) {
    return false;
  }
  ballot.delay_stamp = *delay_stamp;

  *state = ballot;

  return true;
}

}  // namespace ledger
