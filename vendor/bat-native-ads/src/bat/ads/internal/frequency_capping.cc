/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cstdint>
#include <deque>
#include <string>

#include "bat/ads/internal/frequency_capping.h"
#include "bat/ads/ad_info.h"
#include "bat/ads/internal/client.h"
#include "bat/ads/internal/logging.h"
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

    bool FrequencyCapping::DoesAdRespectMaximumCap(
        const AdInfo& ad) const {
      auto creative_set = GetCreativeSetForId(ad.creative_set_id);
      if (creative_set.size() >= ad.total_max) {
        return false;
      }

      return true;
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

    bool FrequencyCapping::DoesAdRespectPerHourCap(
        const AdInfo& ad) const {
      auto ads_shown = GetAdsShownForId(ad.uuid);
      auto hour_window = base::Time::kSecondsPerHour;

      return HistoryRespectsRollingTimeConstraint(
          ads_shown, hour_window, 1);
    }

    bool FrequencyCapping::DoesAdRespectPerDayCapping(
        const AdInfo& ad) const {
      auto creative_set = GetCreativeSetForId(ad.creative_set_id);
      auto day_window = base::Time::kSecondsPerHour * base::Time::kHoursPerDay;

      return HistoryRespectsRollingTimeConstraint(
          creative_set, day_window, ad.per_day);
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

    bool FrequencyCapping::DoesAdRespectDailyCapping(
        const AdInfo& ad) const {
      auto campaign = GetCampaignForId(ad.campaign_id);
      auto day_window = base::Time::kSecondsPerHour * base::Time::kHoursPerDay;

      return HistoryRespectsRollingTimeConstraint(
          campaign, day_window, ad.daily_cap);
    }

    bool FrequencyCapping::IsCapped(
        const AdInfo& ad) const {

      if (!DoesAdRespectMaximumCap(ad)) {
//        BLOG(WARNING) << "creativeSetId " << ad.creative_set_id
//                      << " has exceeded the frequency capping for totalMax";

        return true;
      }

      if (!DoesAdRespectPerHourCap(ad)) {
//        BLOG(WARNING) << "adUUID " << ad.uuid
//                      << " has exceeded the frequency capping for perHour";

        return true;
      }

      if (!DoesAdRespectPerDayCapping(ad)) {
//        BLOG(WARNING) << "creativeSetId " << ad.creative_set_id
//                      << " has exceeded the frequency capping for perDay";

        return true;
      }

      if (!DoesAdRespectDailyCapping(ad)) {
//        BLOG(WARNING) << "campaignId " << ad.campaign_id
//                      << " has exceeded the frequency capping for dailyCap";

        return true;
      }

      return false;
    }

}  // namespace ads
