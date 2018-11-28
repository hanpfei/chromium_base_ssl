// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_UNIFIED_CONSENT_UNIFIED_CONSENT_SERVICE_H_
#define COMPONENTS_UNIFIED_CONSENT_UNIFIED_CONSENT_SERVICE_H_

#include <memory>

#include "base/macros.h"
#include "base/observer_list.h"
#include "components/keyed_service/core/keyed_service.h"
#include "components/sync/base/model_type.h"
#include "components/sync/driver/sync_service_observer.h"
#include "components/unified_consent/unified_consent_metrics.h"
#include "components/unified_consent/unified_consent_service_client.h"
#include "services/identity/public/cpp/identity_manager.h"

namespace user_prefs {
class PrefRegistrySyncable;
}

class PrefService;

namespace syncer {
class SyncService;
}

namespace unified_consent {

using Service = UnifiedConsentServiceClient::Service;
using ServiceState = UnifiedConsentServiceClient::ServiceState;

enum class MigrationState : int {
  kNotInitialized = 0,
  kInProgressWaitForSyncInit = 1,
  // Reserve space for other kInProgress* entries to be added here.
  kCompleted = 10,
};

// A browser-context keyed service that is used to manage the user consent
// when UnifiedConsent feature is enabled.
class UnifiedConsentService : public KeyedService,
                              public identity::IdentityManager::Observer,
                              public syncer::SyncServiceObserver {
 public:
  UnifiedConsentService(
      std::unique_ptr<UnifiedConsentServiceClient> service_client,
      PrefService* pref_service,
      identity::IdentityManager* identity_manager,
      syncer::SyncService* sync_service);
  ~UnifiedConsentService() override;

  // Register the prefs used by this UnifiedConsentService.
  static void RegisterPrefs(user_prefs::PrefRegistrySyncable* registry);

  // Rolls back changes made during migration. This method does nothing if the
  // user hasn't migrated to unified consent yet.
  static void RollbackIfNeeded(PrefService* user_pref_service,
                               syncer::SyncService* sync_service,
                               UnifiedConsentServiceClient* service_client);

  // Enables all Google services tied to unified consent.
  // Note: Sync has to be enabled through the SyncService. It is *not* enabled
  // in this method.
  void EnableGoogleServices();

  // Returns true if all criteria is met to show the consent bump.
  bool ShouldShowConsentBump();
  // Marks the consent bump as shown. Any future calls to
  // |ShouldShowConsentBump| are guaranteed to return false.
  void MarkConsentBumpShown();
  // Records the consent bump suppress reason and updates the state whether the
  // consent bump should be shown. Note: In some cases, e.g. sync paused,
  // |ShouldShowConsentBump| will still return true.
  void RecordConsentBumpSuppressReason(
      metrics::ConsentBumpSuppressReason suppress_reason);

  // KeyedService:
  void Shutdown() override;

  // IdentityManager::Observer:
  void OnPrimaryAccountCleared(
      const AccountInfo& previous_primary_account_info) override;

 private:
  friend class UnifiedConsentServiceTest;

  // syncer::SyncServiceObserver:
  void OnStateChanged(syncer::SyncService* sync) override;

  // Migration helpers.
  MigrationState GetMigrationState();
  void SetMigrationState(MigrationState migration_state);
  // Called when the unified consent service is created. This sets the
  // |kShouldShowUnifiedConsentBump| pref to true if the user is eligible and
  // calls |UpdateSettingsForMigration| at the end.
  void MigrateProfileToUnifiedConsent();
  // Updates the settings preferences for the migration when the sync engine is
  // initialized. When it is not, this function will be called again from
  // |OnStateChanged| when the sync engine is initialized.
  void UpdateSettingsForMigration();

  // Checks if all on-by-default non-personalized services are on.
  bool AreAllOnByDefaultPrivacySettingsOn();

  // Records a sample for each bucket enabled by the user (except kNone).
  // kNone is recorded when none of the other buckets are recorded.
  void RecordSettingsHistogram();

  // This method is called on startup to check the eligibility criteria for
  // showing the consent bump. The check is only done when the profile was
  // eligible before. If the user is not eligible anymore, the
  // kShouldShowUnifiedConsentBump pref is set to false.
  void CheckConsentBumpEligibility();

  std::unique_ptr<UnifiedConsentServiceClient> service_client_;
  PrefService* pref_service_;
  identity::IdentityManager* identity_manager_;
  syncer::SyncService* sync_service_;

  DISALLOW_COPY_AND_ASSIGN(UnifiedConsentService);
};

}  // namespace unified_consent

#endif  // COMPONENTS_UNIFIED_CONSENT_UNIFIED_CONSENT_SERVICE_H_
