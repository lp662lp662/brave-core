/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/ad_info.h"
#include "bat/ads/internal/client.h"
#include "bat/ads/internal/time.h"

namespace ads {

    bool FrequencyCapping::HistoryRespectsRollingTimeConstraint(
        const std::deque<uint64_t> history,
        const uint64_t seconds_window,
        const uint64_t allowable_ad_count) const {
      uint64_t recent_count = 0;

      auto now_in_seconds = Time::NowInSeconds();

      for (const auto& timestamp_in_seconds : history) {
        if (now_in_seconds - timestamp_in_seconds < seconds_window) {
          recent_count++;
        }
      }

      if (recent_count <= allowable_ad_count) {
        return true;
      }

      return false;
    }

    std::deque<uint64_t> FrequencyCapping::GetCreativeSetForId(
        const std::string& id) const {
      std::deque<uint64_t> creative_set = {};

      auto creative_set_history = client_state_->GetCreativeSetHistory();
      if (creative_set_history.find(id) != creative_set_history.end()) {
        creative_set = creative_set_history.at(id);
      }

      return creative_set;
    }

    std::deque<uint64_t> FrequencyCapping::GetAdsShownForId(
        const std::string& id) const {
      std::deque<uint64_t> ads_shown = {};

      auto ads_shown_history = client_state_->GetAdsShownHistory();
      for (const auto& ad_shown : ads_shown_history) {
        if (ad_shown.ad_content.uuid == id) {
          ads_shown.push_back(ad_shown.timestamp_in_seconds);
        }
      }

      return ads_shown;
    }

    std::deque<uint64_t> FrequencyCapping::GetCampaignForId(
        const std::string& id) const {
      std::deque<uint64_t> campaign = {};

      auto campaign_history = client_state_->GetCampaignHistory();
      if (campaign_history.find(id) != campaign_history.end()) {
        campaign = campaign_history.at(id);
      }

      return campaign;
    }
}  // namespace ads
