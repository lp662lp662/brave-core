/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_MINIMUM_WAIT_TIME_H_
#define BAT_ADS_INTERNAL_MINIMUM_WAIT_TIME_H_

#include <string>
#include <deque>

#include "bat/ads/internal/exclusion_rules/frequency/permission_rule.h"

namespace ads {

struct AdInfo;
class AdsImpl;
class AdsClient;
class FrequencyCapping;

class MinimumWaitTimeFrequencyCap : public PermissionRule {
 public:
  explicit MinimumWaitTimeFrequencyCap(
      const AdsImpl* ads,
      const AdsClient* ads_client,
      const FrequencyCapping* frequency_capping)
      : ads_(ads),
        ads_client_(ads_client),
        frequency_capping_(frequency_capping) {
  }
  virtual ~MinimumWaitTimeFrequencyCap() = default;

  bool IsAllowed() override;

  const std::string& GetLastReason() const override;

 private:
  const AdsImpl* ads_;  // NOT OWNED
  const AdsClient* ads_client_;  // NOT OWNED
  const FrequencyCapping* frequency_capping_;  // NOT OWNED

  std::string reason_for_exclusion_;

  bool AreAdsPerHourBelowAllowedThreshold(
      const std::deque<uint64_t>& history) const;
  bool AreAdsAllowedAfterMinimumWaitTime(
      const std::deque<uint64_t>& history) const;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_MINIMUM_WAIT_TIME_H_
