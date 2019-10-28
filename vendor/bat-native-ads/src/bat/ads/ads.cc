/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/ads.h"

#include "bat/ads/internal/ads_impl.h"
#include "bat/ads/internal/locale_helper.h"
#include "bat/ads/internal/static_values.h"

namespace ads {

bool _is_debug = false;
bool _is_testing = false;
Environment _environment = Environment::DEVELOPMENT;

const char _bundle_schema_resource_name[] = "bundle-schema.json";
const char _catalog_schema_resource_name[] = "catalog-schema.json";
const char _catalog_resource_name[] = "catalog.json";
const char _client_resource_name[] = "client.json";

// static
Ads* Ads::CreateInstance(
    AdsClient* ads_client) {
  return new AdsImpl(ads_client);
}

bool Ads::IsSupportedRegion(
    const std::string& locale) {
  auto region = GetRegion(locale);

  for (const auto& schema_version : kSupportedRegions) {
    int version = schema_version.first;
    std::map<std::string, bool> regions = schema_version.second;

    auto it = schema_version.find(region);
    if (it != schema_version.end()) {
      return true;
    }
  }

  return false;
}

bool Ads::IsNewlySupportedRegion(
    const std::string& locale) {
  const auto region = GetRegion(locale);

  for (const auto& schema_version : kSupportedRegions) {
    const int version = schema_version.first;
    if (version <= current_version) {
      continue;
    }

    const std::map<std::string, bool> regions = schema_version.second;

    const auto it = schema_version.find(region);
    if (it != schema_version.end()) {
      return true;
    }
  }

  return false;
}

// DEPRECATED, use Ads::IsSupportedRegion and Ads::IsNewlySupportedRegion
// instead, will be removed at some point in the future
std::string Ads::GetRegion(
    const std::string& locale) {
  return helper::Locale::GetRegionCode(locale);
}

}  // namespace ads
