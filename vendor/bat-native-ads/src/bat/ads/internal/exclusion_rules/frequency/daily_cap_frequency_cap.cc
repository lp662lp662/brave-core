/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/exclusion_rules/frequency/daily_cap_frequency_cap.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/internal/time.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/internal/client.h"

#include "bat/ads/ad_info.h"

namespace ads {

bool DailyCapFrequencyCap::ShouldExclude(
    const AdInfo& ad) const {
  if (!DoesAdRespectDailyCampaignCap(ad)) {
    frequency_capping_.GetAdsClient()->Log(__FILE__, __LINE__,
        ::ads::LogLevel::LOG_WARNING)->stream() << "campaignId " <<
        ad.campaign_id << " has exceeded the frequency capping for dailyCap";

    return true;
  }
  return false;
}

bool DailyCapFrequencyCap::DoesAdRespectDailyCampaignCap(
    const AdInfo& ad) const {
  auto campaign = frequency_capping_.GetCampaignForUuid(ad.campaign_id);
  auto day_window = base::Time::kSecondsPerHour * base::Time::kHoursPerDay;

  return frequency_capping_.DoesHistoryRespectCapForRollingTimeConstraint(
      campaign, day_window, ad.daily_cap);
}

}  // namespace ads
