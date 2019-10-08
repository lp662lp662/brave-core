/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_MIGRATE_JSON_STATE_H_
#define BRAVELEDGER_MIGRATE_JSON_STATE_H_

#include <string>

namespace brave_rewards {

class MigrateJsonState {
 public:
  MigrateJsonState();
  ~MigrateJsonState();

  bool Migrate(
    const std::string& json);
};

}  // namespace brave_rewards

#endif  // BRAVELEDGER_MIGRATE_JSON_STATE_H_
