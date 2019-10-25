/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_HOURLY_FREQUENCY_CAP_H_
#define BAT_ADS_INTERNAL_HOURLY_FREQUENCY_CAP_H_

#include <string>

#include "bat/ads/internal/exclusion_rules/exclusion_rule.h"

namespace ads {
struct AdInfo;
class FrequencyCapping;

class PerHourFrequencyCap final : public ExclusionRule {
 public:
  explicit PerHourFrequencyCap(
      const FrequencyCapping& frequency_capping) :
      frequency_capping_(frequency_capping) {
    }

  bool ShouldExclude(
    const AdInfo& ad) override;

  const std::string& GetLastReason() const override;

 private:
  const FrequencyCapping& frequency_capping_;  // NOT OWNED

  std::string reason_for_exclusion_;

  bool DoesAdRespectPerHourCap(
      const AdInfo& ad) const;
};
}  // namespace ads

#endif  // BAT_ADS_INTERNAL_HOURLY_FREQUENCY_CAP_H_
