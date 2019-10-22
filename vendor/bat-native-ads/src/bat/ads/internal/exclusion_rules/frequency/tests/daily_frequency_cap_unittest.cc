/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>

#include "testing/gtest/include/gtest/gtest.h"

#include "bat/ads/internal/exclusion_rules/exclusion_rule.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/internal/exclusion_rules/frequency/daily_frequency_cap.h"

#include "bat/ads/internal/client_mock.h"
#include "bat/ads/internal/ads_client_mock.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/ad_info.h"
#include "bat/ads/internal/time.h"

// npm run test -- brave_unit_tests --filter=Ads*

using std::placeholders::_1;
using ::testing::_;
using ::testing::Invoke;

namespace ads {

static const char* test_creative_set_id = "654f10df-fbc4-4a92-8d43-2edf73734a60";

static auto day_window = base::Time::kSecondsPerHour * base::Time::kHoursPerDay;

class AdsDailyFrequencyCapTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockAdsClient> mock_ads_client_;
  std::unique_ptr<AdsImpl> ads_;

  AdsDailyFrequencyCapTest() :
      mock_ads_client_(std::make_unique<MockAdsClient>()),
      ads_(std::make_unique<AdsImpl>(mock_ads_client_.get())) {
    // You can do set-up work for each test here
  }

  ~AdsDailyFrequencyCapTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)

    auto callback = std::bind(&AdsDailyFrequencyCapTest::OnAdsImpleInitialize,
        this, _1);
    ads_->Initialize(callback);  // TODO(masparrow): Null callback?
  }

  void OnAdsImpleInitialize(const Result result) {
    EXPECT_EQ(Result::SUCCESS, result);
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right before the
    // destructor)
  }
};

TEST_F(AdsDailyFrequencyCapTest, TestAdAllowed) {
  // Arrange
  ClientMock* client_mock = new ClientMock(ads_.get(),
      mock_ads_client_.get());

  FrequencyCapping frequency_capping(client_mock, mock_ads_client_.get());

  ExclusionRule* exclusionRule = new
      DailyFrequencyCap(frequency_capping);

  AdInfo ad;
  ad.creative_set_id = test_creative_set_id;
  ad.per_day = 2;

  // Act
  bool isAdExcluded = exclusionRule->IsExcluded(ad);
  // Assert
  EXPECT_FALSE(isAdExcluded);

  // Arrange
  client_mock->ConfigureWithDataForHourlyFrequencyCappingTest(test_creative_set_id, 0, 1);
  // Act
  isAdExcluded = exclusionRule->IsExcluded(ad);
  // Assert
  EXPECT_FALSE(isAdExcluded);

  // Arrange
  // 24hrs + 1s ago

  client_mock->ConfigureWithDataForHourlyFrequencyCappingTest(test_creative_set_id, -(day_window + 1), 1);
  // Act
  isAdExcluded = exclusionRule->IsExcluded(ad);
  // Assert
  EXPECT_FALSE(isAdExcluded);
}

TEST_F(AdsDailyFrequencyCapTest, TestAdExcluded) {
  // Arrange
  ClientMock* client_mock = new ClientMock(ads_.get(),
      mock_ads_client_.get());

  FrequencyCapping frequency_capping(client_mock, mock_ads_client_.get());

  ExclusionRule* exclusionRule = new
      DailyFrequencyCap(frequency_capping);

  AdInfo ad;
  ad.creative_set_id = test_creative_set_id;
  ad.per_day = 2;

  client_mock->ConfigureWithDataForHourlyFrequencyCappingTest(test_creative_set_id, 0, 3);

  // Act
  bool isAdExcluded = exclusionRule->IsExcluded(ad);
  // Assert
  EXPECT_TRUE(isAdExcluded);
}

  // Tests
  // None = allowed
  // 1 in last day (0+s ago) with cap of 2 = allowed
  // 1 in last day (day-1s ago) with cap of 2 = allowed
  // 2 in last day (0+s ago) with cap of 2 = excluded
  // 1 over a day (day+1s ago) with cap of 2 = allowed

} // namespace ads