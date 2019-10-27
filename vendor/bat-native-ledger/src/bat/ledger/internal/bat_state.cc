/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <algorithm>
#include <utility>

#include "bat/ledger/internal/bat_state.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/rapidjson_bat_helper.h"

namespace braveledger_bat_state {

BatState::BatState(bat_ledger::LedgerImpl* ledger) :
      ledger_(ledger),
      state_(new braveledger_bat_helper::CLIENT_STATE_ST()) {
}

BatState::~BatState() {
}

bool BatState::LoadState(const std::string& data) {
  braveledger_bat_helper::CLIENT_STATE_ST state;
  if (!braveledger_bat_helper::loadFromJson(&state, data.c_str())) {
    BLOG(ledger_, ledger::LogLevel::LOG_ERROR) <<
      "Failed to load client state: " << data;
    return false;
  }

  state_.reset(new braveledger_bat_helper::CLIENT_STATE_ST(state));

  bool stateChanged = false;

  // fix timestamp ms to s conversion
  if (std::to_string(state_->reconcile_stamp).length() > 10) {
    state_->reconcile_stamp = state_->reconcile_stamp / 1000;
    stateChanged = true;
  }

  // fix timestamp ms to s conversion
  if (std::to_string(state_->boot_stamp).length() > 10) {
    state_->boot_stamp = state_->boot_stamp / 1000;
    stateChanged = true;
  }

  if (stateChanged) {
    SaveState();
  }

  return true;
}

void BatState::SaveState() {
  std::string data;
  braveledger_bat_helper::saveToJsonString(*state_, &data);
  ledger_->SaveLedgerState(data);
}

void BatState::AddReconcile(const std::string& viewing_id,
      const braveledger_bat_helper::CURRENT_RECONCILE& reconcile) {
  state_->current_reconciles.insert(std::make_pair(viewing_id, reconcile));
  SaveState();
}

bool BatState::UpdateReconcile(
    const braveledger_bat_helper::CURRENT_RECONCILE& reconcile) {
  if (state_->current_reconciles.count(reconcile.viewing_id) == 0) {
    return false;
  }

  state_->current_reconciles[reconcile.viewing_id] = reconcile;
  SaveState();
  return true;
}

braveledger_bat_helper::CURRENT_RECONCILE BatState::GetReconcileById(
    const std::string& viewingId) const {
  if (state_->current_reconciles.count(viewingId) == 0) {
    BLOG(ledger_, ledger::LogLevel::LOG_ERROR) <<
      "Could not find any reconcile tasks with the id " << viewingId;
    return braveledger_bat_helper::CURRENT_RECONCILE();
  }

  return state_->current_reconciles[viewingId];
}

bool BatState::ReconcileExists(const std::string& viewingId) const {
  return state_->current_reconciles.count(viewingId) > 0;
}

void BatState::RemoveReconcileById(const std::string& viewingId) {
  braveledger_bat_helper::CurrentReconciles::iterator it =
      state_->current_reconciles.find(viewingId);
  if (it != state_->current_reconciles.end()) {
    state_->current_reconciles.erase(it);
    SaveState();
  }
}

void BatState::SetRewardsMainEnabled(bool enabled) {
  state_->rewards_enabled = enabled;
  SaveState();
}

bool BatState::GetRewardsMainEnabled() const {
  return state_->rewards_enabled;
}

void BatState::SetContributionAmount(double amount) {
  braveledger_bat_helper::WALLET_PROPERTIES_ST properties =
      GetWalletProperties();
  auto hasAmount = std::find(properties.parameters_choices.begin(),
                             properties.parameters_choices.end(),
                             amount);

  if (hasAmount == properties.parameters_choices.end()) {
    // amount is missing in the list
    properties.parameters_choices.push_back(amount);
    std::sort(properties.parameters_choices.begin(),
              properties.parameters_choices.end());
    ledger_->OnWalletProperties(ledger::Result::LEDGER_OK, properties);
    state_->wallet_properties = properties;
  }

  state_->fee_amount = amount;
  SaveState();
}

double BatState::GetContributionAmount() const {
  return state_->fee_amount;
}

void BatState::SetUserChangedContribution() {
  state_->user_changed_fee = true;
  SaveState();
}

bool BatState::GetUserChangedContribution() const {
  return state_->user_changed_fee;
}

void BatState::SetAutoContribute(bool enabled) {
  state_->auto_contribute = enabled;
  SaveState();
}

bool BatState::GetAutoContribute() const {
  return state_->auto_contribute;
}

const std::string& BatState::GetCardIdAddress() const {
  return state_->wallet_info.address_card_id;
}

uint64_t BatState::GetReconcileStamp() const {
  return state_->reconcile_stamp;
}

void BatState::ResetReconcileStamp() {
  if (ledger::reconcile_time > 0) {
    state_->reconcile_stamp = braveledger_bat_helper::currentTime() +
                                ledger::reconcile_time * 60;
  } else {
    state_->reconcile_stamp = braveledger_bat_helper::currentTime() +
                                braveledger_ledger::_reconcile_default_interval;
  }
  SaveState();
}

uint64_t BatState::GetLastGrantLoadTimestamp() const {
  return state_->last_grant_fetch_stamp;
}

void BatState::SetLastGrantLoadTimestamp(uint64_t stamp) {
  state_->last_grant_fetch_stamp = stamp;
  SaveState();
}

bool BatState::IsWalletCreated() const {
  return state_->boot_stamp != 0u;
}

const std::string& BatState::GetPaymentId() const {
  return state_->wallet_info.payment_id;
}

const braveledger_bat_helper::Grants& BatState::GetGrants() const {
  return state_->grants;
}

void BatState::SetGrants(braveledger_bat_helper::Grants grants) {
  state_->grants = grants;
  SaveState();
}

const std::string& BatState::GetPersonaId() const {
  return state_->persona_id;
}

void BatState::SetPersonaId(const std::string& persona_id) {
  state_->persona_id = persona_id;
  SaveState();
}

const std::string& BatState::GetUserId() const {
  return state_->user_id;
}

void BatState::SetUserId(const std::string& user_id) {
  state_->user_id = user_id;
  SaveState();
}

const std::string& BatState::GetRegistrarVK() const {
  return state_->registrar_vk;
}

void BatState::SetRegistrarVK(const std::string& registrar_vk) {
  state_->registrar_vk = registrar_vk;
  SaveState();
}

const std::string& BatState::GetPreFlight() const {
  return state_->pre_flight;
}

void BatState::SetPreFlight(const std::string& pre_flight) {
  state_->pre_flight = pre_flight;
  SaveState();
}

const braveledger_bat_helper::WALLET_INFO_ST& BatState::GetWalletInfo() const {
  return state_->wallet_info;
}

void BatState::SetWalletInfo(
    const braveledger_bat_helper::WALLET_INFO_ST& wallet_info) {
  state_->wallet_info = wallet_info;
  SaveState();
}

const braveledger_bat_helper::WALLET_PROPERTIES_ST&
BatState::GetWalletProperties() const {
  return state_->wallet_properties;
}

void BatState::SetWalletProperties(
    braveledger_bat_helper::WALLET_PROPERTIES_ST* properties) {
  double amount = GetContributionAmount();
  double new_amount = properties->fee_amount;
  bool amount_changed = GetUserChangedContribution();
  if (amount_changed) {
    auto hasAmount = std::find(properties->parameters_choices.begin(),
                               properties->parameters_choices.end(),
                               amount);

    if (hasAmount == properties->parameters_choices.end()) {
      // amount is missing in the list
      properties->parameters_choices.push_back(amount);
      std::sort(properties->parameters_choices.begin(),
                properties->parameters_choices.end());
    }
  }

  state_->wallet_properties = *properties;

  if (!amount_changed && amount != new_amount) {
    SetContributionAmount(new_amount);
  }

  SaveState();
}

unsigned int BatState::GetDays() const {
  return state_->days;
}

void BatState::SetDays(unsigned int days) {
  state_->days = days;
  SaveState();
}

const braveledger_bat_helper::Transactions& BatState::GetTransactions() const {
  return state_->transactions;
}

void BatState::SetTransactions(
    const braveledger_bat_helper::Transactions& transactions) {
  state_->transactions = transactions;
  SaveState();
}

const braveledger_bat_helper::Ballots& BatState::GetBallots() const {
  return state_->ballots;
}

void BatState::SetBallots(const braveledger_bat_helper::Ballots& ballots) {
  state_->ballots = ballots;
  SaveState();
}

const braveledger_bat_helper::BatchVotes& BatState::GetBatch() const {
  return state_->batch;
}

void BatState::SetBatch(const braveledger_bat_helper::BatchVotes& votes) {
  state_->batch = votes;
  SaveState();
}

const std::string& BatState::GetCurrency() const {
  return state_->fee_currency;
}

void BatState::SetCurrency(const std::string &currency) {
  state_->fee_currency = currency;
  SaveState();
}

uint64_t BatState::GetBootStamp() const {
  return state_->boot_stamp;
}

void BatState::SetBootStamp(uint64_t stamp) {
  state_->boot_stamp = stamp;
  SaveState();
}

const std::string& BatState::GetMasterUserToken() const {
  return state_->master_user_token;
}

void BatState::SetMasterUserToken(const std::string &token) {
  state_->master_user_token = token;
  SaveState();
}

bool BatState::AddReconcileStep(const std::string& viewing_id,
                                ledger::ContributionRetry step,
                                int level) {
  braveledger_bat_helper::CURRENT_RECONCILE reconcile =
      GetReconcileById(viewing_id);

  if (reconcile.viewing_id.empty()) {
    return false;
  }

  // don't save step when you are already in the same step
  if (reconcile.retry_step == step && level == -1) {
    return true;
  }

  reconcile.retry_step = step;
  reconcile.retry_level = level;

  return UpdateReconcile(reconcile);
}

const braveledger_bat_helper::CurrentReconciles&
BatState::GetCurrentReconciles() const {
  return state_->current_reconciles;
}

double BatState::GetDefaultContributionAmount() {
  return state_->wallet_properties.fee_amount;
}

void BatState::SetInlineTipSetting(const std::string& key, bool enabled) {
  state_->inline_tip[key] = enabled;
  SaveState();
}

bool BatState::GetInlineTipSetting(const std::string& key) const {
  if (state_->inline_tip.find(key) == state_->inline_tip.end()) {
    // not found, all tips are on by default
    return true;
  } else {
    return state_->inline_tip[key];
  }
}

}  // namespace braveledger_bat_state
