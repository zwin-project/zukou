#pragma once

#include <zukou.h>

#include <wayland-client.h>
#include <zen-space-client-protocol.h>
#include <zigen-client-protocol.h>
#include <zigen-gles-v32-client-protocol.h>
#include <zigen-shell-client-protocol.h>

#include "loop.h"

namespace zukou {

class System::Impl
{
  friend class System;

 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(ISystemDelegate *delegate);
  ~Impl();

  inline Loop *loop();
  inline zgn_compositor *compositor();
  inline zgn_seat *seat();
  inline zgn_gles_v32 *gles_v32();
  inline zgn_shell *shell();
  inline zgn_shm *shm();

  /** nullable */
  inline zen_space_manager *space_manager();

 private:
  static const wl_registry_listener registry_listener_;

  static void HandleGlobal(void *data, struct wl_registry *wl_registry,
      uint32_t name, const char *interface, uint32_t version);
  static void HandleGlobalRemove(
      void *data, struct wl_registry *wl_registry, uint32_t name);

  static const struct zgn_seat_listener seat_listener_;
  static void HandleSeatCapabilities(
      void *data, struct zgn_seat *zgn_seat, uint32_t capabilities);

  static const struct zgn_ray_listener ray_listener_;
  static void HandleRayEnter(void *data, struct zgn_ray *zgn_ray,
      uint32_t serial, struct zgn_virtual_object *virtual_object,
      struct wl_array *origin, struct wl_array *direction);
  static void HandleRayLeave(void *data, struct zgn_ray *zgn_ray,
      uint32_t serial, struct zgn_virtual_object *virtual_object);
  static void HandleRayMotion(void *data, struct zgn_ray *zgn_ray,
      uint32_t time, struct wl_array *origin, struct wl_array *direction);
  static void HandleRayButton(void *data, struct zgn_ray *zgn_ray,
      uint32_t serial, uint32_t time, uint32_t button, uint32_t state);

  bool TryConnect(const char *socket /*nonnull*/);

  void Poll();

  ISystemDelegate *delegate_;  // nullable

  wl_display *display_ = nullptr;
  wl_registry *registry_ = nullptr;

  zgn_compositor *zgn_compositor_ = nullptr;
  zgn_seat *zgn_seat_ = nullptr;
  zgn_gles_v32 *zgn_gles_v32_ = nullptr;
  zgn_shell *zgn_shell_ = nullptr;
  zgn_shm *zgn_shm_ = nullptr;
  zen_space_manager *zen_space_manager_ = nullptr;  // nullable

  zgn_ray *zgn_ray_ = nullptr;  // nullable

  Loop loop_;
  std::unique_ptr<EventSource> event_source_;
};

Loop *
System::Impl::loop()
{
  return &loop_;
}

inline zgn_compositor *
System::Impl::compositor()
{
  return zgn_compositor_;
}

inline zgn_seat *
System::Impl::seat()
{
  return zgn_seat_;
}

inline zgn_gles_v32 *
System::Impl::gles_v32()
{
  return zgn_gles_v32_;
}

inline zgn_shell *
System::Impl::shell()
{
  return zgn_shell_;
}

inline zgn_shm *
System::Impl::shm()
{
  return zgn_shm_;
}

inline zen_space_manager *
System::Impl::space_manager()
{
  return zen_space_manager_;
}

}  // namespace zukou
