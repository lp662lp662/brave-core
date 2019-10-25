/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <deque>

#include "bat/ads/internal/exclusion_rules/bob/minimum_wait_time.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/ads_client.h"
#include "bat/ads/internal/time.h"
#include "bat/ads/internal/client.h"
#include "bat/ads/internal/ads_impl.h"

#include "bat/ads/ad_info.h"

namespace ads {

bool MinimumWaitTime::DoesRespectMinimumWaitTime() {
  if (ads_.IsMobile()) {
    return true;
  }

  auto history = frequency_capping_.GetAdsHistory();

  auto respects_hour_limit = Check1(history);
  auto respects_minimum_wait_time = Check2(history);

  std::ostringstream string_stream;
  string_stream << "DoesRespectMinimumWaitTime:    respects_hour_limit: " <<
      respects_hour_limit << "    respects_minimum_wait_time: " <<
      respects_minimum_wait_time;
  reason_for_exclusion_ = string_stream.str();

  return respects_hour_limit && respects_minimum_wait_time;
}

const std::string& MinimumWaitTime::GetLastReason() const {
    return reason_for_exclusion_;
}

bool MinimumWaitTime::Check1(
  const std::deque<uint64_t>& history) const {
  auto hour_window = base::Time::kSecondsPerHour;
  auto hour_allowed = ads_client_.GetAdsPerHour();

  auto respects_hour_limit =
      frequency_capping_.DoesHistoryRespectCapForRollingTimeConstraint(
          history, hour_window, hour_allowed);

  return respects_hour_limit;
}

bool MinimumWaitTime::Check2(
  const std::deque<uint64_t>& history) const {
  auto hour_window = base::Time::kSecondsPerHour;
  auto hour_allowed = ads_client_.GetAdsPerHour();
  auto minimum_wait_time = hour_window / hour_allowed;

  auto respects_minimum_wait_time =
      frequency_capping_.DoesHistoryRespectCapForRollingTimeConstraint(
          history, minimum_wait_time, 0);

  return respects_minimum_wait_time;
}

}  // namespace ads
