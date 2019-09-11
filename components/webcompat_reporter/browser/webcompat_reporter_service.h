/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_WEBCOMPAT_REPORTER_BROWSER_WEBCOMPAT_REPORTER_SERVICE_H_
#define BRAVE_COMPONENTS_WEBCOMPAT_REPORTER_BROWSER_WEBCOMPAT_REPORTER_SERVICE_H_

#include <string>

#include "base/macros.h"
#include "base/memory/ref_counted.h"

namespace network {
class SharedURLLoaderFactory;
class SimpleURLLoader;
} // namespace network

namespace brave {

class WebcompatReporterService {
 public:
  explicit WebcompatReporterService(scoped_refptr<network::SharedURLLoaderFactory>);
  ~WebcompatReporterService();

  void SubmitReport(std::string report_domain);

 private:
  std::unique_ptr<network::SimpleURLLoader> simple_url_loader_;
  scoped_refptr<network::SharedURLLoaderFactory> shared_url_loader_factory_;
  void CreateAndStartURLLoader(const std::string& upload_url, const std::string& post_data);
  void OnSimpleURLLoaderComplete(std::unique_ptr<std::string> response_body);

  DISALLOW_COPY_AND_ASSIGN(WebcompatReporterService);
};

} // namespace brave

#endif // BRAVE_COMPONENTS_WEBCOMPAT_REPORTER_BROWSER_WEBCOMPAT_REPORTER_SERVICE_H_
