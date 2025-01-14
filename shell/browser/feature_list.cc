// Copyright (c) 2019 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "electron/shell/browser/feature_list.h"

#include <string>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/feature_list.h"
#include "base/metrics/field_trial.h"
#include "components/spellcheck/common/spellcheck_features.h"
#include "content/public/common/content_features.h"
#include "electron/buildflags/buildflags.h"
#include "media/base/media_switches.h"
#include "net/base/features.h"
#include "services/network/public/cpp/features.h"
#include "third_party/blink/renderer/platform/scheduler/common/features.h"  // nogncheck

#if BUILDFLAG(IS_MAC)
#include "device/base/features.h"  // nogncheck
#endif

namespace electron {

void InitializeFeatureList() {
  auto* cmd_line = base::CommandLine::ForCurrentProcess();
  auto enable_features =
      cmd_line->GetSwitchValueASCII(::switches::kEnableFeatures);
  auto disable_features =
      cmd_line->GetSwitchValueASCII(::switches::kDisableFeatures);
  // Disable creation of spare renderer process with site-per-process mode,
  // it interferes with our process preference tracking for non sandboxed mode.
  // Can be reenabled when our site instance policy is aligned with chromium
  // when node integration is enabled.
  disable_features +=
      std::string(",") + features::kSpareRendererForSitePerProcess.name;

  // Microtask queues per WindowAgent causes issues with Node, so disable
  // this feature for now.  See
  // https://chromium-review.googlesource.com/c/chromium/src/+/4003663
  disable_features +=
      std::string(",") + blink::scheduler::kMicrotaskQueuePerWindowAgent.name;

#if BUILDFLAG(IS_MAC)
  // Needed for WebUSB implementation
  enable_features += std::string(",") + device::kNewUsbBackend.name;
#endif

#if !BUILDFLAG(ENABLE_PICTURE_IN_PICTURE)
  disable_features += std::string(",") + media::kPictureInPicture.name;
#endif

#if BUILDFLAG(IS_WIN)
  // Disable async spellchecker suggestions for Windows, which causes
  // an empty suggestions list to be returned
  disable_features +=
      std::string(",") + spellcheck::kWinRetrieveSuggestionsOnlyOnDemand.name;
#endif
  base::FeatureList::InitializeInstance(enable_features, disable_features);
}

void InitializeFieldTrials() {
  auto* cmd_line = base::CommandLine::ForCurrentProcess();
  auto force_fieldtrials =
      cmd_line->GetSwitchValueASCII(::switches::kForceFieldTrials);

  base::FieldTrialList::CreateTrialsFromString(force_fieldtrials);
}

}  // namespace electron
