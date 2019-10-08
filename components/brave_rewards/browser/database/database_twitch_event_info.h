/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_REWARDS_BROWSER_DATABASE_DATABASE_TWITCH_EVENT_INFO_H_
#define BRAVE_COMPONENTS_BRAVE_REWARDS_BROWSER_DATABASE_DATABASE_TWITCH_EVENT_INFO_H_

#include <string>

#include "base/containers/flat_map.h"
#include "bat/ledger/twitch_event_info.h"
#include "brave/components/brave_rewards/browser/database/database_table.h"

namespace brave_rewards {

class DatabaseTwitchEventInfo: public DatabaseTable {
 public:
  explicit DatabaseTwitchEventInfo(int current_db_version);
  ~DatabaseTwitchEventInfo() override;

  bool Init(
      sql::Database* db) override;

  bool CreateTable(
      sql::Database* db) override;

  bool CreateIndex(
      sql::Database* db) override;

  bool InsertOrUpdate(
      sql::Database* db,
      ledger::TwitchEventInfoPtr info);

  base::flat_map<std::string, std::string> GetRecord(
      sql::Database* db,
      const std::string& key);

 private:
  const char* table_name_ = "twitch_event_info";
  const int minimum_version_ = 8;
};

}  // namespace brave_rewards

#endif  // BRAVE_COMPONENTS_BRAVE_REWARDS_BROWSER_DATABASE_DATABASE_TWITCH_EVENT_INFO_H_
