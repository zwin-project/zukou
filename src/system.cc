#include "system.h"

#include <errno.h>

#include <cstdlib>
#include <cstring>

#include "common.h"
#include "convert.h"
#include "logger.h"
#include "loop.h"

namespace zukou {

const zwn_ray_listener System::Impl::ray_listener_ = {
    System::Impl::HandleRayEnter,
    System::Impl::HandleRayLeave,
    System::Impl::HandleRayMotion,
    System::Impl::HandleRayButton,
    System::Impl::HandleRayAxis,
    System::Impl::HandleRayAxisSource,
    System::Impl::HandleRayAxisStop,
    System::Impl::HandleRayAxisDiscrete,
    System::Impl::HandleRayFrame,
};

void
System::Impl::HandleRayEnter(void *data, struct zwn_ray * /*zwn_ray*/,
    uint32_t serial, struct zwn_virtual_object *virtual_object_proxy,
    struct wl_array *origin_wl_array, struct wl_array *direction_wl_array)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  auto virtual_object = static_cast<VirtualObject *>(
      wl_proxy_get_user_data((wl_proxy *)virtual_object_proxy));
  glm::vec3 origin, direction;
  to_vec3(origin_wl_array, origin);
  to_vec3(direction_wl_array, direction);

  self->delegate_->RayEnter(serial, virtual_object, origin, direction);
}

void
System::Impl::HandleRayLeave(void *data, struct zwn_ray * /*zwn_ray*/,
    uint32_t serial, struct zwn_virtual_object *virtual_object_proxy)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  auto virtual_object = static_cast<VirtualObject *>(
      wl_proxy_get_user_data((wl_proxy *)virtual_object_proxy));

  self->delegate_->RayLeave(serial, virtual_object);
}

void
System::Impl::HandleRayMotion(void *data, struct zwn_ray * /*zwn_ray*/,
    uint32_t time, struct wl_array *origin_wl_array,
    struct wl_array *direction_wl_array)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  glm::vec3 origin, direction;
  to_vec3(origin_wl_array, origin);
  to_vec3(direction_wl_array, direction);

  self->delegate_->RayMotion(time, origin, direction);
}

void
System::Impl::HandleRayButton(void *data, struct zwn_ray * /*zwn_ray*/,
    uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  bool pressed;
  if (state == ZWN_RAY_BUTTON_STATE_PRESSED) {
    pressed = true;
  } else if (state == ZWN_RAY_BUTTON_STATE_RELEASED) {
    pressed = false;
  } else {
    assert(false && "unknown ray button state");
  }
  self->delegate_->RayButton(serial, time, button, pressed);
}

void
System::Impl::HandleRayAxis(void *data, struct zwn_ray * /*zwn_ray*/,
    uint32_t /*time*/, uint32_t axis, wl_fixed_t value)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  if (axis == ZWN_RAY_AXIS_HORIZONTAL_SCROLL) {
    self->ray_axis_event_.horizontal += wl_fixed_to_double(value);
  } else {
    self->ray_axis_event_.vertical += wl_fixed_to_double(value);
  }
}

void
System::Impl::HandleRayAxisSource(
    void *data, struct zwn_ray * /*zwn_ray*/, uint32_t axis_source)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  switch (axis_source) {
    case ZWN_RAY_AXIS_SOURCE_WHEEL:
      self->ray_axis_event_.source = RayAxisEvent::kWheel;
      return;

    case ZWN_RAY_AXIS_SOURCE_FINGER:
      self->ray_axis_event_.source = RayAxisEvent::kFinger;
      return;

    case ZWN_RAY_AXIS_SOURCE_CONTINUOUS:
      self->ray_axis_event_.source = RayAxisEvent::kContinuous;
      return;

    case ZWN_RAY_AXIS_SOURCE_WHEEL_TILT:
      self->ray_axis_event_.source = RayAxisEvent::kWheelTilt;
      return;

    default:
      return;
  }
}

void
System::Impl::HandleRayAxisStop(
    void *data, struct zwn_ray * /*zwn_ray*/, uint32_t /*time*/, uint32_t axis)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  if (axis == ZWN_RAY_AXIS_HORIZONTAL_SCROLL) {
    self->ray_axis_event_.stop_horizontal = true;
  } else {
    self->ray_axis_event_.stop_vertical = true;
  }
}

