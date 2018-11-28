// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/metrics/call_stack_profile_collector.h"

#include <memory>
#include <utility>

#include "components/metrics/call_stack_profile_encoding.h"
#include "components/metrics/call_stack_profile_metrics_provider.h"
#include "mojo/public/cpp/bindings/strong_binding.h"

namespace metrics {

CallStackProfileCollector::CallStackProfileCollector() = default;

CallStackProfileCollector::~CallStackProfileCollector() = default;

// static
void CallStackProfileCollector::Create(
    mojom::CallStackProfileCollectorRequest request) {
  mojo::MakeStrongBinding(std::make_unique<CallStackProfileCollector>(),
                          std::move(request));
}

void CallStackProfileCollector::Collect(base::TimeTicks start_timestamp,
                                        mojom::SampledProfilePtr profile) {
  CallStackProfileMetricsProvider::ReceiveSerializedProfile(
      start_timestamp, std::move(profile->contents));
}

}  // namespace metrics
