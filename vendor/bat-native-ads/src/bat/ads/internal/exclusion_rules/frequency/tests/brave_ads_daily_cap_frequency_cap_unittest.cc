/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>

#include "testing/gtest/include/gtest/gtest.h"

#include "bat/ads/internal/exclusion_rules/exclusion_rule.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"
#include "bat/ads/internal/exclusion_rules/frequency/daily_cap_frequency_cap.h"

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

const char test_campaign_ids[][40] = {
    "60267cee-d5bb-4a0d-baaf-91cd7f18e07e",
    "90762cee-d5bb-4a0d-baaf-61cd7f18e07e"
};

const uint64_t kSecondsPerDay = base::Time::kSecondsPerHour *
    base::Time::kHoursPerDay;

class BraveAdsDailyCapFrequencyCapTest : public ::testing::Test {
 protected:
  BraveAdsDailyCapFrequencyCapTest() :
      mock_ads_client_(std::make_unique<MockAdsClient>()),
      ads_(std::make_unique<AdsImpl>(mock_ads_client_.get())) {
    // You can do set-up work for each test here
  }

  ~BraveAdsDailyCapFrequencyCapTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)

    auto callback = std::bind(
        &BraveAdsDailyCapFrequencyCapTest::OnAdsImplInitialize, this, _1);
    ads_->Initialize(callback);  // TODO(masparrow): Null callback?

    client_mock_ = std::make_unique<ClientMock>(ads_.get(),
        mock_ads_client_.get());
    frequency_capping_ = std::make_unique<FrequencyCapping>(ads_.get(),
        client_mock_.get());
    exclusion_rule_ = std::make_unique<DailyCapFrequencyCap>
        (*frequency_capping_);
    ad_info_ = std::make_unique<AdInfo>();
  }

  void OnAdsImplInitialize(const Result result) {
    EXPECT_EQ(Result::SUCCESS, result);
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right before the
    // destructor)
  }

  std::unique_ptr<MockAdsClient> mock_ads_client_;
  std::unique_ptr<AdsImpl> ads_;

  std::unique_ptr<ClientMock> client_mock_;
  std::unique_ptr<FrequencyCapping> frequency_capping_;
  std::unique_ptr<DailyCapFrequencyCap> exclusion_rule_;
  std::unique_ptr<AdInfo> ad_info_;
};

TEST_F(BraveAdsDailyCapFrequencyCapTest, AdAllowedWhenNoAds) {
  // Arrange
  ad_info_->campaign_id = test_campaign_ids[0];
  ad_info_->daily_cap = 2;

  // Act
  bool is_ad_excluded = exclusion_rule_->ShouldExclude(*ad_info_);

  // Assert
  EXPECT_FALSE(is_ad_excluded);
}

TEST_F(BraveAdsDailyCapFrequencyCapTest, AdAllowedWithAds) {
  // Arrange
  ad_info_->campaign_id = test_campaign_ids[0];
  ad_info_->daily_cap = 2;

  client_mock_->GenerateCampaignHistory(
      ad_info_->campaign_id, 0, 1);

  // Act
  bool is_ad_excluded = exclusion_rule_->ShouldExclude(*ad_info_);

  // Assert
  EXPECT_FALSE(is_ad_excluded);
}

TEST_F(BraveAdsDailyCapFrequencyCapTest, AdAllowedWithAdsWithinTheDay) {
  // Arrange
  ad_info_->campaign_id = test_campaign_ids[0];
  ad_info_->daily_cap = 2;

  // 23hrs 59m 59s ago
  client_mock_->GenerateCampaignHistory(
      ad_info_->campaign_id, -(kSecondsPerDay - 1), 1);

  // Act
  bool is_ad_excluded = exclusion_rule_->ShouldExclude(*ad_info_);

  // Assert
  EXPECT_FALSE(is_ad_excluded);
}

TEST_F(BraveAdsDailyCapFrequencyCapTest, AdAllowedWithAdsOverTheDay) {
  // Arrange
  ad_info_->campaign_id = test_campaign_ids[0];
  ad_info_->daily_cap = 2;

  // 24hs ago
  client_mock_->GenerateCampaignHistory(
      ad_info_->campaign_id, -kSecondsPerDay, 1);

  // Act
  bool is_ad_excluded = exclusion_rule_->ShouldExclude(*ad_info_);

  // Assert
  EXPECT_FALSE(is_ad_excluded);
}

TEST_F(BraveAdsDailyCapFrequencyCapTest,
  AdExcludedWithMatchingCampaignAds) {
  // Arrange
  ad_info_->campaign_id = test_campaign_ids[0];
  ad_info_->daily_cap = 2;

  client_mock_->GenerateCampaignHistory(
    ad_info_->campaign_id, 0, 2);

  // Act
  bool is_ad_excluded = exclusion_rule_->ShouldExclude(*ad_info_);
  // Assert
  EXPECT_TRUE(is_ad_excluded);
}

TEST_F(BraveAdsDailyCapFrequencyCapTest,
  AdNotExcludedWithNoMatchingCampaignAds) {
  // Arrange
  client_mock_->GenerateCampaignHistory(
    test_campaign_ids[0], 0, 2);

  ad_info_->campaign_id = test_campaign_ids[1];
  ad_info_->daily_cap = 1;

  // Act
  bool is_ad_excluded = exclusion_rule_->ShouldExclude(*ad_info_);
  // Assert
  EXPECT_FALSE(is_ad_excluded);
}

TEST_F(BraveAdsDailyCapFrequencyCapTest, AdExcludedForIssue4207) {
  // Arrange
  ad_info_->campaign_id = test_campaign_ids[0];
  ad_info_->daily_cap = 20;

  // 5 ads per hour, up to a total of 20
  client_mock_->GenerateCampaignHistory(ad_info_->campaign_id,
      -(base::Time::kSecondsPerHour / 5), 20);

  // Act
  bool is_ad_excluded = exclusion_rule_->ShouldExclude(*ad_info_);

  // Assert
  EXPECT_TRUE(is_ad_excluded);
}

}  // namespace ads
