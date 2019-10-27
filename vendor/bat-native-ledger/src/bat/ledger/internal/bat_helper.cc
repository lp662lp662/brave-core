/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <openssl/base64.h>
#include <openssl/digest.h>
#include <openssl/hkdf.h>
#include <openssl/sha.h>
#include <iomanip>
#include <random>
#include <algorithm>
#include <utility>

#include "bat/ledger/internal/bat_helper.h"
#include "bat/ledger/internal/logging.h"
#include "bat/ledger/internal/rapidjson_bat_helper.h"
#include "bat/ledger/internal/static_values.h"
#include "bat/ledger/internal/migration/state/json/ballot_json_state.h"
#include "bat/ledger/internal/migration/state/json/batch_votes_info_json_state.h"
#include "bat/ledger/internal/migration/state/json/batch_votes_json_state.h"
#include "bat/ledger/ledger.h"
#include "third_party/re2/src/re2/re2.h"
#include "tweetnacl.h"  // NOLINT
#include "url/gurl.h"

namespace braveledger_bat_helper {

bool isProbiValid(const std::string& probi) {
  // probi shouldn't be longer then 44
  if (probi.length() > 44) {
    return false;
  }

  // checks if probi only contains numbers
  return re2::RE2::FullMatch(probi, "^-?[0-9]*$");
}

/////////////////////////////////////////////////////////////////////////////
RECONCILE_PAYLOAD_ST::RECONCILE_PAYLOAD_ST() {}

RECONCILE_PAYLOAD_ST::~RECONCILE_PAYLOAD_ST() {}

/////////////////////////////////////////////////////////////////////////////
WALLET_INFO_ST::WALLET_INFO_ST() {}

WALLET_INFO_ST::WALLET_INFO_ST(const WALLET_INFO_ST& other) {
  payment_id = other.payment_id;
  address_bat = other.address_bat;
  address_btc = other.address_btc;
  address_card_id = other.address_card_id;
  address_eth = other.address_eth;
  address_ltc = other.address_ltc;
  key_info_seed = other.key_info_seed;
}

WALLET_INFO_ST::~WALLET_INFO_ST() {}


bool WALLET_INFO_ST::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("paymentId") && d["paymentId"].IsString() &&
      d.HasMember("addressBAT") && d["addressBAT"].IsString() &&
      d.HasMember("addressBTC") && d["addressBTC"].IsString() &&
      d.HasMember("addressCARD_ID") && d["addressCARD_ID"].IsString() &&
      d.HasMember("addressETH") && d["addressETH"].IsString() &&
      d.HasMember("addressLTC") && d["addressLTC"].IsString() &&
      d.HasMember("keyInfoSeed") && d["keyInfoSeed"].IsString() );
  }

  if (!error) {
    // convert keyInfoSeed and check error
    std::string sKeyInfoSeed = d["keyInfoSeed"].GetString();
    error = !getFromBase64(sKeyInfoSeed, &key_info_seed);
  }

  if (!error) {
    payment_id = d["paymentId"].GetString();
    address_bat = d["addressBAT"].GetString();
    address_btc = d["addressBTC"].GetString();
    address_card_id = d["addressCARD_ID"].GetString();
    address_eth = d["addressETH"].GetString();
    address_ltc = d["addressLTC"].GetString();
  }
  return !error;
}

void saveToJson(JsonWriter* writer, const WALLET_INFO_ST& data) {
  writer->StartObject();

  writer->String("paymentId");
  writer->String(data.payment_id.c_str());

  writer->String("addressBAT");
  writer->String(data.address_bat.c_str());

  writer->String("addressBTC");
  writer->String(data.address_btc.c_str());

  writer->String("addressCARD_ID");
  writer->String(data.address_card_id.c_str());

  writer->String("addressETH");
  writer->String(data.address_eth.c_str());

  writer->String("addressLTC");
  writer->String(data.address_ltc.c_str());

  writer->String("keyInfoSeed");
  if (data.key_info_seed.size() == 0) {
    writer->String("");
  } else {
    writer->String(getBase64(data.key_info_seed).c_str());
  }

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
UNSIGNED_TX::UNSIGNED_TX() {}

UNSIGNED_TX::~UNSIGNED_TX() {}

/////////////////////////////////////////////////////////////////////////////
TRANSACTION_BALLOT_ST::TRANSACTION_BALLOT_ST() :
  offset(0) {}

TRANSACTION_BALLOT_ST::~TRANSACTION_BALLOT_ST() {}

bool TRANSACTION_BALLOT_ST::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("publisher") && d["publisher"].IsString() &&
      d.HasMember("offset") && d["offset"].IsUint() );
  }

  if (!error) {
    publisher = d["publisher"].GetString();
    offset = d["offset"].GetUint();
  }

  return !error;
}

void saveToJson(JsonWriter* writer, const TRANSACTION_BALLOT_ST& data) {
  writer->StartObject();

  writer->String("publisher");
  writer->String(data.publisher.c_str());

  writer->String("offset");
  writer->Uint(data.offset);

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
TRANSACTION_ST::TRANSACTION_ST():
  votes(0) {}

TRANSACTION_ST::TRANSACTION_ST(const TRANSACTION_ST& transaction) {
  viewing_id = transaction.viewing_id;
  surveyor_id = transaction.surveyor_id;
  contribution_rates = transaction.contribution_rates;
  contribution_probi = transaction.contribution_probi;
  submission_stamp = transaction.submission_stamp;
  contribution_rates = transaction.contribution_rates;
  anonize_viewing_id = transaction.anonize_viewing_id;
  registrar_vk = transaction.registrar_vk;
  master_user_token = transaction.master_user_token;
  surveyor_ids = transaction.surveyor_ids;
  votes = transaction.votes;
  ballots = transaction.ballots;
}

TRANSACTION_ST::~TRANSACTION_ST() {}

bool TRANSACTION_ST::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("viewingId") && d["viewingId"].IsString() &&
      d.HasMember("surveyorId") && d["surveyorId"].IsString() &&
      d.HasMember("contribution_fiat_amount") &&
      d["contribution_fiat_amount"].IsString() &&
      d.HasMember("contribution_fiat_currency") &&
      d["contribution_fiat_currency"].IsString() &&
      d.HasMember("rates") && d["rates"].IsObject() &&
      d["rates"].HasMember("ETH") &&
      d["rates"].HasMember("LTC") &&
      d["rates"].HasMember("BTC") &&
      d["rates"].HasMember("USD") &&
      d["rates"].HasMember("EUR") &&
      d.HasMember("contribution_altcurrency") &&
      d["contribution_altcurrency"].IsString() &&
      d.HasMember("contribution_probi") && d["contribution_probi"].IsString() &&
      d.HasMember("contribution_fee") && d["contribution_fee"].IsString() &&
      d.HasMember("submissionStamp") && d["submissionStamp"].IsString() &&
      d.HasMember("submissionId") && d["submissionId"].IsString() &&
      d.HasMember("anonizeViewingId") && d["anonizeViewingId"].IsString() &&
      d.HasMember("registrarVK") && d["registrarVK"].IsString() &&
      d.HasMember("masterUserToken") && d["masterUserToken"].IsString() &&
      d.HasMember("surveyorIds") && d["surveyorIds"].IsArray() &&
      d.HasMember("votes") && d["votes"].IsUint() &&
      d.HasMember("ballots") && d["ballots"].IsArray());
  }

  if (!error) {
    viewing_id = d["viewingId"].GetString();
    surveyor_id = d["surveyorId"].GetString();
    contribution_probi = d["contribution_probi"].GetString();
    submission_stamp = d["submissionStamp"].GetString();
    anonize_viewing_id = d["anonizeViewingId"].GetString();
    registrar_vk = d["registrarVK"].GetString();
    master_user_token = d["masterUserToken"].GetString();
    votes = d["votes"].GetUint();

    for (auto & i : d["rates"].GetObject()) {
      contribution_rates.insert(
          std::make_pair(i.name.GetString(), i.value.GetDouble()));
    }

    for (auto & i : d["surveyorIds"].GetArray()) {
      surveyor_ids.push_back(i.GetString());
    }

    for (const auto & i : d["ballots"].GetArray()) {
      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      i.Accept(writer);

      TRANSACTION_BALLOT_ST ballot;
      ballot.loadFromJson(sb.GetString());
      ballots.push_back(ballot);
    }
  }

  return !error;
}


