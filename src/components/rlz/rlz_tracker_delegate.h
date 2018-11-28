// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_RLZ_RLZ_TRACKER_DELEGATE_H_
#define COMPONENTS_RLZ_RLZ_TRACKER_DELEGATE_H_

#include <string>

#include "base/callback_forward.h"
#include "base/macros.h"
#include "base/strings/string16.h"

namespace network {
class SharedURLLoaderFactory;
}  // namespace network

namespace rlz {

// RLZTrackerDelegate is an abstract interface that provides access to embedder
// specific singletons or gives information about the embedder environment.
class RLZTrackerDelegate {
 public:
  RLZTrackerDelegate() {}
  virtual ~RLZTrackerDelegate() {}

  // Invoked during RLZTracker cleanup, to request the cleanup of the delegate.
  virtual void Cleanup() = 0;

  // Returns whether the current thread is the UI thread.
  virtual bool IsOnUIThread() = 0;

  // Returns the SharedURLLoaderFactory to use for network connections.
  virtual scoped_refptr<network::SharedURLLoaderFactory>
  GetURLLoaderFactory() = 0;

  // Returns the brand code for the installation of Chrome in |brand| and a
  // boolean indicating whether the operation was a success or not.
  virtual bool GetBrand(std::string* brand) = 0;

  // Returns whether |brand| is an organic brand.
  virtual bool IsBrandOrganic(const std::string& brand) = 0;

  // Returns the reactivation brand code for Chrome in |brand| and a boolean
  // indicating whether the operation was a success or not.
  virtual bool GetReactivationBrand(std::string* brand) = 0;

  // Returns true if RLZTracker should ignore initial delay for testing.
  virtual bool ShouldEnableZeroDelayForTesting() = 0;

  // Returns the installation language in |language| and a boolean indicating
  // whether the operation was a success or not.
  virtual bool GetLanguage(base::string16* language) = 0;

  // Returns the referral code in |referral| and a boolean indicating whether
  // the operation was a success or not. Deprecated.
  virtual bool GetReferral(base::string16* referral) = 0;

  // Clears the referral code. Deprecated.
  virtual bool ClearReferral() = 0;

  // Registers |callback| to be invoked the next time the user perform a search
  // using Google search engine via the omnibox. Callback will invoked at most
  // once.
  virtual void SetOmniboxSearchCallback(const base::Closure& callback) = 0;

  // Registers |callback| to be invoked the next time the user perform a search
  // using Google search engine via the homepage. Callback will invoked at most
  // once.
  virtual void SetHomepageSearchCallback(const base::Closure& callback) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(RLZTrackerDelegate);
};

}  // namespace rlz

#endif  // COMPONENTS_RLZ_RLZ_TRACKER_DELEGATE_H_
