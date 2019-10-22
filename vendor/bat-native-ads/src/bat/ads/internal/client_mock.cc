/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/client_mock.h"

namespace ads {

  void ClientMock::ConfigureWithDataForMaximumFrequencyCappingTest(const std::string& creative_set_id, uint8_t ad_instances) {

    for(uint8_t i = 0; i < ad_instances; i++) {
      AppendCurrentTimeToCreativeSetHistory(creative_set_id);
    }
  }

}  // namespace ads
