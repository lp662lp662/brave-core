/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/exclusion_rules/bob/per_day_limit.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/ads_client.h"
#include "bat/ads/internal/time.h"
#include "bat/ads/internal/client.h"

#include "bat/ads/ad_info.h"

namespace ads {

bool PerDayLimit::DoesAbide() {
  auto respects_day_limit = AreAdsPerDayBelowAllowedThreshold();

  std::ostringstream string_stream;
  string_stream << "DoesHistoryRespectAdsPerDayLimit:    respects_day_limit: "
      << respects_day_limit;
  reason_for_exclusion_ = string_stream.str();

  return respects_day_limit;
}

const std::string& PerDayLimit::GetLastReason() const {
    return reason_for_exclusion_;
}

bool PerDayLimit::AreAdsPerDayBelowAllowedThreshold() const {
  auto history = frequency_capping_.GetAdsHistory();

  auto day_window = base::Time::kSecondsPerHour * base::Time::kHoursPerDay;
  auto day_allowed = ads_client_.GetAdsPerDay();

  auto respects_day_limit =
      frequency_capping_.DoesHistoryRespectCapForRollingTimeConstraint(
          history, day_window, day_allowed);

  return respects_day_limit;
}

}  // namespace ads
