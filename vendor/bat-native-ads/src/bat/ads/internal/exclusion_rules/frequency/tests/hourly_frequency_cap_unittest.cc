/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>

#include "testing/gtest/include/gtest/gtest.h"

#include "bat/ads/internal/exclusion_rules/exclusion_rule.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/internal/exclusion_rules/frequency/hourly_frequency_cap.h"

#include "bat/ads/internal/client_mock.h"
#include "bat/ads/internal/ads_client_mock.h"
#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/ad_info.h"

// npm run test -- brave_unit_tests --filter=Ads*

using std::placeholders::_1;
using ::testing::_;
using ::testing::Invoke;

namespace ads {

static const char* test_ad_uuid = "9aea9a47-c6a0-4718-a0fa-706338bb2156";

class AdsHourlyFrequencyCapTest : public ::testing::Test {
 protected:
  std::unique_ptr<MockAdsClient> mock_ads_client_;
  std::unique_ptr<AdsImpl> ads_;

  AdsHourlyFrequencyCapTest() :
      mock_ads_client_(std::make_unique<MockAdsClient>()),
      ads_(std::make_unique<AdsImpl>(mock_ads_client_.get())) {
    // You can do set-up work for each test here
  }

  ~AdsHourlyFrequencyCapTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)

    auto callback = std::bind(&AdsHourlyFrequencyCapTest::OnAdsImpleInitialize,
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

TEST_F(AdsHourlyFrequencyCapTest, TestAdAllowed) {
  // Arrange
  ClientMock* client_mock = new ClientMock(ads_.get(),
      mock_ads_client_.get());

  FrequencyCapping frequency_capping(client_mock, mock_ads_client_.get());

  ExclusionRule* exclusionRule = new
      HourlyFrequencyCap(frequency_capping);

  AdInfo ad;
  ad.uuid = test_ad_uuid;

  // Act
  bool isAdExcluded = exclusionRule->IsExcluded(ad);
  // Assert
  EXPECT_FALSE(isAdExcluded);

  // Arrange
  // 1hr 1s in the past
  client_mock->ConfigureWithDataForAddHistory(test_ad_uuid, -((60*60) + 1), 1); // Act
  isAdExcluded = exclusionRule->IsExcluded(ad);
  // Assert
  EXPECT_FALSE(isAdExcluded);
}

TEST_F(AdsHourlyFrequencyCapTest, TestAdExcluded) {
  // Arrange
  ClientMock* client_mock = new ClientMock(ads_.get(),
      mock_ads_client_.get());

  FrequencyCapping frequency_capping(client_mock, mock_ads_client_.get());

  ExclusionRule* exclusionRule = new
      HourlyFrequencyCap(frequency_capping);

  AdInfo ad;
  ad.uuid = test_ad_uuid;

  // 59m 59s
  client_mock->ConfigureWithDataForAddHistory(test_ad_uuid, (-59*60), 1); 

  // Act
  bool isAdExcluded = exclusionRule->IsExcluded(ad);
  // Assert
  EXPECT_TRUE(isAdExcluded);

  // Arrange
  client_mock->ConfigureWithDataForAddHistory(test_ad_uuid, 0, 1); 
  // Act
  isAdExcluded = exclusionRule->IsExcluded(ad);
  // Assert
  EXPECT_TRUE(isAdExcluded);


  // Tests
  // None = allowed
  // 1 in last hour (0s ago) = excluded
  // 2 in last hour (>= 0s ago) = excluded
  // 1 in last hour (3599s ago) = excluded
  // 1 3601s ago = allowed
}

} // namespace ads