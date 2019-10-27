/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/batch_votes_info_json_state.h"  // NOLINT
#include "bat/ledger/internal/migration/state/json/json_helper.h"

namespace ledger {

namespace {

const char kSurveyorIdKey[] = "surveyorId";
const char kProofKey[] = "proof";

}  // namespace

BatchVotesInfoJsonState::BatchVotesInfoJsonState() = default;

BatchVotesInfoJsonState::~BatchVotesInfoJsonState() = default;

bool BatchVotesInfoJsonState::FromJson(
    const std::string& json,
    BatchVotesInfo* state) const {
  auto* dictionary = JsonHelper::FromJson(json);
  if (!dictionary) {
    return false;
  }

  return FromDict(dictionary, state);
}

bool BatchVotesInfoJsonState::FromDict(
    const base::DictionaryValue* dictionary,
    BatchVotesInfo* state) const {
  DCHECK(dictionary);
  DCHECK(state);

  BatchVotesInfo batch_votes_info;

  const auto* surveyor_id = dictionary->FindStringKey(kSurveyorIdKey);
  if (!surveyor_id) {
    return false;
  }
  batch_votes_info.surveyor_id = *surveyor_id;

  const auto* proof = dictionary->FindStringKey(kProofKey);
  if (!proof) {
    return false;
  }

  batch_votes_info.proof = *proof;

  *state = batch_votes_info;

  return true;
}

}  // namespace ledger
