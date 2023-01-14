#pragma once

#include <zukou.h>

#include <wayland-client.h>
#include <zwin-client-protocol.h>
#include <zwin-gles-v32-client-protocol.h>
#include <zwin-shell-client-protocol.h>

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
  inline zwn_compositor *compositor();
  inline zwn_seat *seat();
  inline zwn_gles_v32 *gles_v32();
  inline zwn_shell *shell();
  inline zwn_shm *shm();

 private:
  static const wl_registry_listener registry_listener_;

  static void HandleGlobal(void *data, struct wl_registry *wl_registry,
      uint32_t name, const char *interface, uint32_t version);
  static void HandleGlobalRemove(
      void *data, struct wl_registry *wl_registry, uint32_t name);

  static const struct zwn_seat_listener seat_listener_;
  static void HandleSeatCapabilities(
      void *data, struct zwn_seat *zwn_seat, uint32_t capabilities);

  static const struct zwn_ray_listener ray_listener_;
  static void HandleRayEnter(void *data, struct zwn_ray *zwn_ray,
      uint32_t serial, struct zwn_virtual_object *virtual_object,
      struct wl_array *origin, struct wl_array *direction);
  static void HandleRayLeave(void *data, struct zwn_ray *zwn_ray,
      uint32_t serial, struct zwn_virtual_object *virtual_object);
  static void HandleRayMotion(void *data, struct zwn_ray *zwn_ray,
      uint32_t time, struct wl_array *origin, struct wl_array *direction);
  static void HandleRayButton(void *data, struct zwn_ray *zwn_ray,
      uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
  static void HandleRayAxis(void *data, struct zwn_ray *zwn_ray, uint32_t time,
      uint32_t axis, wl_fixed_t value);
  static void HandleRayAxisSource(
      void *data, struct zwn_ray *zwn_ray, uint32_t axis_source);
  static void HandleRayAxisStop(
      void *data, struct zwn_ray *zwn_ray, uint32_t time, uint32_t axis);
  static void HandleRayAxisDiscrete(
      void *data, struct zwn_ray *zwn_ray, uint32_t axis, int32_t discrete);
  static void HandleRayFrame(void *data, struct zwn_ray *zwn_ray);

  bool TryConnect(const char *socket /*nonnull*/);

  void Poll();

  ISystemDelegate *delegate_;  // nullable

  wl_display *display_ = nullptr;
  wl_registry *registry_ = nullptr;

  zwn_compositor *zwn_compositor_ = nullptr;
  zwn_seat *zwn_seat_ = nullptr;
  zwn_gles_v32 *zwn_gles_v32_ = nullptr;
  zwn_shell *zwn_shell_ = nullptr;
  zwn_shm *zwn_shm_ = nullptr;

  zwn_ray *zwn_ray_ = nullptr;  // nullable

  RayAxisEvent ray_axis_event_;

  Loop loop_;
  std::unique_ptr<EventSource> event_source_;
};

Loop *
System::Impl::loop()
{
  return &loop_;
}

inline zwn_compositor *
System::Impl::compositor()
{
  return zwn_compositor_;
}

inline zwn_seat *
System::Impl::seat()
{
  return zwn_seat_;
}

inline zwn_gles_v32 *
System::Impl::gles_v32()
{
  return zwn_gles_v32_;
}

inline zwn_shell *
System::Impl::shell()
{
  return zwn_shell_;
}

inline zwn_shm *
System::Impl::shm()
{
  return zwn_shm_;
}

}  // namespace zukou
