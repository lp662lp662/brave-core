/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_TRANSACTION_BALLOT_JSON_STATE_H_
#define BRAVELEDGER_TRANSACTION_BALLOT_JSON_STATE_H_

#include <string>

#include "bat/ledger/internal/migration/state/json/json_state.h"
#include "bat/ledger/transaction_ballot.h"

namespace brave_rewards {

class TransactionBallotJsonState :
    public JsonState<ledger::TransactionBallotPtr> {
 public:
  TransactionBallotJsonState();
  ~TransactionBallotJsonState() override;

  ledger::TransactionBallotPtr FromJson(
      const std::string& json) const override;
};

}  // namespace brave_rewards

#endif  // BRAVELEDGER_TRANSACTION_BALLOT_JSON_STATE_H_
