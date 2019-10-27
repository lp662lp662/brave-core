/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_BALLOT_JSON_STATE_H_
#define BRAVELEDGER_BALLOT_JSON_STATE_H_

#include "bat/ledger/internal/migration/state/json/json_state.h"
#include "bat/ledger/internal/bat_helper.h"

namespace ledger {

using Ballot = braveledger_bat_helper::BALLOT_ST;

class BallotJsonState : public JsonState<Ballot> {
 public:
  BallotJsonState();
  ~BallotJsonState();

  bool FromJson(
      const std::string& json,
      Ballot* state) const override;

  bool FromDict(
      const base::DictionaryValue* dictionary,
      Ballot* state) const override;
};

}  // namespace ledger

#endif  // BRAVELEDGER_BALLOT_JSON_STATE_H_