void saveToJson(JsonWriter* writer, const TRANSACTION_ST& data) {
  writer->StartObject();

  writer->String("viewingId");
  writer->String(data.viewing_id.c_str());

  writer->String("surveyorId");
  writer->String(data.surveyor_id.c_str());

  writer->String("rates");
  writer->StartObject();
  for (auto & p : data.contribution_rates) {
    writer->String(p.first.c_str());
    writer->Double(p.second);
  }
  writer->EndObject();

  writer->String("contribution_probi");
  writer->String(data.contribution_probi.c_str());

  writer->String("submissionStamp");
  writer->String(data.submission_stamp.c_str());

  writer->String("anonizeViewingId");
  writer->String(data.anonize_viewing_id.c_str());

  writer->String("registrarVK");
  writer->String(data.registrar_vk.c_str());

  writer->String("masterUserToken");
  writer->String(data.master_user_token.c_str());

  writer->String("surveyorIds");
  writer->StartArray();
  for (auto & i : data.surveyor_ids) {
    writer->String(i.c_str());
  }
  writer->EndArray();

  writer->String("votes");
  writer->Uint(data.votes);

  writer->String("ballots");
  writer->StartArray();
  for (auto & i : data.ballots) {
    saveToJson(writer, i);
  }
  writer->EndArray();

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
BALLOT_ST::BALLOT_ST():
  offset(0),
  delay_stamp(0) {}

BALLOT_ST::BALLOT_ST(const BALLOT_ST& ballot) {
  viewing_id = ballot.viewing_id;
  surveyor_id = ballot.surveyor_id;
  publisher = ballot.publisher;
  offset = ballot.offset;
  prepare_ballot = ballot.prepare_ballot;
  proof_ballot = ballot.proof_ballot;
  delay_stamp = ballot.delay_stamp;
}

BALLOT_ST::~BALLOT_ST() {}

void saveToJson(JsonWriter* writer, const BALLOT_ST& data) {
  writer->StartObject();

  writer->String("viewingId");
  writer->String(data.viewing_id.c_str());

  writer->String("surveyorId");
  writer->String(data.surveyor_id.c_str());

  writer->String("publisher");
  writer->String(data.publisher.c_str());

  writer->String("offset");
  writer->Uint(data.offset);

  writer->String("prepareBallot");
  writer->String(data.prepare_ballot.c_str());

  writer->String("delayStamp");
  writer->Uint64(data.delay_stamp);

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
BATCH_VOTES_INFO_ST::BATCH_VOTES_INFO_ST() {}

BATCH_VOTES_INFO_ST::BATCH_VOTES_INFO_ST(const BATCH_VOTES_INFO_ST& other) {
  surveyor_id = other.surveyor_id;
  proof = other.proof;
}

BATCH_VOTES_INFO_ST::~BATCH_VOTES_INFO_ST() {}

void saveToJson(JsonWriter* writer, const BATCH_VOTES_INFO_ST& data) {
  writer->StartObject();

  writer->String("surveyorId");
  writer->String(data.surveyor_id.c_str());

  writer->String("proof");
  writer->String(data.proof.c_str());

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
BATCH_VOTES_ST::BATCH_VOTES_ST() {}

BATCH_VOTES_ST::BATCH_VOTES_ST(const BATCH_VOTES_ST& other) {
  publisher = other.publisher;
  batch_votes_info = other.batch_votes_info;
}

BATCH_VOTES_ST::~BATCH_VOTES_ST() {}

void saveToJson(JsonWriter* writer, const BATCH_VOTES_ST& data) {
  writer->StartObject();

  writer->String("publisher");
  writer->String(data.publisher.c_str());

  writer->String("batchVotesInfo");
  writer->StartArray();
  for (auto & b : data.batch_votes_info) {
    saveToJson(writer, b);
  }
  writer->EndArray();

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
REPORT_BALANCE_ST::REPORT_BALANCE_ST():
  opening_balance("0"),
  closing_balance("0"),
  deposits("0"),
  grants("0"),
  earning_from_ads("0"),
  auto_contribute("0"),
  recurring_donation("0"),
  one_time_donation("0"),
  total("0") {}

REPORT_BALANCE_ST::REPORT_BALANCE_ST(const REPORT_BALANCE_ST& state) {
  opening_balance = state.opening_balance;
  closing_balance = state.closing_balance;
  deposits = state.deposits;
  grants = state.grants;
  earning_from_ads = state.earning_from_ads;
  auto_contribute = state.auto_contribute;
  recurring_donation = state.recurring_donation;
  one_time_donation = state.one_time_donation;
  total = state.total;
}

REPORT_BALANCE_ST::~REPORT_BALANCE_ST() {}

bool REPORT_BALANCE_ST::loadFromJson(const std::string& json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("opening_balance") &&
        d["opening_balance"].IsString() &&
        isProbiValid(d["opening_balance"].GetString()) &&
        d.HasMember("closing_balance") && d["closing_balance"].IsString() &&
        isProbiValid(d["closing_balance"].GetString()) &&
        d.HasMember("deposits") && d["deposits"].IsString() &&
        isProbiValid(d["deposits"].GetString()) &&
        d.HasMember("grants") && d["grants"].IsString() &&
        isProbiValid(d["grants"].GetString()) &&
        d.HasMember("earning_from_ads") && d["earning_from_ads"].IsString() &&
        isProbiValid(d["earning_from_ads"].GetString()) &&
        d.HasMember("auto_contribute") && d["auto_contribute"].IsString() &&
        isProbiValid(d["auto_contribute"].GetString()) &&
        d.HasMember("recurring_donation") &&
        d["recurring_donation"].IsString() &&
        isProbiValid(d["recurring_donation"].GetString()) &&
        d.HasMember("one_time_donation") && d["one_time_donation"].IsString() &&
        isProbiValid(d["one_time_donation"].GetString()) &&
        d.HasMember("total") && d["total"].IsString() &&
        isProbiValid(d["total"].GetString()));
  }

  if (!error) {
    opening_balance = d["opening_balance"].GetString();
    closing_balance = d["closing_balance"].GetString();
    deposits = d["deposits"].GetString();
    grants = d["grants"].GetString();
    earning_from_ads = d["earning_from_ads"].GetString();
    auto_contribute = d["auto_contribute"].GetString();
    recurring_donation = d["recurring_donation"].GetString();
    one_time_donation = d["one_time_donation"].GetString();
    total = d["total"].GetString();
  }

  return !error;
}

void saveToJson(JsonWriter* writer, const REPORT_BALANCE_ST& data) {
  writer->StartObject();

  writer->String("opening_balance");

  writer->String(data.opening_balance.c_str());

  writer->String("closing_balance");

  writer->String(data.closing_balance.c_str());

  writer->String("deposits");

  writer->String(data.deposits.c_str());

  writer->String("grants");

  writer->String(data.grants.c_str());

  writer->String("earning_from_ads");

  writer->String(data.earning_from_ads.c_str());

  writer->String("auto_contribute");

  writer->String(data.auto_contribute.c_str());

  writer->String("recurring_donation");

  writer->String(data.recurring_donation.c_str());

  writer->String("one_time_donation");

  writer->String(data.one_time_donation.c_str());

  writer->String("total");

  writer->String(data.total.c_str());

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
PUBLISHER_STATE_ST::PUBLISHER_STATE_ST():
  min_publisher_duration(braveledger_ledger::_default_min_publisher_duration),
  min_visits(1),
  allow_non_verified(true),
  allow_videos(true) {}

PUBLISHER_STATE_ST::PUBLISHER_STATE_ST(const PUBLISHER_STATE_ST& state) {
  min_publisher_duration = state.min_publisher_duration;
  min_visits = state.min_visits;
  allow_non_verified = state.allow_non_verified;
  allow_videos = state.allow_videos;
  monthly_balances = state.monthly_balances;
  migrate_score_2 = state.migrate_score_2;
  processed_pending_publishers = state.processed_pending_publishers;
}

PUBLISHER_STATE_ST::~PUBLISHER_STATE_ST() {}

bool PUBLISHER_STATE_ST::loadFromJson(const std::string& json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("min_pubslisher_duration") &&
        d["min_pubslisher_duration"].IsUint() &&
        d.HasMember("min_visits") && d["min_visits"].IsUint() &&
        d.HasMember("allow_non_verified") && d["allow_non_verified"].IsBool() &&
        d.HasMember("allow_videos") && d["allow_videos"].IsBool() &&
        d.HasMember("monthly_balances") && d["monthly_balances"].IsArray());
  }

  if (!error) {
    min_publisher_duration = d["min_pubslisher_duration"].GetUint();
    min_visits = d["min_visits"].GetUint();
    allow_non_verified = d["allow_non_verified"].GetBool();
    allow_videos = d["allow_videos"].GetBool();

    for (const auto & i : d["monthly_balances"].GetArray()) {
      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      i.Accept(writer);

      rapidjson::Document d1;
      d1.Parse(sb.GetString());

      rapidjson::Value::ConstMemberIterator itr = d1.MemberBegin();
      if (itr != d1.MemberEnd()) {
        rapidjson::StringBuffer sb1;
        rapidjson::Writer<rapidjson::StringBuffer> writer1(sb1);
        itr->value.Accept(writer1);
        REPORT_BALANCE_ST r;
        r.loadFromJson(sb1.GetString());
        monthly_balances.insert(std::make_pair(itr->name.GetString(), r));
      }
    }

    if (d.HasMember("migrate_score_2") && d["migrate_score_2"].IsBool()) {
      migrate_score_2 = d["migrate_score_2"].GetBool();
    } else {
      migrate_score_2 = true;
    }

    if (d.HasMember("processed_pending_publishers") &&
        d["processed_pending_publishers"].IsArray()) {
      for (const auto & i : d["processed_pending_publishers"].GetArray()) {
        processed_pending_publishers.push_back(i.GetString());
      }
    }
  }

  return !error;
}

void saveToJson(JsonWriter* writer, const PUBLISHER_STATE_ST& data) {
  writer->StartObject();

  writer->String("min_pubslisher_duration");
  writer->Uint(data.min_publisher_duration);

  writer->String("min_visits");
  writer->Uint(data.min_visits);

  writer->String("allow_non_verified");
  writer->Bool(data.allow_non_verified);

  writer->String("allow_videos");
  writer->Bool(data.allow_videos);

  writer->String("monthly_balances");
  writer->StartArray();
  for (auto & p : data.monthly_balances) {
    writer->StartObject();
    writer->String(p.first.c_str());
    saveToJson(writer, p.second);
    writer->EndObject();
  }
  writer->EndArray();

  writer->String("migrate_score_2");
  writer->Bool(data.migrate_score_2);

  writer->String("processed_pending_publishers");
  writer->StartArray();
  for (const auto &p : data.processed_pending_publishers) {
    writer->String(p.c_str());
  }
  writer->EndArray();

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
PUBLISHER_ST::PUBLISHER_ST():
  id(""),
  duration(0),
  score(0),
  visits(0),
  percent(0),
  weight(0),
  status(0) {}

PUBLISHER_ST::~PUBLISHER_ST() {}

bool PUBLISHER_ST::operator<(const PUBLISHER_ST& rhs) const {
  return score > rhs.score;
}

bool PUBLISHER_ST::loadFromJson(const std::string& json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("id") && d["id"].IsString() &&
      d.HasMember("duration") && d["duration"].IsUint64() &&
      d.HasMember("score") && d["score"].IsDouble() &&
      d.HasMember("visits") && d["visits"].IsUint() &&
      d.HasMember("percent") && d["percent"].IsUint() &&
      d.HasMember("weight") && d["weight"].IsDouble());
  }

  if (!error) {
    id = d["id"].GetString();
    duration = d["duration"].GetUint64();
    score = d["score"].GetDouble();
    visits = d["visits"].GetUint();
    percent = d["percent"].GetUint();
    weight = d["weight"].GetDouble();
    status = 0;  // ledger::PublisherStatus::NOT_VERIFIED

    if (d.HasMember("status") && d["status"].IsUint()) {
      status = d["status"].GetUint();
    }

    // LEGACY CHECK
    if (d.HasMember("verified") && d["verified"].IsBool()) {
      const bool verified = d["verified"].GetBool();
      status = verified
          ? 2  // ledger::PublisherStatus::VERIFIED
          : 0;  // ledger::PublisherStatus::NOT_VERIFIED
    }
  }

  return !error;
}

