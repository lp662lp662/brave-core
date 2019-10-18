/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/exclusion_rules/frequency/hourly_frequency_cap.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/internal/time.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/internal/client.h"

#include "bat/ads/ad_info.h"

namespace ads {
    bool HourlyFrequencyCap::IsExcluded(
        const AdInfo& ad) const {
      if (!DoesAdRespectPerHourCap(ad)) {
        frequency_capping_.GetAdsClient()->Log(__FILE__, __LINE__, ::ads::LogLevel::LOG_WARNING)->stream() << "adUUID " << ad.uuid
                      << " has exceeded the frequency capping for perHour";

        return true;
      }
      return false;
    }

    bool HourlyFrequencyCap::DoesAdRespectPerHourCap(
        const AdInfo& ad) const {
      auto ads_shown = frequency_capping_.GetAdsShownForId(ad.uuid);
      auto hour_window = base::Time::kSecondsPerHour;

      return frequency_capping_.HistoryRespectsRollingTimeConstraint(
          ads_shown, hour_window, 1);
    }
}  // namespace ads
