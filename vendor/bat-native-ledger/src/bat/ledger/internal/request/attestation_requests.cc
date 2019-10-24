/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>

#include "base/strings/stringprintf.h"
#include "bat/ledger/internal/static_values.h"
#include "bat/ledger/internal/request/attestation_requests.h"
#include "bat/ledger/internal/request/request_util.h"

namespace braveledger_request_util {

std::string GetStartAttestationDesktopUrl() {
  // return BuildUrl("/v1/captchas", PREFIX_V1, ServerTypes::kPromotion);
  // TODO currently server is not at the top address yet
  return "http://reputation-alb-1044480833.us-west-2.elb.amazonaws.com/"
         "v1/captchas";
}
std::string GetStartAttestationAndroidUrl() {
  // return BuildUrl("/v2/attestations/safetynet", PREFIX_V1, ServerTypes::kPromotion);
  // TODO currently server is not at the top address yet
  return "http://reputation-alb-1044480833.us-west-2.elb.amazonaws.com/"
         "/v2/attestations/safetynet";
}

std::string GetCaptchaUrl(const std::string captcha_id) {
  const std::string path = base::StringPrintf(
      "/v1/captchas/%s.png",
      captcha_id.c_str());
  // return BuildUrl("/v2/attestations/safetynet", PREFIX_V1, ServerTypes::kPromotion);
  // TODO currently server is not at the top address yet
  return "http://reputation-alb-1044480833.us-west-2.elb.amazonaws.com" + path;
}

std::string GetClaimAttestationDesktopUrl(const std::string captcha_id) {
  const std::string path = base::StringPrintf(
      "/v1/captchas/%s",
      captcha_id.c_str());
  // return BuildUrl("/v2/attestations/safetynet", PREFIX_V1, ServerTypes::kPromotion);
  // TODO currently server is not at the top address yet
  return "http://reputation-alb-1044480833.us-west-2.elb.amazonaws.com" + path;
}

}  // namespace braveledger_request_util