void saveToJson(JsonWriter* writer, const PUBLISHER_ST& data) {
  writer->StartObject();

  writer->String("id");
  writer->String(data.id.c_str());

  writer->String("duration");
  writer->Uint64(data.duration);

  writer->String("score");
  writer->Double(data.score);

  writer->String("visits");
  writer->Uint(data.visits);

  writer->String("percent");
  writer->Uint(data.percent);

  writer->String("weight");
  writer->Double(data.weight);

  writer->String("status");
  writer->Uint(data.status);

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
WINNERS_ST::WINNERS_ST() :
  votes(0) {}

WINNERS_ST::~WINNERS_ST() {}

/////////////////////////////////////////////////////////////////////////////
WALLET_PROPERTIES_ST::WALLET_PROPERTIES_ST() {}

WALLET_PROPERTIES_ST::~WALLET_PROPERTIES_ST() {}

WALLET_PROPERTIES_ST::WALLET_PROPERTIES_ST(
    const WALLET_PROPERTIES_ST &properties) {
  fee_amount = properties.fee_amount;
  parameters_choices = properties.parameters_choices;
  grants = properties.grants;
}

bool WALLET_PROPERTIES_ST::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("parameters") && d["parameters"].IsObject());
  }

  if (!error) {
    for (auto & i : d["parameters"]["adFree"]["choices"]["BAT"].GetArray()) {
      parameters_choices.push_back(i.GetDouble());
    }

    fee_amount = d["parameters"]["adFree"]["fee"]["BAT"].GetDouble();

    if (d.HasMember("grants") && d["grants"].IsArray()) {
      for (auto &i : d["grants"].GetArray()) {
        GRANT grant;
        auto obj = i.GetObject();
        if (obj.HasMember("probi")) {
          grant.probi = obj["probi"].GetString();
        }

        if (obj.HasMember("altcurrency")) {
          grant.alt_currency = obj["altcurrency"].GetString();
        }

        if (obj.HasMember("expiryTime")) {
          grant.expiry_time = obj["expiryTime"].GetUint64();
        }

        if (obj.HasMember("type")) {
          grant.type = obj["type"].GetString();
        }

        grants.push_back(grant);
      }
    } else {
      grants.clear();
    }
  }
  return !error;
}

void saveToJson(JsonWriter* writer, const WALLET_PROPERTIES_ST& data) {
  writer->StartObject();

  writer->String("fee_amount");
  writer->Double(data.fee_amount);

  writer->String("parameters");
  writer->StartObject();
  writer->String("adFree");
  writer->StartObject();

  writer->String("fee");
  writer->StartObject();
  writer->String("BAT");
  writer->Double(data.fee_amount);
  writer->EndObject();

  writer->String("choices");
  writer->StartObject();
  writer->String("BAT");

  writer->StartArray();
  for (auto & choice : data.parameters_choices) {
    writer->Double(choice);
  }
  writer->EndArray();
  writer->EndObject();

  writer->EndObject();
  writer->EndObject();

  writer->String("grants");
  writer->StartArray();
  for (auto & grant : data.grants) {
    saveToJson(writer, grant);
  }
  writer->EndArray();

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
GRANTS_PROPERTIES_ST::GRANTS_PROPERTIES_ST() {}

GRANTS_PROPERTIES_ST::GRANTS_PROPERTIES_ST(
    const GRANTS_PROPERTIES_ST& properties) {
  grants = properties.grants;
}

GRANTS_PROPERTIES_ST::~GRANTS_PROPERTIES_ST() {}

bool GRANTS_PROPERTIES_ST::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("grants") && d["grants"].IsArray());
  }

  if (!error) {
    for (auto &i : d["grants"].GetArray()) {
      GRANT_RESPONSE grant;
      auto obj = i.GetObject();

      if (obj.HasMember("promotionId")) {
        grant.promotion_id = obj["promotionId"].GetString();
      }

      if (obj.HasMember("minimumReconcileTimestamp")) {
        grant.minimum_reconcile_timestamp =
            obj["minimumReconcileTimestamp"].GetUint64();
      }

      if (obj.HasMember("protocolVersion")) {
        grant.protocol_version = obj["protocolVersion"].GetUint64();
      }

      if (obj.HasMember("type")) {
        grant.type = obj["type"].GetString();
      }

      grants.push_back(grant);
    }
  } else {
    grants.clear();
  }
  return !error;
}

