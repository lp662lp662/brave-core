/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_FREQUENCY_CAPPING_H_
#define BAT_ADS_INTERNAL_FREQUENCY_CAPPING_H_

namespace ads {
  struct AdInfo;
  class Client;

  class FrequencyCapping {
    public:
      FrequencyCapping(Client* client_state)
          : client_state_(client_state) {
      }
      ~FrequencyCapping() { }

      void IsBob() const;

      bool IsCapped(
          const AdInfo& ad) const;

  private:
      Client* client_state_;

      bool HistoryRespectsRollingTimeConstraint(
          const std::deque<uint64_t> history,
          const uint64_t seconds_window,
          const uint64_t allowable_ad_count) const;

      std::deque<uint64_t> GetCreativeSetForId(
          const std::string& id) const;

      std::deque<uint64_t> GetAdsShownForId(
          const std::string& id) const;

      std::deque<uint64_t> GetCampaignForId(
          const std::string& id) const;

      bool DoesAdRespectMaximumCap(
          const AdInfo& ad) const;
      bool DoesAdRespectPerHourCap(
          const AdInfo& ad) const;
      bool DoesAdRespectPerDayCapping(
          const AdInfo& ad) const;
      bool DoesAdRespectDailyCapping(
          const AdInfo& ad) const;

  };

}  // namespace ads

#endif // BAT_ADS_INTERNAL_FREQUENCY_CAPPING_H_