void
System::Impl::HandleRayAxisDiscrete(
    void *data, struct zwn_ray * /*zwn_ray*/, uint32_t axis, int32_t discrete)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  if (axis == ZWN_RAY_AXIS_HORIZONTAL_SCROLL) {
    self->ray_axis_event_.discrete_horizontal += discrete;
  } else {
    self->ray_axis_event_.discrete_vertical += discrete;
  }
}

void
System::Impl::HandleRayFrame(void *data, struct zwn_ray * /*zwn_ray*/)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  self->delegate_->RayAxisFrame(self->ray_axis_event_);

  self->ray_axis_event_ = RayAxisEvent();
}

const zwn_seat_listener System::Impl::seat_listener_ = {
    System::Impl::HandleSeatCapabilities,
};

void
System::Impl::HandleSeatCapabilities(
    void *data, struct zwn_seat * /*zwn_seat*/, uint32_t capabilities)
{
  auto self = static_cast<System::Impl *>(data);
  bool has_ray =
      (ZWN_SEAT_CAPABILITY_RAY_ORIGIN | ZWN_SEAT_CAPABILITY_RAY_DIRECTION) &
      capabilities;

  if (has_ray && !self->zwn_ray_) {
    self->zwn_ray_ = zwn_seat_get_ray(self->zwn_seat_);
    zwn_ray_add_listener(self->zwn_ray_, &System::Impl::ray_listener_, self);
  }

  if (!has_ray && self->zwn_ray_) {
    zwn_ray_release(self->zwn_ray_);
    self->zwn_ray_ = nullptr;
  }
}

const wl_registry_listener System::Impl::registry_listener_ = {
    System::Impl::HandleGlobal,
    System::Impl::HandleGlobalRemove,
};

void
System::Impl::HandleGlobal(void *data, struct wl_registry *registry,
    uint32_t name, const char *interface, uint32_t version)
{
  auto self = static_cast<System::Impl *>(data);
  if (std::strcmp(interface, "zwn_compositor") == 0) {
    if (self->zwn_compositor_) zwn_compositor_destroy(self->zwn_compositor_);
    self->zwn_compositor_ = static_cast<zwn_compositor *>(
        wl_registry_bind(registry, name, &zwn_compositor_interface, version));
  } else if (std::strcmp(interface, "zwn_seat") == 0) {
    if (self->zwn_seat_) zwn_seat_destroy(self->zwn_seat_);
    self->zwn_seat_ = static_cast<zwn_seat *>(
        wl_registry_bind(registry, name, &zwn_seat_interface, version));
    zwn_seat_add_listener(self->zwn_seat_, &System::Impl::seat_listener_, self);
  } else if (std::strcmp(interface, "zwn_gles_v32") == 0) {
    if (self->zwn_gles_v32_) zwn_gles_v32_destroy(self->zwn_gles_v32_);
    self->zwn_gles_v32_ = static_cast<zwn_gles_v32 *>(
        wl_registry_bind(registry, name, &zwn_gles_v32_interface, version));
  } else if (std::strcmp(interface, "zwn_shell") == 0) {
    if (self->zwn_shell_) zwn_shell_destroy(self->zwn_shell_);
    self->zwn_shell_ = static_cast<zwn_shell *>(
        wl_registry_bind(registry, name, &zwn_shell_interface, version));
  } else if (std::strcmp(interface, "zwn_shm") == 0) {
    if (self->zwn_shm_) zwn_shm_destroy(self->zwn_shm_);
    self->zwn_shm_ = static_cast<zwn_shm *>(
        wl_registry_bind(registry, name, &zwn_shm_interface, version));
  }
}

void
System::Impl::HandleGlobalRemove(
    void * /*data*/, struct wl_registry * /*wl_registry*/, uint32_t /*name*/)
{}