/////////////////////////////////////////////////////////////////////////////

GRANT::GRANT() : expiry_time(0) {}

GRANT::~GRANT() {}

GRANT::GRANT(const GRANT &properties) {
  promotion_id = properties.promotion_id;
  alt_currency = properties.alt_currency;
  expiry_time = properties.expiry_time;
  probi = properties.probi;
  type = properties.type;
}

bool GRANT::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (error) {
    return !error;
  }

  // First grant get
  error = !(
      d.HasMember("promotionId") && d["promotionId"].IsString());

  if (!error) {
    promotion_id = d["promotionId"].GetString();

    if (d.HasMember("type") && d["type"].IsString()) {
      type = d["type"].GetString();
    }
    return !error;
  }

  // On successful grant
  error = !(
      d.HasMember("altcurrency") && d["altcurrency"].IsString() &&
      d.HasMember("expiryTime") && d["expiryTime"].IsNumber() &&
      d.HasMember("probi") && d["probi"].IsString());

  if (!error) {
    alt_currency = d["altcurrency"].GetString();
    expiry_time = d["expiryTime"].GetUint64();
    probi = d["probi"].GetString();
    if (d.HasMember("type") && d["type"].IsString()) {
      type = d["type"].GetString();
    }

#if defined(OS_ANDROID)
    if (type == "ugp") {
      type = "android";
    }
#endif
  }

  return !error;
}

GRANT_RESPONSE::GRANT_RESPONSE() {}

GRANT_RESPONSE::~GRANT_RESPONSE() {}

GRANT_RESPONSE::GRANT_RESPONSE(const GRANT_RESPONSE &properties) {
  promotion_id = properties.promotion_id;
  minimum_reconcile_timestamp = properties.minimum_reconcile_timestamp;
  protocol_version = properties.protocol_version;
  type = properties.type;
}

bool GRANT_RESPONSE::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (error) {
    return !error;
  }

  // First grant get
  error = !(d.HasMember("promotionId") && d["promotionId"].IsString());

  if (!error) {
    promotion_id = d["promotionId"].GetString();
    return !error;
  }

  // On successful grant
  error = !(
      d.HasMember("protocolVersion") && d["protocolVersion"].IsNumber() &&
      d.HasMember("minimumReconcileTimestamp") &&
      d["minimumReconcileTimestamp"].IsNumber() &&
      d.HasMember("type") && d["type"].IsString());

  if (!error) {
    minimum_reconcile_timestamp = d["minimumReconcileTimestamp"].GetUint64();
    protocol_version = d["protocolVersion"].GetUint64();
    type = d["type"].GetString();
  }

  return !error;
}

void saveToJson(JsonWriter* writer, const GRANT& data) {
  writer->StartObject();

  writer->String("altcurrency");
  writer->String(data.alt_currency.c_str());

  writer->String("probi");
  writer->String(data.probi.c_str());

  writer->String("expiryTime");
  writer->Uint64(data.expiry_time);

  writer->String("promotionId");
  writer->String(data.promotion_id.c_str());

  writer->String("type");
  writer->String(data.type.c_str());

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
SURVEYOR_INFO_ST::SURVEYOR_INFO_ST() {}

SURVEYOR_INFO_ST::~SURVEYOR_INFO_ST() {}

/////////////////////////////////////////////////////////////////////////////
SURVEYOR_ST::SURVEYOR_ST() {}

SURVEYOR_ST::~SURVEYOR_ST() {}

bool SURVEYOR_ST::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("signature") && d["signature"].IsString() &&
      d.HasMember("surveyorId") && d["surveyorId"].IsString() &&
      d.HasMember("surveyVK") && d["surveyVK"].IsString() &&
      d.HasMember("registrarVK") && d["registrarVK"].IsString());
  }

  if (!error) {
    signature_ = d["signature"].GetString();
    surveyor_id = d["surveyorId"].GetString();
    survey_vk = d["surveyVK"].GetString();
    registrar_vk = d["registrarVK"].GetString();
    if (d.HasMember("surveySK") && d["surveySK"].IsString()) {
      survey_sk = d["surveySK"].GetString();
    }
  }

  return !error;
}

/////////////////////////////////////////////////////////////////////////////
RECONCILE_DIRECTION::RECONCILE_DIRECTION() {}
RECONCILE_DIRECTION::RECONCILE_DIRECTION(const std::string& publisher_key,
                                         const double amount_percent) :
  publisher_key(publisher_key),
  amount_percent(amount_percent) {}
RECONCILE_DIRECTION::~RECONCILE_DIRECTION() {}

bool RECONCILE_DIRECTION::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("publisher_key") && d["publisher_key"].IsString());
  }

  if (!error) {
    if (d.HasMember("amount") && d["amount"].IsInt()) {
      amount_percent = static_cast<double>(d["amount"].GetInt());
    } else if (d["amount_percent"].IsDouble()) {
      amount_percent = d["amount_percent"].GetDouble();
    } else {
      return false;
    }

    publisher_key = d["publisher_key"].GetString();
  }

  return !error;
}

void saveToJson(JsonWriter* writer, const RECONCILE_DIRECTION& data) {
  writer->StartObject();

  writer->String("amount_percent");
  writer->Double(data.amount_percent);

  writer->String("publisher_key");
  writer->String(data.publisher_key.c_str());

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
CURRENT_RECONCILE::CURRENT_RECONCILE() :
  timestamp(0),
  fee(.0),
  retry_step(ledger::ContributionRetry::STEP_NO),
  retry_level(0) {}

CURRENT_RECONCILE::CURRENT_RECONCILE(const CURRENT_RECONCILE& data):
  viewing_id(data.viewing_id),
  anonize_viewing_id(data.anonize_viewing_id),
  registrar_vk(data.registrar_vk),
  pre_flight(data.pre_flight),
  master_user_token(data.master_user_token),
  surveyor_info(data.surveyor_info),
  timestamp(data.timestamp),
  rates(data.rates),
  amount(data.amount),
  currency(data.currency),
  fee(data.fee),
  directions(data.directions),
  type(data.type),
  retry_step(data.retry_step),
  retry_level(data.retry_level),
  destination(data.destination),
  proof(data.proof) {}

CURRENT_RECONCILE::~CURRENT_RECONCILE() {}

bool CURRENT_RECONCILE::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("viewingId") && d["viewingId"].IsString() &&
      d.HasMember("fee") && d["fee"].IsDouble() &&
      ((d.HasMember("category") && d["category"].IsInt()) ||
      (d.HasMember("type") && d["type"].IsInt())));
  }

  if (!error) {
    viewing_id = d["viewingId"].GetString();
    anonize_viewing_id = d["anonizeViewingId"].GetString();
    registrar_vk = d["registrarVK"].GetString();
    pre_flight = d["preFlight"].GetString();
    master_user_token = d["masterUserToken"].GetString();
    timestamp = d["timestamp"].GetUint64();
    amount = d["amount"].GetString();
    currency = d["currency"].GetString();
    fee = d["fee"].GetDouble();
    if (d.HasMember("category") && d["category"].IsInt()) {
      type = static_cast<ledger::RewardsType>(d["category"].GetInt());
    } else {
      type = static_cast<ledger::RewardsType>(d["type"].GetInt());
    }

    if (d.HasMember("surveyorInfo") && d["surveyorInfo"].IsObject()) {
      auto obj = d["surveyorInfo"].GetObject();
      SURVEYOR_INFO_ST info;
      info.surveyor_id = obj["surveyorId"].GetString();
      surveyor_info = info;
    }

    if (d.HasMember("rates") && d["rates"].IsObject()) {
      for (auto & i : d["rates"].GetObject()) {
        rates.insert(std::make_pair(i.name.GetString(), i.value.GetDouble()));
      }
    }

    if (d.HasMember("directions") && d["directions"].IsArray()) {
      for (auto & i : d["directions"].GetArray()) {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        i.Accept(writer);

        RECONCILE_DIRECTION direction;
        direction.loadFromJson(sb.GetString());
        directions.push_back(direction);
      }
    }

    // LEGACY MIGRATION from publisher object
    if (d.HasMember("list") && d["list"].IsArray()) {
      for (auto &i : d["list"].GetArray()) {
        RECONCILE_DIRECTION direction;

        auto obj = i.GetObject();
        direction.publisher_key = obj["id"].GetString();
        direction.amount_percent = obj["weight"].GetDouble();
        directions.push_back(direction);
      }
    }

    if (d.HasMember("retry_step") && d["retry_step"].IsInt()) {
      retry_step = static_cast<ledger::ContributionRetry>(
          d["retry_step"].GetInt());
    } else {
      retry_step = ledger::ContributionRetry::STEP_NO;
    }

    if (d.HasMember("retry_level") && d["retry_level"].IsInt()) {
      retry_level = d["retry_level"].GetInt();
    } else {
      retry_level = 0;
    }

    if (d.HasMember("destination") && d["destination"].IsString()) {
      destination = d["destination"].GetString();
    }

    if (d.HasMember("proof") && d["proof"].IsString()) {
      proof = d["proof"].GetString();
    }
  }

  return !error;
}

