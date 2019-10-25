/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_PER_DAY_LIMIT_H_
#define BAT_ADS_INTERNAL_PER_DAY_LIMIT_H_

#include <string>

namespace ads {

struct AdInfo;
class AdsClient;
class FrequencyCapping;

class PerDayLimit final {
 public:
  explicit PerDayLimit(
      const AdsClient& ads_client,
      const FrequencyCapping& frequency_capping):
      ads_client_(ads_client),
      frequency_capping_(frequency_capping) {
  }
  bool DoesRespectPerDayLimit() const;

  const std::string& GetLastReason() const;

 private:
  const AdsClient& ads_client_;
  const FrequencyCapping& frequency_capping_;

  std::string reason_for_exclusion_;

  bool Check1() const;

};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_PER_DAY_LIMIT_H_
