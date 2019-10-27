/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_BAT_HELPER_H_
#define BRAVELEDGER_BAT_HELPER_H_

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "bat/ledger/ledger.h"
#include "bat/ledger/internal/static_values.h"

namespace braveledger_bat_helper {
bool isProbiValid(const std::string& number);

struct UNSIGNED_TX {
  UNSIGNED_TX();
  ~UNSIGNED_TX();

  std::string amount;
  std::string currency;
  std::string destination_;
};

struct RECONCILE_PAYLOAD_ST {
  RECONCILE_PAYLOAD_ST();
  ~RECONCILE_PAYLOAD_ST();

  std::string request_type;
  std::string request_signed_tx_headers_digest;
  std::string request_signed_tx_headers_signature;
  UNSIGNED_TX request_signed_tx_body;
  std::string request_signed_tx_octets;
  std::string request_viewing_id;
  std::string request_surveyor_id;
};

struct WALLET_INFO_ST {
  WALLET_INFO_ST();
  WALLET_INFO_ST(const WALLET_INFO_ST&);
  ~WALLET_INFO_ST();

  // load from json string
  bool loadFromJson(const std::string & json);

  std::string payment_id;
  std::string address_bat;
  std::string address_btc;
  std::string address_card_id;
  std::string address_eth;
  std::string address_ltc;
  std::vector<uint8_t> key_info_seed;
};

struct TRANSACTION_BALLOT_ST {
  TRANSACTION_BALLOT_ST();
  ~TRANSACTION_BALLOT_ST();

  // load from json string
  bool loadFromJson(const std::string & json);

  std::string publisher;
  unsigned int offset = 0u;
};

struct TRANSACTION_ST {
  TRANSACTION_ST();
  TRANSACTION_ST(const TRANSACTION_ST& transaction);
  ~TRANSACTION_ST();

  // load from json string
  bool loadFromJson(const std::string & json);

  std::string viewing_id;
  std::string surveyor_id;
  std::map<std::string, double> contribution_rates;
  std::string contribution_probi;
  std::string submission_stamp;
  std::string anonize_viewing_id;
  std::string registrar_vk;
  std::string master_user_token;
  std::vector<std::string> surveyor_ids;
  unsigned int votes = 0u;
  std::vector<TRANSACTION_BALLOT_ST> ballots;
};

struct BALLOT_ST {
  BALLOT_ST();
  BALLOT_ST(const BALLOT_ST& ballot);
  ~BALLOT_ST();

  std::string viewing_id;
  std::string surveyor_id;
  std::string publisher;
  unsigned int offset = 0u;
  std::string prepare_ballot;
  std::string proof_ballot;
  uint64_t delay_stamp = 0u;
};

struct BATCH_VOTES_INFO_ST {
  BATCH_VOTES_INFO_ST();
  BATCH_VOTES_INFO_ST(const BATCH_VOTES_INFO_ST&);
  ~BATCH_VOTES_INFO_ST();

  std::string surveyor_id;
  std::string proof;
};

struct BATCH_VOTES_ST {
  BATCH_VOTES_ST();
  BATCH_VOTES_ST(const BATCH_VOTES_ST&);
  ~BATCH_VOTES_ST();

  std::string publisher;
  std::vector<BATCH_VOTES_INFO_ST> batch_votes_info;
};

struct GRANT {
  GRANT();
  GRANT(const GRANT& properties);
  ~GRANT();

  // load from json string
  bool loadFromJson(const std::string & json);
  std::string alt_currency;
  std::string probi;
  uint64_t expiry_time;
  std::string promotion_id;
  std::string type;
};

struct GRANT_RESPONSE {
  GRANT_RESPONSE();
  GRANT_RESPONSE(const GRANT_RESPONSE& properties);
  ~GRANT_RESPONSE();

  // load from json string
  bool loadFromJson(const std::string & json);

  std::string promotion_id;
  uint64_t minimum_reconcile_timestamp;
  uint64_t protocol_version;
  std::string type;
};

struct WALLET_PROPERTIES_ST {
  WALLET_PROPERTIES_ST();
  WALLET_PROPERTIES_ST(const WALLET_PROPERTIES_ST& properties);
  ~WALLET_PROPERTIES_ST();

  // load from json string
  bool loadFromJson(const std::string & json);

  double fee_amount;
  std::vector<double> parameters_choices;
  std::vector<GRANT> grants;
};

struct REPORT_BALANCE_ST {
  REPORT_BALANCE_ST();
  REPORT_BALANCE_ST(const REPORT_BALANCE_ST&);
  ~REPORT_BALANCE_ST();