void saveToJson(JsonWriter* writer, const CURRENT_RECONCILE& data) {
  writer->StartObject();

  writer->String("viewingId");
  writer->String(data.viewing_id.c_str());

  writer->String("anonizeViewingId");
  writer->String(data.anonize_viewing_id.c_str());

  writer->String("registrarVK");
  writer->String(data.registrar_vk.c_str());

  writer->String("preFlight");
  writer->String(data.pre_flight.c_str());

  writer->String("masterUserToken");
  writer->String(data.master_user_token.c_str());

  writer->String("surveyorInfo");
  writer->StartObject();
    writer->String("surveyorId");
    writer->String(data.surveyor_info.surveyor_id.c_str());
  writer->EndObject();

  writer->String("timestamp");
  writer->Uint64(data.timestamp);

  writer->String("amount");
  writer->String(data.amount.c_str());

  writer->String("currency");
  writer->String(data.currency.c_str());

  writer->String("fee");
  writer->Double(data.fee);

  writer->String("type");
  writer->Int(static_cast<int>(data.type));

  writer->String("rates");
  writer->StartObject();
  for (auto & p : data.rates) {
    writer->String(p.first.c_str());
    writer->Double(p.second);
  }
  writer->EndObject();

  writer->String("directions");
  writer->StartArray();
  for (auto & i : data.directions) {
    saveToJson(writer, i);
  }
  writer->EndArray();

  writer->String("retry_step");
  writer->Int(static_cast<int32_t>(data.retry_step));

  writer->String("retry_level");
  writer->Int(data.retry_level);

  writer->String("destination");
  writer->String(data.destination.c_str());

  writer->String("proof");
  writer->String(data.proof.c_str());

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
CLIENT_STATE_ST::CLIENT_STATE_ST():
  boot_stamp(0),
  reconcile_stamp(0),
  last_grant_fetch_stamp(0),
  settings(AD_FREE_SETTINGS),
  fee_amount(0),
  user_changed_fee(false),
  days(0),
  auto_contribute(false),
  rewards_enabled(false) {}

CLIENT_STATE_ST::CLIENT_STATE_ST(const CLIENT_STATE_ST& other) {
  wallet_properties = other.wallet_properties;
  wallet_info = other.wallet_info;
  boot_stamp = other.boot_stamp;
  reconcile_stamp = other.reconcile_stamp;
  last_grant_fetch_stamp = other.last_grant_fetch_stamp;
  persona_id = other.persona_id;
  user_id = other.user_id;
  registrar_vk = other.registrar_vk;
  master_user_token = other.master_user_token;
  pre_flight = other.pre_flight;
  fee_currency = other.fee_currency;
  settings = other.settings;
  fee_amount = other.fee_amount;
  user_changed_fee = other.user_changed_fee;
  days = other.days;
  transactions = other.transactions;
  ballots = other.ballots;
  rule_set = other.rule_set;
  rule_set_v2 = other.rule_set_v2;
  batch = other.batch;
  auto_contribute = other.auto_contribute;
  rewards_enabled = other.rewards_enabled;
  current_reconciles = other.current_reconciles;
  inline_tip = other.inline_tip;
  grants = other.grants;
}

CLIENT_STATE_ST::~CLIENT_STATE_ST() {}

bool CLIENT_STATE_ST::loadFromJson(const std::string & json) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser error or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("walletInfo") && d["walletInfo"].IsObject() &&
      d.HasMember("bootStamp") && d["bootStamp"].IsUint64() &&
      d.HasMember("reconcileStamp") && d["reconcileStamp"].IsUint64() &&
      d.HasMember("personaId") && d["personaId"].IsString() &&
      d.HasMember("userId") && d["userId"].IsString() &&
      d.HasMember("registrarVK") && d["registrarVK"].IsString() &&
      d.HasMember("masterUserToken") && d["masterUserToken"].IsString() &&
      d.HasMember("preFlight") && d["preFlight"].IsString() &&
      d.HasMember("fee_currency") && d["fee_currency"].IsString() &&
      d.HasMember("settings") && d["settings"].IsString() &&
      d.HasMember("fee_amount") && d["fee_amount"].IsDouble() &&
      d.HasMember("user_changed_fee") && d["user_changed_fee"].IsBool() &&
      d.HasMember("days") && d["days"].IsUint() &&
      d.HasMember("transactions") && d["transactions"].IsArray() &&
      d.HasMember("ballots") && d["ballots"].IsArray() &&
      d.HasMember("ruleset") && d["ruleset"].IsString() &&
      d.HasMember("rulesetV2") && d["rulesetV2"].IsString() &&
      d.HasMember("batch") && d["batch"].IsArray() &&
      d.HasMember("auto_contribute") && d["auto_contribute"].IsBool() &&
      d.HasMember("rewards_enabled") && d["rewards_enabled"].IsBool());
  }

  if (!error) {
    {
      auto & i = d["walletInfo"];
      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      i.Accept(writer);
      wallet_info.loadFromJson(sb.GetString());
    }

    boot_stamp = d["bootStamp"].GetUint64();
    reconcile_stamp = d["reconcileStamp"].GetUint64();

    if (d.HasMember("last_grant_fetch_stamp") &&
        d["last_grant_fetch_stamp"].IsUint64()) {
      last_grant_fetch_stamp = d["last_grant_fetch_stamp"].GetUint64();
    } else {
      last_grant_fetch_stamp = 0u;
    }

    persona_id = d["personaId"].GetString();
    user_id = d["userId"].GetString();
    registrar_vk = d["registrarVK"].GetString();
    master_user_token = d["masterUserToken"].GetString();
    pre_flight = d["preFlight"].GetString();
    fee_currency = d["fee_currency"].GetString();
    settings = d["settings"].GetString();
    fee_amount = d["fee_amount"].GetDouble();
    user_changed_fee = d["user_changed_fee"].GetBool();
    days = d["days"].GetUint();
    auto_contribute = d["auto_contribute"].GetBool();
    rewards_enabled = d["rewards_enabled"].GetBool();

    for (const auto & i : d["transactions"].GetArray()) {
      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      i.Accept(writer);

      TRANSACTION_ST ta;
      ta.loadFromJson(sb.GetString());
      transactions.push_back(ta);
    }

    for (const auto & i : d["ballots"].GetArray()) {
      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      i.Accept(writer);

      BALLOT_ST b;
      const ledger::BallotJsonState ballot_state;
      if (!ballot_state.FromJson(sb.GetString(), &b)) {
        continue;
      }

      ballots.push_back(b);
    }

    rule_set = d["ruleset"].GetString();
    rule_set_v2 = d["rulesetV2"].GetString();

    for (const auto & i : d["batch"].GetArray()) {
      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      i.Accept(writer);

      BATCH_VOTES_ST b;
      const ledger::BatchVotesJsonState batch_votes_state;
      if (!batch_votes_state.FromJson(sb.GetString(), &b)) {
        continue;
      }

      batch.push_back(b);
    }

    if (d.HasMember("current_reconciles") &&
        d["current_reconciles"].IsObject()) {
      for (const auto & i : d["current_reconciles"].GetObject()) {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        i.value.Accept(writer);

        CURRENT_RECONCILE b;
        b.loadFromJson(sb.GetString());
        current_reconciles[i.name.GetString()] = b;
      }
    }

    if (d.HasMember("walletProperties") && d["walletProperties"].IsObject()) {
      auto & i = d["walletProperties"];
      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      i.Accept(writer);
      wallet_properties.loadFromJson(sb.GetString());
    }

    if (d.HasMember("inlineTip") && d["inlineTip"].IsObject()) {
      for (auto & k : d["inlineTip"].GetObject()) {
        inline_tip.insert(
            std::make_pair(k.name.GetString(), k.value.GetBool()));
      }
    }
  }

  return !error;
}

