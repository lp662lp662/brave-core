/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/publisher_json_state.h"
#include "bat/ledger/public/interfaces/ledger.mojom.h"

namespace brave_rewards {

using PublisherStatus = mojom::PublisherStatus;

const char kIdKey[] = "id";
const char kDurationKey[] = "duration";
const char kScoreKey[] = "score";
const char kVisitsKey[] = "visits";
const char kPercentKey[] = "percent";
const char kWeightKey[] = "weight";
const char kStatusKey[] = "status";
const char kVerifiedKey[] = "verified";

PublisherJsonState::PublisherJsonState() = default;

PublisherJsonState::~PublisherJsonState() = default;

ledger::PublisherPtr PublisherJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* id = dictionary.FindIntKey(kIdKey);
  if (!id) {
    return nullptr;
  }

  auto* duration = dictionary.FindIntKey(kDurationKey);
  if (!duration) {
    return nullptr;
  }

  auto* score = dictionary.FindDoubleKey(kScoreKey);
  if (!score) {
    return nullptr;
  }

  auto* visits = dictionary.FindIntKey(kVisitsKey);
  if (!visits) {
    return nullptr;
  }

  auto* percent = dictionary.FindIntKey(kPercentKey);
  if (!percent) {
    return nullptr;
  }

  base::Optional<int> status;
  auto* verified = dictionary.FindBoolKey(kVerifiedKey);
  if (!verified) {
     status = dictionary.FindIntKey(kStatusKey)
     if (!status) {
       *status = PublisherStatus::NOT_VERIFIED
    }
  } else {
    // Transition legacy verified flag to status enum
    if (*verified) {
      *status = PublisherStatus::VERIFIED
    } else {
      *status = PublisherStatus::NOT_VERIFIED
    }
  }

  auto state = ledger::Publisher::New();

  state->id = *id;
  state->duration = *duration;
  state->score = *score;
  state->visits = *visits;
  state->percent = *percent;
  state->status = *status;

  return state;
}

}  // namespace brave_rewards
