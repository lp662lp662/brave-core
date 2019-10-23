/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/version_info.h"

namespace {

const char kBraveBuildInstructionsUrl[] = "https://github.com/brave/brave-browser/wiki";
const char kBraveLicenseUrl[] = "https://mozilla.org/MPL/2.0/";
const char kBraveReleaseTagPrefix[] = "https://github.com/brave/brave-browser/releases/tag/v";

}

#define BRAVE_LICENSING_INFO base::ASCIIToUTF16(kBraveLicenseUrl), \
    base::ASCIIToUTF16(kBraveBuildInstructionsUrl), \
    base::ASCIIToUTF16(kBraveReleaseTagPrefix) + \
        base::UTF8ToUTF16( \
            version_info::GetBraveVersionWithoutChromiumMajorVersion()),

#define GetVersionNumber GetBraveVersionNumberForDisplay
#include "../../../../../../../chrome/browser/ui/webui/settings/about_handler.cc"
#undef GetVersionNumber
