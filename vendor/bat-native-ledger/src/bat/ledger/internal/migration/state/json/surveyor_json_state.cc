/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/surveyor_json_state.h"

namespace brave_rewards {

const char kSignatureKey[] = "signature";
const char kSurveyorIdKey[] = "surveyorId";
const char kSurveyVkKey[] = "surveyVK";
const char kRegistrarVkKey[] = "registrarVK";

SurveyorJsonState::SurveyorJsonState() = default;

SurveyorJsonState::~SurveyorJsonState() = default;

ledger::SurveyorPtr SurveyorJsonState::FromJson(
    const std::string& json) {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* signature = choices_dictionary.FindStringKey(kSignatureKey);
  if (!signature) {
    return nullptr;
  }

  auto* surveyor_id = choices_dictionary.FindStringKey(kSurveyorIdKey);
  if (!surveyor_id) {
    return nullptr;
  }

  auto* survey_vk = choices_dictionary.FindStringKey(kSurveyVkKey);
  if (!survey_vk) {
    return nullptr;
  }

  auto* registrar_vk = choices_dictionary.FindStringKey(kRegistrarVkKey);
  if (!registrar_vk) {
    return nullptr;
  }

  auto state = ledger::WalletProperties::New();

  state->signature = *signature;
  state->surveyor_id = *surveyor_id;
  state->survey_vk = *survey_vk;
  state->registrar_vk = *registrar_vk;

  return state;
}

}  // namespace brave_rewards
