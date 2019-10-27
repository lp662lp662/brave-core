/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>

#include "testing/gtest/include/gtest/gtest.h"

#include "base/time/time.h"

#include "bat/ads/internal/exclusion_rules/frequency/minimum_wait_time_frequency_cap.h"
#include "bat/ads/internal/exclusion_rules/frequency/frequency_capping.h"

#include "bat/ads/internal/client_mock.h"
#include "bat/ads/internal/ads_client_mock.h"
#include "bat/ads/internal/ads_impl.h"

// npm run test -- brave_unit_tests --filter=Ads*

using std::placeholders::_1;
using ::testing::_;
using ::testing::Invoke;

namespace ads {

class BraveAdsMinimumWaitTimeTest : public ::testing::Test {
 protected:
  BraveAdsMinimumWaitTimeTest()
  : mock_ads_client_(std::make_unique<MockAdsClient>()),
    ads_(std::make_unique<AdsImpl>(mock_ads_client_.get())) {
    // You can do set-up work for each test here
  }

  ~BraveAdsMinimumWaitTimeTest() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)

    auto callback = std::bind(
        &BraveAdsMinimumWaitTimeTest::OnAdsImplInitialize, this, _1);
    ads_->Initialize(callback);  // TODO(masparrow): Null callback?

    client_mock_ = std::make_unique<ClientMock>(ads_.get(),
        mock_ads_client_.get());
    frequency_capping_ = std::make_unique<FrequencyCapping>(
        client_mock_.get());
    minimum_wait_time_ = std::make_unique<MinimumWaitTimeFrequencyCap>(
        ads_.get(), mock_ads_client_.get(), frequency_capping_.get());

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
  std::unique_ptr<MinimumWaitTimeFrequencyCap> minimum_wait_time_;
};

TEST_F(BraveAdsMinimumWaitTimeTest, MinimumWaitTimeRespectedWithNoAdHistory) {
  // Arrange
  ON_CALL(*mock_ads_client_, GetAdsPerHour())
      .WillByDefault(testing::Return(2));

  // Act
  auto does_history_respect_minimum_wait_time =
       minimum_wait_time_->IsAllowed();

  // Assert
  EXPECT_TRUE(does_history_respect_minimum_wait_time);
}

  // ads per hour   ads in last hour  ads in min wait time  IsAllowed
  // 2              0                 0                     true
  // 2              1                 0                     true
  // 2              2                 0                     false
  // 2              0                 1                     false
  // 2              2                 1                     false

}  // namespace ads