bool
System::Impl::TryConnect(const char *socket)
{
  display_ = wl_display_connect(socket);
  if (display_ == nullptr) {
    goto err;
  }

  registry_ = wl_display_get_registry(display_);
  if (registry_ == nullptr) {
    LOG_DEBUG("Failed to get wl_registry");
    goto err_disconnect;
  }

  wl_registry_add_listener(registry_, &System::Impl::registry_listener_, this);

  wl_display_roundtrip(display_);

  if (zwn_compositor_ == nullptr || zwn_seat_ == nullptr ||
      zwn_gles_v32_ == nullptr || zwn_shell_ == nullptr ||
      zwn_shm_ == nullptr) {
    LOG_DEBUG("Server does not support zwin protocols");
    goto err_globals;
  }

  event_source_ = loop_.AddFd(
      wl_display_get_fd(display_), Loop::kEventReadable,
      [](int /*fd*/, uint32_t mask, void *data) {
        auto system = static_cast<System::Impl *>(data);
        if (mask & Loop::kEventError || mask & Loop::kEventHangUp) {
          LOG_ERROR("Disconnected from wayland server");
          system->loop()->Terminate(EXIT_FAILURE);
        } else if (mask == Loop::kEventReadable) {
          system->Poll();
        }
      },
      this);

  return true;

err_globals:
  if (zwn_shm_) zwn_shm_destroy(zwn_shm_);
  zwn_shm_ = nullptr;

  if (zwn_shell_) zwn_shell_destroy(zwn_shell_);
  zwn_shell_ = nullptr;

  if (zwn_gles_v32_) zwn_gles_v32_destroy(zwn_gles_v32_);
  zwn_gles_v32_ = nullptr;

  if (zwn_seat_) zwn_seat_destroy(zwn_seat_);
  zwn_seat_ = nullptr;

  if (zwn_compositor_) zwn_compositor_destroy(zwn_compositor_);
  zwn_compositor_ = nullptr;

  wl_registry_destroy(registry_);
  registry_ = nullptr;

err_disconnect:
  wl_display_disconnect(display_);

err:
  return false;
}

void
System::Impl::Poll()
{
  while (wl_display_prepare_read(display_) != 0) {
    if (errno != EAGAIN) {
      loop_.Terminate(EXIT_FAILURE);
      return;
    }
    wl_display_dispatch_pending(display_);
  }

  if (wl_display_flush(display_) == -1) {
    loop_.Terminate(EXIT_FAILURE);
    return;
  }
  wl_display_read_events(display_);
  wl_display_dispatch_pending(display_);

  if (wl_display_flush(display_) == -1) {
    loop_.Terminate(EXIT_FAILURE);
    return;
  }
}

ZUKOU_EXPORT bool
System::Init(const char *socket) const
{
  char candidate[] = "wayland-n";

  if (!pimpl->loop_.Init()) {
    LOG_ERROR("Failed to initialize event loop");
    return false;
  }

  if (socket == nullptr) socket = std::getenv("WAYLAND_DISPLAY");

  bool connected = false;
  if (socket) {
    connected = pimpl->TryConnect(socket);
    if (!connected) LOG_ERROR("Failed to connect to %s", socket);
  } else {
    for (int i = 0; i <= 9; i++) {
      candidate[8] = '0' + i;
      if (pimpl->TryConnect(candidate)) {
        connected = true;
        socket = candidate;
        setenv("WAYLAND_DISPLAY", socket, true);
        break;
      }
      LOG_DEBUG("Failed to connect to %s", candidate);
    }
  }

  if (connected) LOG_INFO("Connected to %s", socket);

  return connected;
}

ZUKOU_EXPORT int
System::Run() const
{
  wl_display_flush(pimpl->display_);
  return pimpl->loop_.Run();
}

ZUKOU_EXPORT void
System::Terminate(int exit_status)
{
  pimpl->loop_.Terminate(exit_status);
}

ZUKOU_EXPORT
System::System(ISystemDelegate *delegate) : pimpl(new Impl(delegate)) {}

ZUKOU_EXPORT
System::~System() = default;

System::Impl::Impl(ISystemDelegate *delegate) : delegate_(delegate) {}

System::Impl::~Impl()
{
  if (zwn_shm_) zwn_shm_destroy(zwn_shm_);
  if (zwn_shell_) zwn_shell_destroy(zwn_shell_);
  if (zwn_gles_v32_) zwn_gles_v32_destroy(zwn_gles_v32_);
  if (zwn_seat_) zwn_seat_destroy(zwn_seat_);
  if (zwn_compositor_) zwn_compositor_destroy(zwn_compositor_);
  if (registry_) wl_registry_destroy(registry_);
  if (display_) wl_display_disconnect(display_);
}

}  // namespace zukou
