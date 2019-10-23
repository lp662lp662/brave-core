/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_FREQUENCY_CAPPING_H_
#define BAT_ADS_INTERNAL_FREQUENCY_CAPPING_H_

#include <stdint.h>
#include <deque>
#include <string>

namespace ads {

class Client;
class AdsClient;

class FrequencyCapping {
 public:
  FrequencyCapping(Client* client_state, AdsClient* ads_client) :
      client_state_(client_state),
      ads_client_(ads_client) {
  }

  ~FrequencyCapping() = default;

  bool DoesHistoryRespectCapForRollingTimeConstraint(
      const std::deque<uint64_t> history,
      const uint64_t time_constraint_in_seconds,
      const uint64_t cap) const;

  std::deque<uint64_t> GetCreativeSetHistoryForUuid(
      const std::string& uuid) const;

  std::deque<uint64_t> GetAdsHistoryForUuid(
      const std::string& uuid) const;

  std::deque<uint64_t> GetCampaignForUuid(
      const std::string& uuid) const;

  AdsClient* GetAdsClient() const {
    return ads_client_;
  }

 private:
  Client* client_state_;  // NOT OWNED

  AdsClient* ads_client_;  // NOT OWNED
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_FREQUENCY_CAPPING_H_
