/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_GRANTS_PROPERTIES_JSON_STATE_H_
#define BRAVELEDGER_GRANTS_PROPERTIES_JSON_STATE_H_

#include <string>

#include "bat/ledger/internal/migration/state/json/json_state.h"
#include "bat/ledger/grants_properties.h"

namespace brave_rewards {

class GrantsPropertiesJsonState :
    public JsonState<ledger::GrantsPropertiesPtr> {
 public:
  GrantsPropertiesJsonState();
  ~GrantsPropertiesJsonState() override;

  ledger::GrantsPropertiesPtr FromJson(
      const std::string& json) const override;
};

}  // namespace brave_rewards

#endif  // BRAVELEDGER_GRANTS_PROPERTIES_JSON_STATE_H_