void saveToJson(JsonWriter* writer, const CLIENT_STATE_ST& data) {
  writer->StartObject();

  writer->String("walletInfo");
  saveToJson(writer, data.wallet_info);

  writer->String("bootStamp");
  writer->Uint64(data.boot_stamp);

  writer->String("reconcileStamp");
  writer->Uint64(data.reconcile_stamp);

  writer->String("last_grant_fetch_stamp");
  writer->Uint64(data.last_grant_fetch_stamp);

  writer->String("personaId");
  writer->String(data.persona_id.c_str());

  writer->String("userId");
  writer->String(data.user_id.c_str());

  writer->String("registrarVK");
  writer->String(data.registrar_vk.c_str());

  writer->String("masterUserToken");
  writer->String(data.master_user_token.c_str());

  writer->String("preFlight");
  writer->String(data.pre_flight.c_str());

  writer->String("fee_currency");
  writer->String(data.fee_currency.c_str());

  writer->String("settings");
  writer->String(data.settings.c_str());

  writer->String("fee_amount");
  writer->Double(data.fee_amount);

  writer->String("user_changed_fee");
  writer->Bool(data.user_changed_fee);

  writer->String("days");
  writer->Uint(data.days);

  writer->String("rewards_enabled");
  writer->Bool(data.rewards_enabled);

  writer->String("auto_contribute");
  writer->Bool(data.auto_contribute);

  writer->String("transactions");
  writer->StartArray();
  for (auto & t : data.transactions) {
    saveToJson(writer, t);
  }
  writer->EndArray();

  writer->String("ballots");
  writer->StartArray();
  for (auto & b : data.ballots) {
    saveToJson(writer, b);
  }
  writer->EndArray();

  writer->String("ruleset");
  writer->String(data.rule_set.c_str());

  writer->String("rulesetV2");
  writer->String(data.rule_set_v2.c_str());

  writer->String("batch");
  writer->StartArray();
  for (auto & b : data.batch) {
    saveToJson(writer, b);
  }
  writer->EndArray();

  writer->String("current_reconciles");
  writer->StartObject();
  for (auto & t : data.current_reconciles) {
    writer->Key(t.first.c_str());
    saveToJson(writer, t.second);
  }
  writer->EndObject();

  writer->String("walletProperties");
  saveToJson(writer, data.wallet_properties);

  writer->String("inlineTip");
  writer->StartObject();
  for (auto & p : data.inline_tip) {
    writer->String(p.first.c_str());
    writer->Bool(p.second);
  }
  writer->EndObject();

  writer->EndObject();
}

/////////////////////////////////////////////////////////////////////////////
BATCH_PROOF::BATCH_PROOF() {}

BATCH_PROOF::~BATCH_PROOF() {}

/////////////////////////////////////////////////////////////////////////////

bool getJSONValue(const std::string& fieldName,
                  const std::string& json,
                  std::string* value) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError() || (false == d.HasMember(fieldName.c_str()));
  if (!error) {
    *value = d[fieldName.c_str()].GetString();
  }
  return !error;
}

bool getJSONList(const std::string& fieldName,
                 const std::string& json,
                 std::vector<std::string>* value) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError() ||
      (false == (d.HasMember(fieldName.c_str()) &&
       d[fieldName.c_str()].IsArray()));
  if (!error) {
    for (auto & i : d[fieldName.c_str()].GetArray()) {
      value->push_back(i.GetString());
    }
  }
  return !error;
}

bool getJSONTwitchProperties(
    const std::string& json,
    std::vector<std::map<std::string, std::string>>* parts) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    for (auto & i : d.GetArray()) {
      const char * event_field = "event";
      std::map<std::string, std::string> eventmap;

      auto obj = i.GetObject();
      if (obj.HasMember(event_field)) {
        eventmap[event_field] = obj[event_field].GetString();
      }

      const char * props_field = "properties";
      if (obj.HasMember(props_field)) {
        eventmap[props_field] = "";

        const char * channel_field = "channel";
        if (obj[props_field].HasMember(channel_field) &&
          obj[props_field][channel_field].IsString()) {
          eventmap[channel_field] = obj[props_field][channel_field].GetString();
        }

        const char * vod_field = "vod";
        if (obj[props_field].HasMember(vod_field)) {
          eventmap[vod_field] = obj[props_field][vod_field].GetString();
        }

        const char * time_field = "time";
        if (obj[props_field].HasMember(time_field)) {
          double d = obj[props_field][time_field].GetDouble();
          eventmap[time_field] = std::to_string(d);
        }
      }
      parts->push_back(eventmap);
    }
  }
  return !error;
}

bool getJSONBatchSurveyors(const std::string& json,
                           std::vector<std::string>* surveyors) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    for (auto & i : d.GetArray()) {
      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      i.Accept(writer);
      std::string surveyor = sb.GetString();
      surveyors->push_back(surveyor);
    }
  }

  return !error;
}

bool getJSONRates(const std::string& json,
                  std::map<std::string, double>* rates) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("rates") && d["rates"].IsObject() &&
      d["rates"].HasMember("ETH") &&
      d["rates"].HasMember("LTC") &&
      d["rates"].HasMember("BTC") &&
      d["rates"].HasMember("USD") &&
      d["rates"].HasMember("EUR"));
  }

  if (!error) {
    for (auto & i : d["rates"].GetObject()) {
      double value = 0.0;
      if (i.value.IsDouble()) {
        value = i.value.GetDouble();
      } else if (i.value.IsString()) {
        value = std::stod(i.value.GetString());
      }
      rates->insert(std::make_pair(i.name.GetString(), value));
    }
  }
  return !error;
}

bool getJSONTransaction(const std::string& json, TRANSACTION_ST* transaction) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("paymentStamp") && d["paymentStamp"].IsUint64() &&
      d.HasMember("probi") && d["probi"].IsString() &&
      d.HasMember("altcurrency") && d["altcurrency"].IsString() );
  }

  if (!error) {
    uint64_t stamp = d["paymentStamp"].GetUint64();
    transaction->submission_stamp = std::to_string(stamp);
    transaction->contribution_probi = d["probi"].GetString();
  }
  return !error;
}

bool getJSONUnsignedTx(const std::string& json, UNSIGNED_TX* unsignedTx) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !d.HasMember("unsignedTx") || !(d["unsignedTx"].IsObject());
  }

  if (!error) {
    unsignedTx->amount =
        d["unsignedTx"]["denomination"]["amount"].GetString();
    unsignedTx->currency =
        d["unsignedTx"]["denomination"]["currency"].GetString();
    unsignedTx->destination_ = d["unsignedTx"]["destination"].GetString();
  }
  return !error;
}

