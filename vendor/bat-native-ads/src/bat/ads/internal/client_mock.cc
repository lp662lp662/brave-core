/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>
#include <string>

#include "bat/ads/internal/client_mock.h"

#include "bat/ads/ad_history_detail.h"
#include "bat/ads/internal/time.h"
#include "base/guid.h"

namespace ads {

void ClientMock::ConfigureWithDataForTotalMaximumFrequencyCappingTest(
    const std::string& creative_set_id, const uint8_t ad_instances) {
  auto now_in_seconds = Time::NowInSeconds();

  for (uint8_t i = 0; i < ad_instances; i++) {
    AppendTimestampToCreativeSetHistoryForUuid(creative_set_id, now_in_seconds);
  }
}

void ClientMock::ConfigureWithDataForAddHistory(
    const std::string uuid,
    const int64_t time_offset_per_ad_in_seconds,
    const uint8_t ad_instances) {
  auto now_in_seconds = Time::NowInSeconds();

  auto ad_history_detail = std::make_unique<AdHistoryDetail>();
  ad_history_detail->uuid = base::GenerateGUID();
  ad_history_detail->ad_content.uuid = uuid;

  for (uint8_t i = 0; i < ad_instances; i++) {
    now_in_seconds += time_offset_per_ad_in_seconds;

    ad_history_detail->timestamp_in_seconds = now_in_seconds;
    AppendAdToAdsShownHistory(*ad_history_detail);
  }
}

void ClientMock::ConfigureWithDataForPerHourFrequencyCappingTest(
    const std::string& creative_set_id,
    const int64_t time_offset_per_ad_in_seconds,
    const uint8_t ad_instances) {
  auto now_in_seconds = Time::NowInSeconds();

  for (uint8_t i = 0; i < ad_instances; i++) {
    now_in_seconds += time_offset_per_ad_in_seconds;
    AppendTimestampToCreativeSetHistoryForUuid(creative_set_id, now_in_seconds);
  }
}

void ClientMock::ConfigureWithDataForDailyCampaignHistory(
    const std::string campaign_id,
    const int64_t time_offset_per_ad_in_seconds,
    const uint8_t ad_instances) {
  auto now_in_seconds = Time::NowInSeconds();

  for (uint8_t i = 0; i < ad_instances; i++) {
    now_in_seconds += time_offset_per_ad_in_seconds;
    AppendTimestampToCampaignHistoryForUuid(campaign_id, now_in_seconds);
  }
}

}  // namespace ads
