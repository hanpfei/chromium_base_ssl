// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_DOM_DISTILLER_CONTENT_RENDERER_DISTILLABILITY_AGENT_H_
#define COMPONENTS_DOM_DISTILLER_CONTENT_RENDERER_DISTILLABILITY_AGENT_H_

#include "content/public/renderer/render_frame_observer.h"

namespace dom_distiller {

// DistillabilityAgent returns distillability result to DistillabilityDriver.
class DistillabilityAgent : public content::RenderFrameObserver {
 public:
  DistillabilityAgent(content::RenderFrame* render_frame);
  ~DistillabilityAgent() override;

  // content::RenderFrameObserver:
  void DidMeaningfulLayout(blink::WebMeaningfulLayout layout_type) override;
  void OnDestruct() override;
};

}  // namespace dom_distiller

#endif  // COMPONENTS_DOM_DISTILLER_CONTENT_RENDERER_DISTILLABILITY_AGENT_H_
