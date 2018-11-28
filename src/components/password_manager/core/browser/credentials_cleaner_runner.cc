// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/password_manager/core/browser/credentials_cleaner_runner.h"

#include <utility>

namespace password_manager {

CredentialsCleanerRunner::CredentialsCleanerRunner() = default;

void CredentialsCleanerRunner::AddCleaningTask(
    std::unique_ptr<CredentialsCleaner> cleaning_task) {
  cleaning_tasks_queue_.push(std::move(cleaning_task));
}

void CredentialsCleanerRunner::StartCleaning() {
  DCHECK(!cleaning_started_);
  cleaning_started_ = true;
  StartCleaningTask();
}

void CredentialsCleanerRunner::CleaningCompleted() {
  // Delete the cleaner object, because the cleaning is finished.
  DCHECK(!cleaning_tasks_queue_.empty());
  cleaning_tasks_queue_.pop();
  StartCleaningTask();
}

CredentialsCleanerRunner::~CredentialsCleanerRunner() = default;

void CredentialsCleanerRunner::StartCleaningTask() {
  if (cleaning_tasks_queue_.empty()) {
    // Delete the runner if no more clean-up is scheduled.
    delete this;
    return;
  }
  cleaning_tasks_queue_.front()->StartCleaning(this);
}

}  // namespace password_manager