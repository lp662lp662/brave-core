/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_rewards/browser/database/database_contribution_info.h"

#include <string>
#include <utility>

#include "base/bind.h"
#include "base/strings/stringprintf.h"
#include "sql/statement.h"
#include "sql/transaction.h"

namespace brave_rewards {

DatabaseContributionInfo::DatabaseContributionInfo(int current_db_version) :
    DatabaseTable(current_db_version) {
}

DatabaseContributionInfo::~DatabaseContributionInfo() = default;

bool DatabaseContributionInfo::Init(
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

bool DatabaseContributionInfo::CreateTable(
    sql::Database* db) {
  if (db->DoesTableExist(table_name_)) {
    return true;
  }

  const auto query = base::StringPrintf(
      "CREATE TABLE %s "
      "("
      "publisher_id LONGVARCHAR PRIMARY KEY NOT NULL UNIQUE,"
      "probi TEXT \"0\"  NOT NULL,"
      "date INTEGER NOT NULL,"
      "type INTEGER NOT NULL,"
      "month INTEGER NOT NULL,"
      "year INTEGER NOT NULL,"
      ")",
      table_name_);

  auto success = db->Execute(query.c_str());
  if (!success) {
    return false;
  }

  return true;
}

bool DatabaseContributionInfo::CreateIndex(
    sql::Database* db) {
  auto success = this->InsertIndex(db, table_name_, "publisher_id");
  if (!success) {
    return false;
  }

  return true;
}

bool DatabaseContributionInfo::InsertOrUpdate(
    sql::Database* db,
    ledger::ContributionInfoPtr info) {
  if (!info) {
    return false;
  }

  const auto query = base::StringPrintf(
      "INSERT OR REPLACE INTO %s "
      "(id, type, amount, probi, created_date, reconciled_date) "
      "VALUES (?, ?, ?, ?, ?, ?)",
      table_name_);

  sql::Statement statement(db->GetCachedStatement(
      SQL_FROM_HERE, query.c_str()));

  statement.BindString(0, info->id);
  statement.BindInt(1, info->type);
  statement.BindDouble(2, info->amount);
  statement.BindString(3, info->probi);
  statement.BindInt64(4, info->created_date);
  statement.BindInt64(5, info->reconciled_date);

  return statement.Run();
}

void DatabaseContributionInfo::FetchOneTimeTips(
    sql::Database* db,
    ledger::PublisherInfoList* list,
    const ledger::ACTIVITY_MONTH month,
    const int year) {
  const auto query = base::StringPrintf(
      "SELECT pi.publisher_id, pi.name, pi.url, pi.favIcon, "
      "ci.probi, ci.date, spi.status, pi.provider "
      "FROM %s as ci "
      "INNER JOIN publisher_info AS pi ON ci.publisher_id = pi.publisher_id "
      "LEFT JOIN server_publisher_info AS spi "
      "ON spi.publisher_key = pi.publisher_id "
      "AND strftime('%m%Y', ti.date) = ? "
      "AND ci.type = ?",
      table_name_);

  sql::Statement statement(db->GetUniqueStatement(query.c_str()));

  auto date = base::StringPrintf("%02d%04d", month, year);

  statement.BindString(0, date);
  statement.BindInt(1, static_cast<int>(ledger::RewardsType::ONE_TIME_TIP));

  while (statement.Step()) {
    auto publisher = ledger::PublisherInfo::New();

    publisher->id = statement.ColumnString(0);
    publisher->name = statement.ColumnString(1);
    publisher->url = statement.ColumnString(2);
    publisher->favicon_url = statement.ColumnString(3);
    publisher->weight = statement.ColumnDouble(4);
    publisher->reconcile_stamp = statement.ColumnInt64(5);
    publisher->status =
        static_cast<ledger::mojom::PublisherStatus>(statement.ColumnInt64(6));
    publisher->provider = statement.ColumnString(7);

    list->push_back(std::move(publisher));
  }
}

}  // namespace brave_rewards
