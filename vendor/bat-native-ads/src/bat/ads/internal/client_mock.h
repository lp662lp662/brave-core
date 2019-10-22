/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_CLIENT_MOCK_H_
#define BAT_ADS_INTERNAL_CLIENT_MOCK_H_

#include "bat/ads/internal/client.h"

namespace ads {

class ClientMock : public Client {
 public:
  ClientMock(AdsImpl* ads, AdsClient* ads_client): Client(ads, ads_client) { }

  void ConfigureWithDataForMaximumFrequencyCappingTest(const std::string& creative_set_id, uint8_t ad_instances);

};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_CLIENT_MOCK_H_
