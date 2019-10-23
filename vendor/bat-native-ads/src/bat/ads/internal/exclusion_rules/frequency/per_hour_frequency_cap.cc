/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/exclusion_rules/frequency/per_hour_frequency_cap.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/internal/time.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/internal/client.h"

#include "bat/ads/ad_info.h"

namespace ads {

bool PerHourFrequencyCap::ShouldExclude(
  const AdInfo& ad) const {
  if (!DoesAdRespectPerHourCap(ad)) {
    frequency_capping_.GetAdsClient()->Log(__FILE__, __LINE__,
    ::ads::LogLevel::LOG_WARNING)->stream() << "adUUID " << ad.uuid
    << " has exceeded the frequency capping for perHour";

    return true;
  }
  return false;
}

bool PerHourFrequencyCap::DoesAdRespectPerHourCap(
    const AdInfo& ad) const {
  auto ads_shown = frequency_capping_.GetAdsHistoryForUuid(ad.uuid);
  auto hour_window = base::Time::kSecondsPerHour;

  LOG(INFO) << "\033[1;32m[MAS] ads_shown.size = " << ads_shown.size() <<
  " hour_window = " << hour_window << "\033[0m";

  return frequency_capping_.DoesHistoryRespectCapForRollingTimeConstraint(
      ads_shown, hour_window, 1);
}

}  // namespace ads
