// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_EXO_WM_HELPER_H_
#define COMPONENTS_EXO_WM_HELPER_H_

#include <vector>

#include "base/macros.h"
#include "base/observer_list.h"
#include "ui/aura/client/drag_drop_delegate.h"
#include "ui/aura/env.h"
#include "ui/base/cursor/cursor.h"
#include "ui/compositor/compositor_vsync_manager.h"

namespace aura {
class env;
class Window;
namespace client {
class CursorClient;
class FocusChangeObserver;
}  // namespace client
}  // namespace aura

namespace wm {
class ActivationChangeObserver;
}

namespace display {
class ManagedDisplayInfo;
}

namespace ui {
class EventHandler;
class DropTargetEvent;
}  // namespace ui

namespace wm {
class ActivationChangeObserver;
}

namespace exo {

// Helper interface for accessing WindowManager related features.
class WMHelper : public aura::client::DragDropDelegate {
 public:
  class DragDropObserver {
   public:
    virtual void OnDragEntered(const ui::DropTargetEvent& event) = 0;
    virtual int OnDragUpdated(const ui::DropTargetEvent& event) = 0;
    virtual void OnDragExited() = 0;
    virtual int OnPerformDrop(const ui::DropTargetEvent& event) = 0;

   protected:
    virtual ~DragDropObserver() {}
  };

  WMHelper();
  ~WMHelper() override;

  static void SetInstance(WMHelper* helper);
  static WMHelper* GetInstance();
  static bool HasInstance();

  virtual aura::Env* env() = 0;

  virtual void AddActivationObserver(
      wm::ActivationChangeObserver* observer) = 0;
  virtual void RemoveActivationObserver(
      wm::ActivationChangeObserver* observer) = 0;
  virtual void AddFocusObserver(
      aura::client::FocusChangeObserver* observer) = 0;
  virtual void RemoveFocusObserver(
      aura::client::FocusChangeObserver* observer) = 0;

  virtual void AddDragDropObserver(DragDropObserver* observer) = 0;
  virtual void RemoveDragDropObserver(DragDropObserver* observer) = 0;
  virtual void SetDragDropDelegate(aura::Window*) = 0;
  virtual void ResetDragDropDelegate(aura::Window*) = 0;
  virtual void AddVSyncObserver(
      ui::CompositorVSyncManager::Observer* observer) = 0;
  virtual void RemoveVSyncObserver(
      ui::CompositorVSyncManager::Observer* observer) = 0;

  virtual const display::ManagedDisplayInfo& GetDisplayInfo(
      int64_t display_id) const = 0;
  virtual const std::vector<uint8_t>& GetDisplayIdentificationData(
      int64_t display_id) const = 0;

  virtual aura::Window* GetPrimaryDisplayContainer(int container_id) = 0;
  virtual aura::Window* GetActiveWindow() const = 0;
  virtual aura::Window* GetFocusedWindow() const = 0;
  virtual aura::Window* GetRootWindowForNewWindows() const = 0;
  virtual aura::client::CursorClient* GetCursorClient() = 0;
  virtual void AddPreTargetHandler(ui::EventHandler* handler) = 0;
  virtual void PrependPreTargetHandler(ui::EventHandler* handler) = 0;
  virtual void RemovePreTargetHandler(ui::EventHandler* handler) = 0;
  virtual void AddPostTargetHandler(ui::EventHandler* handler) = 0;
  virtual void RemovePostTargetHandler(ui::EventHandler* handler) = 0;
  virtual bool IsTabletModeWindowManagerEnabled() const = 0;
  virtual double GetDefaultDeviceScaleFactor() const = 0;

  // Overridden from aura::client::DragDropDelegate:
  void OnDragEntered(const ui::DropTargetEvent& event) override = 0;
  int OnDragUpdated(const ui::DropTargetEvent& event) override = 0;
  void OnDragExited() override = 0;
  int OnPerformDrop(const ui::DropTargetEvent& event) override = 0;

 protected:
  DISALLOW_COPY_AND_ASSIGN(WMHelper);
};

}  // namespace exo

#endif  // COMPONENTS_EXO_WM_HELPER_H_