  bool loadFromJson(const std::string &json);

  std::string opening_balance = "0";
  std::string closing_balance = "0";
  std::string deposits = "0";
  std::string grants = "0";
  std::string earning_from_ads = "0";
  std::string auto_contribute = "0";
  std::string recurring_donation = "0";
  std::string one_time_donation = "0";
  std::string total = "0";
};

struct PUBLISHER_STATE_ST {
  PUBLISHER_STATE_ST();
  PUBLISHER_STATE_ST(const PUBLISHER_STATE_ST&);
  ~PUBLISHER_STATE_ST();

  // load from json string
  bool loadFromJson(const std::string &json);

  uint64_t min_publisher_duration =
      braveledger_ledger::_default_min_publisher_duration;  // In seconds
  unsigned int min_visits = 1u;
  bool allow_non_verified = true;
  bool allow_videos = true;
  std::map<std::string, REPORT_BALANCE_ST> monthly_balances;
  bool migrate_score_2 = false;
  std::vector<std::string> processed_pending_publishers;
};

struct PUBLISHER_ST {
  PUBLISHER_ST();
  ~PUBLISHER_ST();
  bool operator<(const PUBLISHER_ST& rhs) const;

  // load from json string
  bool loadFromJson(const std::string & json);

  std::string id;
  uint64_t duration = 0u;
  double score = .0;
  unsigned int visits = 0;

  // The mathematically rounded publisher voting weight, as described
  // below.
  unsigned int percent = 0;

  // The exact weight to use when calculating the number of votes to
  // cast for each publisher.
  double weight = .0;

  unsigned int status = 0;
};

struct RECONCILE_DIRECTION {
  RECONCILE_DIRECTION();
  RECONCILE_DIRECTION(const std::string& publisher_key,
                      double amount_percent);
  ~RECONCILE_DIRECTION();

  bool loadFromJson(const std::string &json);

  std::string publisher_key;
  double amount_percent;
};

typedef std::vector<RECONCILE_DIRECTION> Directions;

struct WINNERS_ST {
  WINNERS_ST();
  ~WINNERS_ST();

  RECONCILE_DIRECTION direction;
  unsigned int votes = 0;
};

typedef std::vector<braveledger_bat_helper::WINNERS_ST> Winners;

struct SURVEYOR_INFO_ST {
  SURVEYOR_INFO_ST();
  ~SURVEYOR_INFO_ST();

  std::string surveyor_id;
};

struct SURVEYOR_ST {
  SURVEYOR_ST();
  ~SURVEYOR_ST();

  // load from json string
  bool loadFromJson(const std::string & json);

  std::string signature_;
  std::string surveyor_id;
  std::string survey_vk;
  std::string registrar_vk;
  std::string survey_sk;
};


typedef std::vector<PUBLISHER_ST> PublisherList;

struct CURRENT_RECONCILE {
  CURRENT_RECONCILE();
  CURRENT_RECONCILE(const CURRENT_RECONCILE&);
  ~CURRENT_RECONCILE();

  // load from json string
  bool loadFromJson(const std::string & json);

  std::string viewing_id;
  std::string anonize_viewing_id;
  std::string registrar_vk;
  std::string pre_flight;
  std::string master_user_token;
  SURVEYOR_INFO_ST surveyor_info;
  uint64_t timestamp = 0u;
  std::map<std::string, double> rates;
  std::string amount;
  std::string currency;
  double fee;
  Directions directions;
  ledger::RewardsType type;
  ledger::ContributionRetry retry_step;
  int retry_level;
  std::string destination;
  std::string proof;
};

typedef std::vector<GRANT> Grants;
typedef std::vector<TRANSACTION_ST> Transactions;
typedef std::vector<BALLOT_ST> Ballots;
typedef std::vector<BATCH_VOTES_ST> BatchVotes;
typedef std::map<std::string, CURRENT_RECONCILE> CurrentReconciles;

struct CLIENT_STATE_ST {
  CLIENT_STATE_ST();
  CLIENT_STATE_ST(const CLIENT_STATE_ST&);
  ~CLIENT_STATE_ST();

  // Load from json string
  bool loadFromJson(const std::string & json);

