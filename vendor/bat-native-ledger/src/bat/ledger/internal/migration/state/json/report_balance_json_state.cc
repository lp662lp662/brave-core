/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/report_balance_json_state.h"

namespace brave_rewards {

const char kOpeningBalanceKey[] = "opening_balance";
const char kClosingBalanceKey[] = "closing_balance";
const char kDeposits[] = "deposits";
const char kGrantsKey[] = "grants";
const char kEarningFromAdsKey[] = "earning_from_ads";
const char kAutoContributeKey[] = "auto_contribute";
const char kRecurringDonationKey[] = "recurring_donation";
const char kOneTimeDonationKey[] = "one_time_donation";
const char kTotalKey[] = "total";

ReportBalanceJsonState::ReportBalanceJsonState() = default;

ReportBalanceJsonState::~ReportBalanceJsonState() = default;

ledger::ReportBalancePtr ReportBalanceJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* opening_balance = dictionary.FindStringKey(kOpeningBalanceKey);
  if (!opening_balance) {
    return nullptr;
  }

  auto* closing_balance = dictionary.FindStringKey(kClosingBalanceKey);
  if (!closing_balance) {
    return nullptr;
  }

  auto* deposits = dictionary.FindStringKey(kDeposits);
  if (!deposits) {
    return nullptr;
  }

  auto* grants = dictionary.FindStringKey(kGrantsKey);
  if (!grants) {
    return nullptr;
  }

  auto* earning_from_ads = dictionary.FindStringKey(kEarningFromAdsKey);
  if (!earning_from_ads) {
    return nullptr;
  }

  auto* auto_contribute = dictionary.FindStringKey(kAutoContributeKey);
  if (!auto_contribute) {
    return nullptr;
  }

  auto* recurring_donation = dictionary.FindStringKey(kRecurringDonationKey);
  if (!recurring_donation) {
    return nullptr;
  }

  auto* one_time_donation = dictionary.FindStringKey(kOneTimeDonationKey);
  if (!one_time_donation) {
    return nullptr;
  }

  auto* total = dictionary.FindStringKey(kTotalKey);
  if (!total) {
    return nullptr;
  }

  auto state = ledger::ReportBalancePtr::New();

  state->opening_balance = *opening_balance;
  state->closing_balance = *closing_balance;
  state->deposits = *deposits;
  state->grants = *grants;
  state->earning_from_ads = *earning_from_ads;
  state->auto_contribute = *auto_contribute;
  state->recurring_donation = *recurring_donation;
  state->surveyor_id = *one_time_donation;
  state->total = total;

  return state;
}

}  // namespace brave_rewards
