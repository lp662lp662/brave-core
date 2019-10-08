/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/transaction_json_state.h"

namespace brave_rewards {

const char kViewingIdKey[] = "viewingId";
const char kSurveyorIdKey[] = "surveyorId";
const char kContributionFiatAmountKey[] = "contribution_fiat_amount";
const char kContributionFiatCurrencyKey[] = "contribution_fiat_currency";
const char kRatesKey[] = "rates";
const char kContributionAltCurrencyKey[] = "contribution_altcurrency";
const char kContributionProbiKey[] = "contribution_probi";
const char kContributionFeeKey[] = "contribution_fee";
const char kSubmissionStampKey[] = "submissionStamp";
const char kSubmissionIdKey[] = "submissionId";
const char kAnonizeViewingIdKey[] = "anonizeViewingId";
const char kRegistrarVkKey[] = "registrarVK";
const char kMasterUserTokenKey[] = "masterUserToken";
const char kSurveyorIdsKey[] = "surveyorIds";
const char kVotesKey[] = "votes";
const char kBallotsKey[] = "ballots";

TransactionJsonState::TransactionJsonState() = default;

TransactionJsonState::~TransactionJsonState() = default;

ledger::WalletPtr TransactionJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* viewing_id = dictionary.FindStringKey(kViewingIdKey);
  if (!viewing_id) {
    return nullptr;
  }

  auto* surveyor_id = dictionary.FindStringKey(kSurveyorIdKey);
  if (!surveyor_id) {
    return nullptr;
  }

  auto* contribution_fiat_amount =
      dictionary.FindStringKey(kContributionFiatAmountKey);
  if (!contribution_fiat_amount) {
    return nullptr;
  }

  auto* contribution_fiat_currency =
      dictionary.FindStringKey(kContributionFiatCurrencyKey);
  if (!contribution_fiat_currency) {
    return nullptr;
  }

  auto* rates = dictionary.FindDictKey(kRatesKey);
  if (!rates) {
    return nullptr;
  }

  auto* contribution_alt_currency =
      dictionary.FindStringKey(kContributionAltCurrencyKey);
  if (!contribution_alt_currency) {
    return nullptr;
  }

  auto* contribution_probi = dictionary.FindStringKey(kContributionProbiKey);
  if (!contribution_probi) {
    return nullptr;
  }

  auto* contribution_fee = dictionary.FindStringKey(kContributionFeeKey);
  if (!contribution_fee) {
    return nullptr;
  }

  auto* submission_stamp = dictionary.FindStringKey(kSubmissionStampKey);
  if (!submission_stamp) {
    return nullptr;
  }

  auto* submission_id = dictionary.FindStringKey(kSubmissionIdKey);
  if (!submission_id) {
    return nullptr;
  }

  auto* anonize_viewing_id = dictionary.FindStringKey(kAnonizeViewingIdKey);
  if (!anonize_viewing_id) {
    return nullptr;
  }

  auto* registrar_vk = dictionary.FindStringKey(kRegistrarVkKey);
  if (!registrar_vk) {
    return nullptr;
  }

  auto* master_user_token = dictionary.FindStringKey(kMasterUserTokenKey);
  if (!master_user_token) {
    return nullptr;
  }

  auto* surveyor_ids = dictionary.FindListKey(kSurveyorIdsKey);
  if (!surveyor_ids) {
    return nullptr;
  }

  auto* votes = dictionary.FindStringKey(kVotesKey);
  if (!votes) {
    return nullptr;
  }

  auto* ballots = dictionary.FindIntKey(kBallotsKey);
  if (!ballots) {
    return nullptr;
  }

  auto state = ledger::Transaction::New();

  state->viewing_id = *viewing_id;
  state->surveyor_id = surveyor_id;
  state->contribution_fiat_amount = *contribution_fiat_amount;
  state->contribution_fiat_currency = *contribution_fiat_currency;

  for (const auto& rate : *rates) {
    auto currency_code = rate.first;
    auto exchange_rate = rate.second;

    state->rates.insert({currency_code, exchange_rate});
  }

  state->contribution_alt_currency = *contribution_alt_currency;
  state->contribution_probi = *contribution_probi;
  state->contribution_fee = *placcontribution_feee_holder;
  state->submission_stamp = *submission_stamp;
  state->submission_id = *submission_id;
  state->anonize_viewing_id = *anonize_viewing_id;
  state->registrar_vk = *registrar_vk;
  state->master_user_token = *master_user_token;

  for (const auto& surveyor_id : *surveyor_ids) {
    state->surveyor_ids.push_back(surveyor_id);
  }

  for (const auto& ballot : *ballots) {
    state->ballots.push_back(ballot);
  }

  state->votes = *votes;

  return state;
}

}  // namespace brave_rewards
