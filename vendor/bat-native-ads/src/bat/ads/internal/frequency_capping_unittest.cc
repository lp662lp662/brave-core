/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testing/gtest/include/gtest/gtest.h"

// npm run test -- brave_unit_tests --filter=Confirmations*

using ::testing::_;
using ::testing::Invoke;

namespace ads {

class FrequencyCappingTest : public ::testing::Test {
 protected:

  FrequencyCapping() {
    // You can do set-up work for each test here
    }

  ~FrequencyCapping() override {
    // You can do clean-up work that doesn't throw exceptions here
  }

  // If the constructor and destructor are not enough for setting up and
  // cleaning up each test, you can use the following methods

  void SetUp() override {
    // Code here will be called immediately after the constructor (right before
    // each test)
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right before the
    // destructor)
  }

  // Objects declared here can be used by all tests in the test case
  base::FilePath GetTestDataPath() {
    return base::FilePath(FILE_PATH_LITERAL(
        "brave/vendor/bat-native-confirmations/test/data"));
  }

  bool Load(const base::FilePath path, std::string* value) {
    if (!value) {
      return false;
    }

    std::ifstream ifs{path.value().c_str()};
    if (ifs.fail()) {
      *value = "";
      return false;
    }

    std::stringstream stream;
    stream << ifs.rdbuf();
    *value = stream.str();
    return true;
  }
};

TEST_F(FrequencyCapping, DoTest) {
  // Arrange

  // Act

  // Assert
}

}  // namespace ads
