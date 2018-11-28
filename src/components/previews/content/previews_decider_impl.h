// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_PREVIEWS_CONTENT_PREVIEWS_DECIDER_IMPL_H_
#define COMPONENTS_PREVIEWS_CONTENT_PREVIEWS_DECIDER_IMPL_H_

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/sequence_checker.h"
#include "base/single_thread_task_runner.h"
#include "base/time/time.h"
#include "components/blacklist/opt_out_blacklist/opt_out_blacklist_data.h"
#include "components/blacklist/opt_out_blacklist/opt_out_blacklist_delegate.h"
#include "components/previews/content/previews_optimization_guide.h"
#include "components/previews/core/previews_black_list.h"
#include "components/previews/core/previews_decider.h"
#include "components/previews/core/previews_experiments.h"
#include "components/previews/core/previews_logger.h"
#include "net/nqe/effective_connection_type.h"
#include "url/gurl.h"

namespace base {
class Clock;
}

namespace blacklist {
class OptOutStore;
}

namespace previews {
class PreviewsUIService;

typedef base::RepeatingCallback<bool(PreviewsType)> PreviewsIsEnabledCallback;

// A class to manage the IO portion of inter-thread communication between
// previews/ objects. Created on the UI thread, but used only on the IO thread
// after initialization.
class PreviewsDeciderImpl : public PreviewsDecider,
                            public blacklist::OptOutBlacklistDelegate {
 public:
  explicit PreviewsDeciderImpl(base::Clock* clock);
  ~PreviewsDeciderImpl() override;

  // blacklist::OptOutBlacklistDelegate:
  void OnNewBlacklistedHost(const std::string& host, base::Time time) override;
  void OnUserBlacklistedStatusChange(bool blacklisted) override;
  void OnBlacklistCleared(base::Time time) override;

  // Initializes the blacklist and and stores the passed in members.
  // |previews_ui_service| owns |this|, and shares the same lifetime.
  virtual void Initialize(
      PreviewsUIService* previews_ui_service,
      std::unique_ptr<blacklist::OptOutStore> previews_opt_out_store,
      std::unique_ptr<PreviewsOptimizationGuide> previews_opt_guide,
      const PreviewsIsEnabledCallback& is_enabled_callback,
      blacklist::BlacklistData::AllowedTypesAndVersions allowed_previews);

  // Adds log message of the navigation asynchronously.
  void LogPreviewNavigation(const GURL& url,
                            bool opt_out,
                            PreviewsType type,
                            base::Time time,
                            uint64_t page_id) const;

  // Adds a log message for the preview decision made (|reason|) asynchronously.
  // |passed_reasons| is a collection of reason codes that correspond to
  // eligibility checks that were satisfied prior to determining |reason| and
  // so the opposite of these |passed_reasons| codes was true. The method
  // takes ownership of |passed_reasons|. |page_id| is generated by
  // PreviewsDeciderImpl, and used to group decisions into groups on the page,
  // messages that don't need to be grouped can pass in 0 as page_id.
  void LogPreviewDecisionMade(
      PreviewsEligibilityReason reason,
      const GURL& url,
      base::Time time,
      PreviewsType type,
      std::vector<PreviewsEligibilityReason>&& passed_reasons,
      uint64_t page_id) const;

  // Adds a navigation to |url| to the black list with result |opt_out|.
  void AddPreviewNavigation(const GURL& url,
                            bool opt_out,
                            PreviewsType type,
                            uint64_t page_id);

  // Clears the history of the black list between |begin_time| and |end_time|,
  // both inclusive.
  void ClearBlackList(base::Time begin_time, base::Time end_time);

  // Change the status of whether to ignore the decisions made by
  // PreviewsBlackList to |ignored|. Virtualized in testing.
  virtual void SetIgnorePreviewsBlacklistDecision(bool ignored);

  // The previews black list that decides whether a navigation can use previews.
  PreviewsBlackList* black_list() const { return previews_black_list_.get(); }

  // PreviewsDecider implementation:
  bool ShouldAllowPreviewAtNavigationStart(PreviewsUserData* previews_data,
                                           const GURL& url,
                                           bool is_reload,
                                           PreviewsType type) const override;
  bool ShouldCommitPreview(PreviewsUserData* previews_data,
                           const GURL& committed_url,
                           PreviewsType type) const override;

  // Set whether to ignore the long term blacklist rules for server previews.
  void SetIgnoreLongTermBlackListForServerPreviews(
      bool ignore_long_term_blacklist_for_server_previews);

  void LoadResourceHints(const GURL& url) override;

  void LogHintCacheMatch(const GURL& url, bool is_committed) const override;

  // Generates a page ID that is guaranteed to be unique from any other page ID
  // generated in this browser session. Also, guaranteed to be non-zero.
  uint64_t GeneratePageId();

  void SetEffectiveConnectionType(
      net::EffectiveConnectionType effective_connection_type);

 protected:
  // Posts a task to deliver the resource patterns to the PreviewsUIService.
  void OnResourceLoadingHints(
      const GURL& document_gurl,
      const std::vector<std::string>& patterns_to_block);

  // Sets a blacklist for testing.
  void SetPreviewsBlacklistForTesting(
      std::unique_ptr<PreviewsBlackList> previews_back_list);

 private:
  // Returns whether the preview |type| should be considered for |url|.
  // This is an initial check on the preview |type| being enabled and the
  // |url| not being a local URL.
  bool ShouldConsiderPreview(PreviewsType type,
                             const GURL& url,
                             PreviewsUserData* previews_data) const;

  // Determines the eligibility of the preview |type| for |url|.
  PreviewsEligibilityReason DeterminePreviewEligibility(
      PreviewsUserData* previews_data,
      const GURL& url,
      bool is_reload,
      PreviewsType type,
      bool is_drp_server_preview,
      std::vector<PreviewsEligibilityReason>* passed_reasons) const;

  // Whether the preview |type| should be allowed to be considered for |url|
  // subject to any server provided optimization hints. This is meant for
  // checking the initial navigation URL. Returns ALLOWED if no reason found
  // to deny the preview for consideration.
  PreviewsEligibilityReason ShouldAllowPreviewPerOptimizationHints(
      PreviewsUserData* previews_data,
      const GURL& url,
      PreviewsType type,
      std::vector<PreviewsEligibilityReason>* passed_reasons) const;

  // Whether |url| is allowed for |type| according to server provided
  // optimization hints, if available. This is meant for checking the committed
  // navigation URL against any specific hint details.
  PreviewsEligibilityReason ShouldCommitPreviewPerOptimizationHints(
      PreviewsUserData* previews_data,
      const GURL& url,
      PreviewsType type,
      std::vector<PreviewsEligibilityReason>* passed_reasons) const;

  // The UI service object owns |this| and exists as long as |this| does.
  PreviewsUIService* previews_ui_service_;

  std::unique_ptr<PreviewsBlackList> previews_black_list_;

  // Only used when the blacklist has been disabled to allow "Show Original" to
  // function as expected. The time of the most recent opt out event.
  base::Time last_opt_out_time_;

  // Holds optimization guidance from the server.
  std::unique_ptr<PreviewsOptimizationGuide> previews_opt_guide_;

  // Whether the decisions made by PreviewsBlackList should be ignored or not.
  // This can be changed by chrome://interventions-internals to test/debug the
  // behavior of Previews decisions.
  // This is related to a test flag and should only be true when the user has
  // set it in flags. See previews::IsPreviewsBlacklistIgnoredViaFlag.
  bool blacklist_ignored_;

  // Whether to ignore the blacklist for server previews.
  bool ignore_long_term_blacklist_for_server_previews_ = false;

  // The estimate of how slow a user's connection is. Used for triggering
  // Previews.
  net::EffectiveConnectionType effective_connection_type_ =
      net::EffectiveConnectionType::EFFECTIVE_CONNECTION_TYPE_UNKNOWN;

  base::Clock* clock_;

  // Whether the preview is enabled. Valid after Initialize() is called.
  PreviewsIsEnabledCallback is_enabled_callback_;

  uint64_t page_id_;

  SEQUENCE_CHECKER(sequence_checker_);

  base::WeakPtrFactory<PreviewsDeciderImpl> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(PreviewsDeciderImpl);
};

}  // namespace previews

#endif  // COMPONENTS_PREVIEWS_CONTENT_PREVIEWS_DECIDER_IMPL_H_
