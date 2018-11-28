// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"
#include "components/autofill/core/common/autofill_switches.h"

namespace autofill {
namespace switches {

// Override the default autofill server URL with "scheme://host[:port]/prefix/".
const char kAutofillServerURL[] = "autofill-server-url";

// The randomized encoding type to use when sending metadata uploads. The
// value of the parameter must be one of the valid integer values of the
// AutofillRandomizedValue_EncodingType enum.
extern const char kAutofillMetadataUploadEncoding[] =
    "autofill-metadata-upload-encoding";

// Force hiding the local save checkbox in the autofill dialog box for getting
// the full credit card number for a wallet card. The card will never be stored
// locally.
const char kDisableOfferStoreUnmaskedWalletCards[] =
    "disable-offer-store-unmasked-wallet-cards";

// Force showing the local save checkbox in the autofill dialog box for getting
// the full credit card number for a wallet card.
const char kEnableOfferStoreUnmaskedWalletCards[] =
    "enable-offer-store-unmasked-wallet-cards";

// Enables suggestions with substring matching instead of prefix matching.
const char kEnableSuggestionsWithSubstringMatch[] =
    "enable-suggestions-with-substring-match";

// Ignores autocomplete="off" for Autofill data (profiles + credit cards).
const char kIgnoreAutocompleteOffForAutofill[] =
    "ignore-autocomplete-off-autofill";

// Annotates forms with Autofill field type predictions.
const char kShowAutofillTypePredictions[]   = "show-autofill-type-predictions";

// Annotates forms and fields with Autofill signatures.
const char kShowAutofillSignatures[] = "show-autofill-signatures";

// Use the sandbox Online Wallet service URL (for developer testing).
const char kWalletServiceUseSandbox[]       = "wallet-service-use-sandbox";

}  // namespace switches
}  // namespace autofill
