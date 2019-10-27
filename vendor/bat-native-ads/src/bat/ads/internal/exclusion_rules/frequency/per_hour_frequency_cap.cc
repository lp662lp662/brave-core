/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/exclusion_rules/frequency/per_hour_frequency_cap.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/internal/time.h"
#include "bat/ads/internal/client.h"

#include "bat/ads/ad_info.h"

namespace ads {

bool PerHourFrequencyCap::ShouldExclude(
  const AdInfo& ad) {
  if (!DoesAdRespectPerHourCap(ad)) {
    std::ostringstream string_stream;
    string_stream  << "adUUID " << ad.uuid <<
        " has exceeded the frequency capping for perHour";
    reason_for_exclusion_ = string_stream.str();
    return true;
  }
  return false;
}

const std::string& PerHourFrequencyCap::GetLastReason() const {
    return reason_for_exclusion_;
}

bool PerHourFrequencyCap::DoesAdRespectPerHourCap(
    const AdInfo& ad) const {
  auto ads_shown = frequency_capping_->GetAdsHistoryForUuid(ad.uuid);
  auto hour_window = base::Time::kSecondsPerHour;

  return frequency_capping_->DoesHistoryRespectCapForRollingTimeConstraint(
      ads_shown, hour_window, 1);
}

}  // namespace ads
