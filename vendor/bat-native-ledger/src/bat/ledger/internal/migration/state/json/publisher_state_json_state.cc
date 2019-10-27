/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/publisher_state_json_state.h"
#include "bat/ledger/internal/migration/state/json/report_balance_json_state.h"

namespace brave_rewards {

const char kMinPublisherDuration[] = "min_publisher_duration";
const char kMinVisitsKey[] = "min_visits";
const char kAllowNonVerifiedKey[] = "allow_non_verified";
const char kAllowVideosKey[] = "allow_videos";
const char kMonthlyBalancesKey[] = "monthly_balances";
const char kProcessedPendingPublishersKey[] = "processed_pending_publishers";

// TODO(Terry Mancey): migrate_score_2 logic needs to apply when migrating from
// JSON to the database

PublisherStateJsonState::PublisherStateJsonState() = default;

PublisherStateJsonState::~PublisherStateJsonState() = default;

ledger::PublisherStatePtr PublisherStateJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* min_publisher_duration =
      dictionary.FindIntKey(kMinPublisherDurationKey);
  if (!min_publisher_duration) {
    return nullptr;
  }

  auto* min_visits = dictionary.FindIntKey(kMinVisitsKey);
  if (!min_visits) {
    return nullptr;
  }

  auto* allow_non_verified = dictionary.FindBoolKey(kAllowNonVerifiedKey);
  if (!allow_non_verified) {
    return nullptr;
  }

  auto* allow_videos = dictionary.FindBoolKey(kAllowVideosKey);
  if (!allow_videos) {
    return nullptr;
  }

  auto* monthly_balances = dictionary.FindListKey(kMonthlyBalancesKey);
  if (!monthly_balances) {
    return nullptr;
  }

  auto* processed_pending_publishers =
      dictionary.FindListKey(kProcessedPendingPublishersKey);
  if (!processed_pending_publishers) {
    return nullptr;
  }

  auto state = ledger::PublisherState::New();

  state->min_publisher_duration = *min_publisher_duration;
  state->min_visits = *min_visits;
  state->allow_non_verified = *allow_non_verified;
  state->allow_videos = *allow_videos;

  for (const auto& monthly_balance_dictionary : *monthly_balances) {
    ReportBalanceJsonState report_balance_state();
    auto report_balance =
        report_balance_state.FromJson(monthly_balance_dictionary);

    state->monthly_balances.push_back(report_balance);
  }

  for (const auto& processed_pending_publisher :
      *processed_pending_publishers) {
    state->processed_pending_publishers.push_back(processed_pending_publisher);
  }

  return state;
}

}  // namespace brave_rewards