bool getJSONWalletInfo(const std::string& json,
                       WALLET_INFO_ST* walletInfo,
                       std::string* fee_currency,
                       double* fee_amount,
                       unsigned int* days) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(
        ((d.HasMember("parameters") && d["parameters"].IsObject()) &&
         (d.HasMember("addresses") && d["addresses"].IsObject())) ||
        ((d.HasMember("payload") && d["payload"].IsObject()) &&
         (d.HasMember("wallet") && d["wallet"].IsObject())));
  }

  if (!error) {
    if (d.HasMember("payload") && d["payload"].IsObject()) {
      walletInfo->payment_id = d["wallet"]["paymentId"].GetString();
      walletInfo->address_bat = d["wallet"]["addresses"]["BAT"].GetString();
      walletInfo->address_btc = d["wallet"]["addresses"]["BTC"].GetString();
      walletInfo->address_card_id =
          d["wallet"]["addresses"]["CARD_ID"].GetString();
      walletInfo->address_eth = d["wallet"]["addresses"]["ETH"].GetString();
      walletInfo->address_ltc = d["wallet"]["addresses"]["LTC"].GetString();

      *days = d["payload"]["adFree"]["days"].GetUint();
      const auto & fee = d["payload"]["adFree"]["fee"].GetObject();
      auto itr = fee.MemberBegin();
      if (itr != fee.MemberEnd()) {
        *fee_currency = itr->name.GetString();
        *fee_amount = itr->value.GetDouble();
      }
    } else if (d.HasMember("parameters") && d["parameters"].IsObject()) {
      walletInfo->address_bat = d["addresses"]["BAT"].GetString();
      walletInfo->address_btc = d["addresses"]["BTC"].GetString();
      walletInfo->address_card_id = d["addresses"]["CARD_ID"].GetString();
      walletInfo->address_eth = d["addresses"]["ETH"].GetString();
      walletInfo->address_ltc = d["addresses"]["LTC"].GetString();
      *days = d["parameters"]["adFree"]["days"].GetUint();
      const auto & fee = d["parameters"]["adFree"]["fee"].GetObject();
      auto itr = fee.MemberBegin();
      if (itr != fee.MemberEnd()) {
        *fee_currency = itr->name.GetString();
        *fee_amount = itr->value.GetDouble();
      }
    }
  }
  return !error;
}

bool getJSONRecoverWallet(const std::string& json,
                          double* balance,
                          std::vector<GRANT>* grants) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("balance") && d["balance"].IsString());
  }

  if (!error) {
    *balance = std::stod(d["balance"].GetString());

    if (d.HasMember("grants") && d["grants"].IsArray()) {
      for (auto &i : d["grants"].GetArray()) {
        GRANT grant;
        auto obj = i.GetObject();
        if (obj.HasMember("probi")) {
          grant.probi = obj["probi"].GetString();
        }

        if (obj.HasMember("altcurrency")) {
          grant.alt_currency = obj["altcurrency"].GetString();
        }

        if (obj.HasMember("expiryTime")) {
          grant.expiry_time = obj["expiryTime"].GetUint64();
        }

        if (obj.HasMember("type")) {
          grant.type = obj["type"].GetString();
        }

        grants->push_back(grant);
      }
    } else {
      grants->clear();
    }
  }
  return !error;
}

bool getJSONResponse(const std::string& json,
                     unsigned int* statusCode,
                     std::string* error) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool hasError = d.HasParseError();
  if (!hasError) {
    hasError = !(d.HasMember("statusCode") && d["statusCode"].IsNumber() &&
              d.HasMember("error") && d["error"].IsString());
  }

  if (!hasError) {
    *statusCode = d["statusCode"].GetUint();
    *error = d["error"].GetString();
  }
  return !hasError;
}

bool getJSONGrant(const std::string& json, uint64_t* expiryTime) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool hasError = d.HasParseError();
  if (!hasError) {
    hasError = !(d.HasMember("expiryTime") && d["expiryTime"].IsNumber());
  }

  if (!hasError) {
    *expiryTime = d["expiryTime"].GetUint();
  }
  return !hasError;
}

bool getJSONAddresses(const std::string& json,
                      std::map<std::string, std::string>* addresses) {
  rapidjson::Document d;
  d.Parse(json.c_str());

  // has parser errors or wrong types
  bool error = d.HasParseError();
  if (!error) {
    error = !(d.HasMember("addresses") && d["addresses"].IsObject());
  }

  if (!error) {
    addresses->insert(
        std::make_pair("BAT", d["addresses"]["BAT"].GetString()));
    addresses->insert(
        std::make_pair("BTC", d["addresses"]["BTC"].GetString()));
    addresses->insert(
        std::make_pair("CARD_ID", d["addresses"]["CARD_ID"].GetString()));
    addresses->insert(
        std::make_pair("ETH", d["addresses"]["ETH"].GetString()));
    addresses->insert(
        std::make_pair("LTC", d["addresses"]["LTC"].GetString()));
  }

  return !error;
}

bool getJSONMessage(const std::string& json,
                     std::string* message) {
  DCHECK(message);
  rapidjson::Document d;
  d.Parse(json.c_str());

  if (message && d.HasMember("message")) {
    *message = d["message"].GetString();
    return true;
  }

  return false;
}

std::vector<uint8_t> generateSeed() {
  std::vector<uint8_t> vSeed(SEED_LENGTH);
  std::random_device r;
  std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
  auto rand = std::bind(std::uniform_int_distribution<>(0, UCHAR_MAX),
                        std::mt19937(seed));

  std::generate_n(vSeed.begin(), SEED_LENGTH, rand);
  return vSeed;
}

std::vector<uint8_t> getHKDF(const std::vector<uint8_t>& seed) {
  DCHECK(!seed.empty());
  std::vector<uint8_t> out(SEED_LENGTH);

  const uint8_t info[] = {0};
  int hkdfRes = HKDF(&out.front(),
                     SEED_LENGTH,
                     EVP_sha512(),
                     &seed.front(),
                     seed.size(),
                     braveledger_ledger::g_hkdfSalt,
                     SALT_LENGTH,
                     info,
                     sizeof(info) / sizeof(info[0]));

  DCHECK(hkdfRes);
  DCHECK(!seed.empty());

  // We set the key_length to the length of the expected output and then take
  // the result from the first key, which is the client write key.

  return out;
}

bool getPublicKeyFromSeed(const std::vector<uint8_t>& seed,
                          std::vector<uint8_t>* publicKey,
                          std::vector<uint8_t>* secretKey) {
  DCHECK(!seed.empty());
  if (seed.empty()) {
    return false;
  }
  publicKey->resize(crypto_sign_PUBLICKEYBYTES);
  *secretKey = seed;
  secretKey->resize(crypto_sign_SECRETKEYBYTES);

  crypto_sign_keypair(&publicKey->front(), &secretKey->front(), 1);

  DCHECK(!publicKey->empty() && !secretKey->empty());
  if (publicKey->empty() && secretKey->empty()) {
    return false;
  }

  return true;
}

std::string uint8ToHex(const std::vector<uint8_t>& in) {
  std::ostringstream res;
  for (size_t i = 0; i < in.size(); i++) {
    res << std::setfill('0') << std::setw(sizeof(uint8_t) * 2)
       << std::hex << static_cast<int>(in[i]);
  }
  return res.str();
}


std::string stringifyBatch(std::vector<BATCH_VOTES_INFO_ST> payload) {
  rapidjson::StringBuffer buffer;
  JsonWriter writer(buffer);

  writer.StartArray();
  for (auto & d : payload) {
    saveToJson(&writer, d);
  }
  writer.EndArray();

  return buffer.GetString();
}

std::string stringify(std::string* keys,
                      std::string* values,
                      const unsigned int size) {
  rapidjson::StringBuffer buffer;
  JsonWriter writer(buffer);
  writer.StartObject();

  for (unsigned int i = 0; i < size; i++) {
    writer.String(keys[i].c_str());
    writer.String(values[i].c_str());
  }

  writer.EndObject();
  return buffer.GetString();
}

std::string stringifyUnsignedTx(const UNSIGNED_TX& unsignedTx) {
  rapidjson::StringBuffer buffer;
  JsonWriter writer(buffer);
  writer.StartObject();

  writer.String("denomination");
  writer.StartObject();

  writer.String("amount");
  writer.String(unsignedTx.amount.c_str());

  writer.String("currency");
  writer.String(unsignedTx.currency.c_str());
  writer.EndObject();

  writer.String("destination");
  writer.String(unsignedTx.destination_.c_str());

  writer.EndObject();
  return buffer.GetString();
}

