/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/migration/state/json/wallet_info_json_state.h"

namespace brave_rewards {

const char kPaymentIdKey[] = "paymentId";
const char kAddressBatKey[] = "addressBAT";
const char kAddressBtcKey[] = "addressBTC";
const char kAddressCardIdKey[] = "addressCARD_ID";
const char kAddressEthKey[] = "addressETH";
const char kAddressLtcKey[] = "addressLTC";
const char kKeyInfoSeedKey[] = "keyInfoSeed";

WalletInfoJsonState::WalletInfoJsonState() = default;

WalletInfoJsonState::~WalletInfoJsonState() = default;

ledger::WalletPtr WalletInfoJsonState::FromJson(
    const std::string& json) const {
  auto* dictionary = GetAsDictionary(json);
  if (!dictionary) {
    return nullptr;
  }

  auto* payment_id = dictionary->FindStringKey(kPaymentIdKey);
  if (!payment_id) {
    return nullptr;
  }

  auto* key_info_seed = dictionary->FindStringKey(kKeyInfoSeedKey);
  if (!key_info_seed) {
    return nullptr;
  }

  auto state = ledger::Wallet::New();

  state->id = *payment_id;
  state->private_key = *key_info_seed;
  wallet->cards = GetCardsFromDictionary(*dictionary);

  return state;
}

//////////////////////////////////////////////////////////////////////////////

std::map<std::string, std::string> WalletInfoJsonState::GetCardsFromDictionary(
    const base::DictionaryValue& dictionary) const {
  const std::map<std::string, std::string> supported_cards = {
    {"BAT", kAddressBatKey},
    {"BTC", kAddressBtcKey},
    {"CardId", kAddressCardIdKey},
    {"ETH", kAddressEthKey},
    {"LTC", kAddressLtcKey}
  };

  std::map<std::string, std::string> cards;

  for (const auto& card : supported_cards) {
    auto card_type = card.first;

    auto* card_number = dictionary.FindStringKey(card.second);
    if (!card_number) {
      continue;
    }

    cards.insert({card_type, *card_number});
  }

  return cards;
}

}  // namespace brave_rewards
