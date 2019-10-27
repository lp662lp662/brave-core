/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_WALLET_INFO_JSON_STATE_H_
#define BRAVELEDGER_WALLET_INFO_JSON_STATE_H_

#include <string>
#include <map>

#include "bat/ledger/internal/migration/state/json/json_state.h"
#include "bat/ledger/wallet.h"

namespace brave_rewards {

class WalletInfoJsonState : public JsonState<ledger::WalletPtr> {
 public:
  WalletInfoJsonState();
  ~WalletInfoJsonState() override;

  ledger::WalletPtr FromJson(
      const std::string& json) const override;

 private:
  std::map<std::string, std::string> GetCardsFromDictionary(
      const base::DictionaryValue& dictionary) const;
};

}  // namespace brave_rewards

#endif  // BRAVELEDGER_WALLET_INFO_JSON_STATE_H_