std::string stringifyReconcilePayloadSt(
    const RECONCILE_PAYLOAD_ST& reconcile_payload) {
  rapidjson::StringBuffer buffer;
  JsonWriter writer(buffer);
  writer.StartObject();  // root

  if (!reconcile_payload.request_type.empty()) {
    writer.String("requestType");
    writer.String(reconcile_payload.request_type.c_str());
  }

  writer.String("signedTx");
  writer.StartObject();  // signedTx

  writer.String("headers");
  writer.StartObject();  // headers

  writer.String("digest");
  writer.String(reconcile_payload.request_signed_tx_headers_digest.c_str());

  writer.String("signature");
  writer.String(reconcile_payload.request_signed_tx_headers_signature.c_str());

  writer.EndObject();  // headers

  writer.String("body");
  writer.StartObject();  // body

  writer.String("denomination");
  writer.StartObject();  // denomination

  writer.String("amount");
  writer.String(reconcile_payload.request_signed_tx_body.amount.c_str());

  writer.String("currency");
  writer.String(reconcile_payload.request_signed_tx_body.currency.c_str());

  writer.EndObject();  // denomination

  writer.String("destination");
  writer.String(reconcile_payload.request_signed_tx_body.destination_.c_str());

  writer.EndObject();  // body

  writer.String("octets");
  writer.String(reconcile_payload.request_signed_tx_octets.c_str());

  writer.EndObject();  // signedTx

  if (!reconcile_payload.request_surveyor_id.empty()) {
    writer.String("surveyorId");
    writer.String(reconcile_payload.request_surveyor_id.c_str());
  }

  if (!reconcile_payload.request_viewing_id.empty()) {
    writer.String("viewingId");
    writer.String(reconcile_payload.request_viewing_id.c_str());
  }

  writer.EndObject();  // root
  return buffer.GetString();
}

std::vector<uint8_t> getSHA256(const std::string& in) {
  std::vector<uint8_t> res(SHA256_DIGEST_LENGTH);
  SHA256((uint8_t*)in.c_str(), in.length(), &res.front());
  return res;
}

std::string getBase64(const std::vector<uint8_t>& in) {
  std::string res;
  size_t size = 0;
  if (!EVP_EncodedLength(&size, in.size())) {
    DCHECK(false);
    return "";
  }
  std::vector<uint8_t> out(size);
  int numEncBytes = EVP_EncodeBlock(&out.front(), &in.front(), in.size());
  DCHECK_NE(numEncBytes, 0);
  res = reinterpret_cast<char*>(&out.front());
  return res;
}

bool getFromBase64(const std::string& in, std::vector<uint8_t>* out) {
  bool succeded = true;
  size_t size = 0;
  if (!EVP_DecodedLength(&size, in.length())) {
    DCHECK(false);
    succeded = false;
  }

  if (succeded) {
    out->resize(size);
    size_t final_size = 0;
    int numDecBytes = EVP_DecodeBase64(&out->front(),
                                       &final_size,
                                       size,
                                       (const uint8_t*)in.c_str(),
                                       in.length());
    DCHECK_NE(numDecBytes, 0);

    if (numDecBytes == 0) {
      succeded = false;
      out->clear();
    } else if (final_size != size) {
      out->resize(final_size);
    }
  }
  return succeded;
}

std::string sign(std::string* keys,
                 std::string* values,
                 const unsigned int size,
                 const std::string& keyId,
                 const std::vector<uint8_t>& secretKey) {
  std::string headers;
  std::string message;
  for (unsigned int i = 0; i < size; i++) {
    if (i != 0) {
      headers += " ";
      message += "\n";
    }
    headers += keys[i];
    message += keys[i] + ": " + values[i];
  }
  std::vector<uint8_t> signedMsg(crypto_sign_BYTES + message.length());

  unsigned long long signedMsgSize = 0;  // NOLINT
  crypto_sign(&signedMsg.front(),
              &signedMsgSize,
              reinterpret_cast<const unsigned char*>(message.c_str()),
              (unsigned long long)message.length(),  // NOLINT
              &secretKey.front());

  std::vector<uint8_t> signature(crypto_sign_BYTES);
  std::copy(signedMsg.begin(),
            signedMsg.begin() + crypto_sign_BYTES,
            signature.begin());

  return "keyId=\"" + keyId + "\",algorithm=\"" + SIGNATURE_ALGORITHM +
    "\",headers=\"" + headers + "\",signature=\"" + getBase64(signature) + "\"";
}

uint64_t currentTime() {
  return time(0);
}

bool HasSameDomainAndPath(
    const std::string& url_to_validate,
    const std::string& domain_to_match,
    const std::string& path_to_match) {
  GURL gurl(url_to_validate);
  return gurl.is_valid() && !domain_to_match.empty() &&
      gurl.DomainIs(domain_to_match) &&
      gurl.has_path() && !path_to_match.empty() &&
      gurl.path().substr(0, path_to_match.size()) == path_to_match;
}

std::string buildURL(const std::string& path,
                     const std::string& prefix,
                     const SERVER_TYPES& server) {
  std::string url;
  switch (server) {
    case SERVER_TYPES::BALANCE:
      switch (ledger::_environment) {
        case ledger::Environment::STAGING:
          url = BALANCE_STAGING_SERVER;
          break;
        case ledger::Environment::PRODUCTION:
          url = BALANCE_PRODUCTION_SERVER;
          break;
        case ledger::Environment::DEVELOPMENT:
          url = BALANCE_DEVELOPMENT_SERVER;
          break;
      }
      break;
    case SERVER_TYPES::PUBLISHER:
      switch (ledger::_environment) {
        case ledger::Environment::STAGING:
          url = PUBLISHER_STAGING_SERVER;
          break;
        case ledger::Environment::PRODUCTION:
          url = PUBLISHER_PRODUCTION_SERVER;
          break;
        case ledger::Environment::DEVELOPMENT:
          url = PUBLISHER_DEVELOPMENT_SERVER;
          break;
      }
      break;
    case SERVER_TYPES::PUBLISHER_DISTRO:
      switch (ledger::_environment) {
        case ledger::Environment::STAGING:
          url = PUBLISHER_DISTRO_STAGING_SERVER;
          break;
        case ledger::Environment::PRODUCTION:
          url = PUBLISHER_DISTRO_PRODUCTION_SERVER;
          break;
        case ledger::Environment::DEVELOPMENT:
          url = PUBLISHER_DISTRO_DEVELOPMENT_SERVER;
          break;
      }
      break;
    case SERVER_TYPES::LEDGER:
      switch (ledger::_environment) {
        case ledger::Environment::STAGING:
          url = LEDGER_STAGING_SERVER;
          break;
        case ledger::Environment::PRODUCTION:
          url = LEDGER_PRODUCTION_SERVER;
          break;
        case ledger::Environment::DEVELOPMENT:
          url = LEDGER_DEVELOPMENT_SERVER;
          break;
      }
      break;
  }

  return url + prefix + path;
}

std::vector<std::string> split(const std::string& s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> result;
  while (getline(ss, item, delim)) {
    if (s[0] != '\n') {
      result.push_back(item);
    }
  }

  return result;
}

bool ignore_for_testing() {
  return ledger::is_testing;
}

std::string toLowerCase(std::string word) {
  std::transform(word.begin(), word.end(), word.begin(), ::tolower);
  return word;
}

uint8_t niceware_mnemonic_to_bytes(
    const std::string& w,
    std::vector<uint8_t>* bytes_out,
    size_t* written,
    std::vector<std::string> wordDictionary) {
  std::vector<std::string> wordList = braveledger_bat_helper::split(
      toLowerCase(w),
      WALLET_PASSPHRASE_DELIM);
  std::vector<uint8_t> buffer(wordList.size() * 2);

  for (uint8_t ix = 0; ix < wordList.size(); ix++) {
    std::vector<std::string>::iterator it =
      std::find(wordDictionary.begin(),
      wordDictionary.end(), wordList[ix]);
    if (it != wordDictionary.end()) {
      int wordIndex = std::distance(wordDictionary.begin(), it);
      buffer[2 * ix] = floor(wordIndex / 256);
      buffer[2 * ix + 1] = wordIndex % 256;
    } else {
      return INVALID_LEGACY_WALLET;
    }
  }
  *bytes_out = buffer;
  *written = NICEWARE_BYTES_WRITTEN;
  return 0;
}

void saveToJson(JsonWriter* writer, const ledger::VisitData& visitData) {
  writer->StartObject();

  writer->String("tld");
  writer->String(visitData.tld.c_str());

  writer->String("domain");
  writer->String(visitData.domain.c_str());

  writer->String("path");
  writer->String(visitData.path.c_str());

  writer->String("tab_id");
  writer->Uint(visitData.tab_id);

  writer->String("name");
  writer->String(visitData.name.c_str());

  writer->String("url");
  writer->String(visitData.url.c_str());

  writer->String("provider");
  writer->String(visitData.provider.c_str());

  writer->String("favicon_url");
  writer->String(visitData.favicon_url.c_str());

  writer->EndObject();
}

}  // namespace braveledger_bat_helper
