/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_DAILY_FREQUENCY_CAP_H_
#define BAT_ADS_INTERNAL_DAILY_FREQUENCY_CAP_H_

#include "bat/ads/internal/exclusion_rules/exclusion_rule.h"

namespace ads {
    struct AdInfo;
    class FrequencyCapping;

    class DailyFrequencyCap : public ExclusionRule {
    public:
        DailyFrequencyCap(const FrequencyCapping& frequency_capping) 
        : frequency_capping_(frequency_capping) { 
        }
        bool IsExcluded(
            const AdInfo& ad) const override;

    private:
        const FrequencyCapping& frequency_capping_;

        bool DoesAdRespectPerDayCap(
            const AdInfo& ad) const;
    };
}  // namespace ads

#endif // BAT_ADS_INTERNAL_DAILY_FREQUENCY_CAP_H_
