/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/exclusion_rules/frequency/total_max_frequency_cap.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/internal/time.h"
#include "bat/ads/internal/client.h"

#include "bat/ads/ad_info.h"

namespace ads {

bool TotalMaxFrequencyCap::ShouldExclude(
    const AdInfo& ad) const {
  if (!DoesAdRespectMaximumCap(ad)) {
    std::ostringstream string_stream;
    string_stream << "creativeSetId " << ad.creative_set_id <<
        " has exceeded the frequency capping for totalMax";
    return true;
  }
  return false;
}

const std::string& TotalMaxFrequencyCap::GetLastReason() const {
    return reason_for_exclusion_;
}

bool TotalMaxFrequencyCap::DoesAdRespectMaximumCap(
    const AdInfo& ad) const {
  auto creative_set = frequency_capping_.GetCreativeSetHistoryForUuid(
    ad.creative_set_id);

  if (creative_set.size() >= ad.total_max) {
    return false;
  }

  return true;
}

}  // namespace ads
