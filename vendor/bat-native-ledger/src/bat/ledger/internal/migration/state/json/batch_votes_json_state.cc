/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/batch_votes_json_state.h"
#include "bat/ledger/internal/migration/state/json/batch_votes_info_json_state.h"  // NOLINT
#include "bat/ledger/internal/migration/state/json/json_helper.h"

namespace ledger {

namespace {

const char kPublisherKey[] = "publisher";
const char kBatchVotesInfoKey[] = "batchVotesInfo";

}  // namespace

BatchVotesJsonState::BatchVotesJsonState() = default;

BatchVotesJsonState::~BatchVotesJsonState() = default;

bool BatchVotesJsonState::FromJson(
    const std::string& json,
    BatchVotes* state) const {
  auto* dictionary = JsonHelper::FromJson(json);
  if (!dictionary) {
    return false;
  }

  return FromDict(dictionary, state);
}

bool BatchVotesJsonState::FromDict(
    const base::DictionaryValue* dictionary,
    BatchVotes* state) const {
  DCHECK(dictionary);
  DCHECK(state);

  BatchVotes batch_votes;

  auto* publisher = dictionary->FindStringKey(kPublisherKey);
  if (!publisher) {
    return false;
  }
  batch_votes.publisher = *publisher;

  const auto* batch_votes_info_list =
      dictionary->FindListKey(kBatchVotesInfoKey);
  if (!batch_votes_info_list) {
    return false;
  }

  for (const auto& batch_votes_info_value : batch_votes_info_list->GetList()) {
    const base::DictionaryValue* batch_votes_info_dictionary;
    batch_votes_info_value.GetAsDictionary(&batch_votes_info_dictionary);

    BatchVotesInfo batch_votes_info;
    const ledger::BatchVotesInfoJsonState batch_votes_info_state;
    if (!batch_votes_info_state.FromDict(batch_votes_info_dictionary,
        &batch_votes_info)) {
      continue;
    }

    batch_votes.batch_votes_info.push_back(batch_votes_info);
  }

  *state = batch_votes;

  return true;
}

}  // namespace ledger
