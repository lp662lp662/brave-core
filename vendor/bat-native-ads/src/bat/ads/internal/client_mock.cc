/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/client_mock.h"

#include "bat/ads/ad_history_detail.h"
#include "bat/ads/internal/time.h"
#include "base/guid.h"

namespace ads {

  void ClientMock::ConfigureWithDataForMaximumFrequencyCappingTest(const std::string& creative_set_id, const uint8_t ad_instances) {

    for(uint8_t i = 0; i < ad_instances; i++) {
      AppendCurrentTimeToCreativeSetHistory(creative_set_id);
    }
  }

  void ClientMock::ConfigureWithDataForAddHistory(const std::string uuid, const int64_t time_offset_per_ad_in_seconds, const uint8_t ad_instances) {

    auto ad_history_detail = std::make_unique<AdHistoryDetail>();
    ad_history_detail->timestamp_in_seconds = Time::NowInSeconds();
    ad_history_detail->uuid = base::GenerateGUID();
    ad_history_detail->ad_content.uuid = uuid;

    for(uint8_t i = 0; i < ad_instances; i++) {
      AppendAdToAdsShownHistory(*ad_history_detail);

      ad_history_detail->timestamp_in_seconds += time_offset_per_ad_in_seconds;
    }
  }

  void ClientMock::ConfigureWithDataForHourlyFrequencyCappingTest(const std::string& creative_set_id, const int64_t time_offset_per_ad_in_seconds, const uint8_t ad_instances) {
    // TODO:(masparrow) Figure out how to apply time offsets...
    for(uint8_t i = 0; i < ad_instances; i++) {
      AppendCurrentTimeToCreativeSetHistory(creative_set_id);
    }
  }
    void ClientMock::ConfigureWithDataForDailyCampaignHistory(const std::string campaign_id, const uint8_t ad_instances) {
          for(uint8_t i = 0; i < ad_instances; i++) {
      AppendCurrentTimeToCampaignHistory(campaign_id);
    }

    }

}  // namespace ads
