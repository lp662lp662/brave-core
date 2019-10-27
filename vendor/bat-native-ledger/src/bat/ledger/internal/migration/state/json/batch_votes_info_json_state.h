/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_BATCH_VOTES_INFO_JSON_STATE_H_
#define BRAVELEDGER_BATCH_VOTES_INFO_JSON_STATE_H_

#include "bat/ledger/internal/migration/state/json/json_state.h"
#include "bat/ledger/internal/bat_helper.h"

namespace ledger {

using BatchVotesInfo = braveledger_bat_helper::BATCH_VOTES_INFO_ST;

class BatchVotesInfoJsonState : public JsonState<BatchVotesInfo> {
 public:
  BatchVotesInfoJsonState();
  ~BatchVotesInfoJsonState();

  bool FromJson(
      const std::string& json,
      BatchVotesInfo* state) const override;

  bool FromDict(
      const base::DictionaryValue* dictionary,
      BatchVotesInfo* state) const override;
};

}  // namespace ledger

#endif  // BRAVELEDGER_BATCH_VOTES_INFO_JSON_STATE_H_
