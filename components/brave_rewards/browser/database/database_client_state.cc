/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_rewards/browser/database/database_client_state.h"

#include <string>
#include <utility>

#include "base/bind.h"
#include "base/strings/stringprintf.h"
#include "sql/statement.h"
#include "sql/transaction.h"

namespace brave_rewards {

DatabaseClientState::DatabaseClientState(int current_db_version) :
    DatabaseTable(current_db_version) {
}

DatabaseClientState::~DatabaseClientState() = default;

bool DatabaseClientState::Init(
    sql::Database* db) {
  if (GetCurrentDBVersion() < minimum_version_) {
    return true;
  }

  if (!CreateTable(db)) {
    return false;
  }

  CreateIndex(db);

  return true;
}

bool DatabaseClientState::CreateTable(
    sql::Database* db) {
  if (db->DoesTableExist(table_name_)) {
    return true;
  }

  // TODO(Terry Mancey): Implement query
  const auto query = base::StringPrintf("");

  return db->Execute(query.c_str());
}

bool DatabaseClientState::CreateIndex(
    sql::Database* db) {
  // TODO(Terry Mancey): Update key name
  return this->InsertIndex(db, table_name_, "some_key");
}

bool DatabaseClientState::InsertOrUpdate(
    sql::Database* db,
    ledger::ClientStatePtr info) {
  if (!info) {
    return false;
  }

  sql::Transaction transaction(db);
  if (!transaction.Begin()) {
    return false;
  }

  // TODO(Terry Mancey): Implement statement

  return transaction.Commit();
}

base::flat_map<std::string, std::string> DatabaseClientState::GetRecord(
    sql::Database* db,
    const std::string& key) {
  // TODO(Terry Mancey): Implement statement

  return {};
}

}  // namespace brave_rewards