  WALLET_INFO_ST wallet_info;
  WALLET_PROPERTIES_ST wallet_properties;
  uint64_t boot_stamp = 0u;
  uint64_t reconcile_stamp = 0u;
  uint64_t last_grant_fetch_stamp = 0u;
  std::string persona_id;
  std::string user_id;
  std::string registrar_vk;
  std::string master_user_token;
  std::string pre_flight;
  std::string fee_currency;
  std::string settings = AD_FREE_SETTINGS;
  double fee_amount = .0;
  bool user_changed_fee = false;
  unsigned int days = 0u;
  Transactions transactions;
  Ballots ballots;
  std::string rule_set;
  std::string rule_set_v2;
  BatchVotes batch;
  Grants grants;
  CurrentReconciles current_reconciles;
  bool auto_contribute = false;
  bool rewards_enabled = false;
  std::map<std::string, bool> inline_tip;
};

struct GRANTS_PROPERTIES_ST {
  GRANTS_PROPERTIES_ST();
  GRANTS_PROPERTIES_ST(const GRANTS_PROPERTIES_ST& properties);
  ~GRANTS_PROPERTIES_ST();

  //  load from json string
  bool loadFromJson(const std::string & json);

  std::vector<GRANT_RESPONSE> grants;
};

struct BATCH_PROOF {
  BATCH_PROOF();
  ~BATCH_PROOF();

  TRANSACTION_ST transaction;
  BALLOT_ST ballot;
};

typedef std::vector<braveledger_bat_helper::BATCH_PROOF> BatchProofs;

enum class SERVER_TYPES {
  LEDGER,
  BALANCE,
  PUBLISHER,
  PUBLISHER_DISTRO
};

using SaveVisitSignature = void(const std::string&, uint64_t);
using SaveVisitCallback = std::function<SaveVisitSignature>;

bool getJSONValue(const std::string& fieldName,
                  const std::string& json,
                  std::string* value);

bool getJSONList(const std::string& fieldName,
                 const std::string& json,
                 std::vector<std::string>* value);

bool getJSONWalletInfo(const std::string& json,
                       WALLET_INFO_ST* walletInfo,
                       std::string* fee_currency,
                       double* fee_amount,
                       unsigned int* days);

bool getJSONUnsignedTx(const std::string& json, UNSIGNED_TX* unsignedTx);

bool getJSONTransaction(const std::string& json, TRANSACTION_ST* transaction);

bool getJSONRates(const std::string& json,
                  std::map<std::string, double>* rates);

bool getJSONTwitchProperties(
    const std::string& json,
    std::vector<std::map<std::string, std::string>>* parts);

bool getJSONBatchSurveyors(const std::string& json,
                           std::vector<std::string>* surveyors);

bool getJSONRecoverWallet(const std::string& json,
                          double* balance,
                          std::vector<GRANT>* grants);

bool getJSONResponse(const std::string& json,
                     unsigned int* statusCode,
                     std::string* error);

bool getJSONAddresses(const std::string& json,
                      std::map<std::string, std::string>* addresses);

bool getJSONMessage(const std::string& json,
                     std::string* message);

std::vector<uint8_t> generateSeed();

std::vector<uint8_t> getHKDF(const std::vector<uint8_t>& seed);

bool getPublicKeyFromSeed(const std::vector<uint8_t>& seed,
                          std::vector<uint8_t>* publicKey,
                          std::vector<uint8_t>* secretKey);

std::string uint8ToHex(const std::vector<uint8_t>& in);

std::string stringify(std::string* keys,
                      std::string* values,
                      const unsigned int size);

std::string stringifyReconcilePayloadSt(
    const RECONCILE_PAYLOAD_ST& reconcile_payload);

std::string stringifyUnsignedTx(const UNSIGNED_TX& unsignedTx);

std::string stringifyBatch(std::vector<BATCH_VOTES_INFO_ST> payload);

std::vector<uint8_t> getSHA256(const std::string& in);

std::string getBase64(const std::vector<uint8_t>& in);

bool getFromBase64(const std::string& in, std::vector<uint8_t>* out);

// Sign using ed25519 algorithm
std::string sign(std::string* keys,
                 std::string* values,
                 const unsigned int size,
                 const std::string& keyId,
                 const std::vector<uint8_t>& secretKey);

uint64_t currentTime();

std::string buildURL(const std::string& path,
                     const std::string& prefix = "",
                     const SERVER_TYPES& server = SERVER_TYPES::LEDGER);

std::vector<std::string> split(const std::string& s, char delim);

bool ignore_for_testing();

void set_ignore_for_testing(bool ignore);

uint8_t niceware_mnemonic_to_bytes(
    const std::string& w,
    std::vector<uint8_t>* bytes_out,
    size_t* written,
    std::vector<std::string> wordDictionary);

bool HasSameDomainAndPath(
    const std::string& url,
    const std::string& to_match,
    const std::string& path);

}  // namespace braveledger_bat_helper

#endif  // BRAVELEDGER_BAT_HELPER_H_
