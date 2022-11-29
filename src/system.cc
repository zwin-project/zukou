#include "system.h"

#include <errno.h>

#include <cstdlib>
#include <cstring>

#include "common.h"
#include "convert.h"
#include "logger.h"
#include "loop.h"

namespace zukou {

const zgn_ray_listener System::Impl::ray_listener_ = {
    System::Impl::HandleRayEnter,
    System::Impl::HandleRayLeave,
    System::Impl::HandleRayMotion,
    System::Impl::HandleRayButton,
};

void
System::Impl::HandleRayEnter(void *data, struct zgn_ray * /*zgn_ray*/,
    uint32_t serial, struct zgn_virtual_object *virtual_object_proxy,
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
System::Impl::HandleRayLeave(void *data, struct zgn_ray * /*zgn_ray*/,
    uint32_t serial, struct zgn_virtual_object *virtual_object_proxy)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  auto virtual_object = static_cast<VirtualObject *>(
      wl_proxy_get_user_data((wl_proxy *)virtual_object_proxy));

  self->delegate_->RayLeave(serial, virtual_object);
}

void
System::Impl::HandleRayMotion(void *data, struct zgn_ray * /*zgn_ray*/,
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
System::Impl::HandleRayButton(void *data, struct zgn_ray * /*zgn_ray*/,
    uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
  auto self = static_cast<System::Impl *>(data);
  if (!self->delegate_) return;

  bool pressed;
  if (state == ZGN_RAY_BUTTON_STATE_PRESSED) {
    pressed = true;
  } else if (state == ZGN_RAY_BUTTON_STATE_RELEASED) {
    pressed = false;
  } else {
    assert(false && "unknown ray button state");
  }
  self->delegate_->RayButton(serial, time, button, pressed);
}

const zgn_seat_listener System::Impl::seat_listener_ = {
    System::Impl::HandleSeatCapabilities,
};

void
System::Impl::HandleSeatCapabilities(
    void *data, struct zgn_seat * /*zgn_seat*/, uint32_t capabilities)
{
  auto self = static_cast<System::Impl *>(data);
  bool has_ray =
      (ZGN_SEAT_CAPABILITY_RAY_ORIGIN | ZGN_SEAT_CAPABILITY_RAY_DIRECTION) &
      capabilities;

  if (has_ray && !self->zgn_ray_) {
    self->zgn_ray_ = zgn_seat_get_ray(self->zgn_seat_);
    zgn_ray_add_listener(self->zgn_ray_, &System::Impl::ray_listener_, self);
  }

  if (!has_ray && self->zgn_ray_) {
    zgn_ray_release(self->zgn_ray_);
    self->zgn_ray_ = nullptr;
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
  if (std::strcmp(interface, "zgn_compositor") == 0) {
    if (self->zgn_compositor_) zgn_compositor_destroy(self->zgn_compositor_);
    self->zgn_compositor_ = static_cast<zgn_compositor *>(
        wl_registry_bind(registry, name, &zgn_compositor_interface, version));
  } else if (std::strcmp(interface, "zgn_seat") == 0) {
    if (self->zgn_seat_) zgn_seat_destroy(self->zgn_seat_);
    self->zgn_seat_ = static_cast<zgn_seat *>(
        wl_registry_bind(registry, name, &zgn_seat_interface, version));
    zgn_seat_add_listener(self->zgn_seat_, &System::Impl::seat_listener_, self);
  } else if (std::strcmp(interface, "zgn_gles_v32") == 0) {
    if (self->zgn_gles_v32_) zgn_gles_v32_destroy(self->zgn_gles_v32_);
    self->zgn_gles_v32_ = static_cast<zgn_gles_v32 *>(
        wl_registry_bind(registry, name, &zgn_gles_v32_interface, version));
  } else if (std::strcmp(interface, "zgn_shell") == 0) {
    if (self->zgn_shell_) zgn_shell_destroy(self->zgn_shell_);
    self->zgn_shell_ = static_cast<zgn_shell *>(
        wl_registry_bind(registry, name, &zgn_shell_interface, version));
  } else if (std::strcmp(interface, "zgn_shm") == 0) {
    if (self->zgn_shm_) zgn_shm_destroy(self->zgn_shm_);
    self->zgn_shm_ = static_cast<zgn_shm *>(
        wl_registry_bind(registry, name, &zgn_shm_interface, version));
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

  if (zgn_compositor_ == nullptr || zgn_seat_ == nullptr ||
      zgn_gles_v32_ == nullptr || zgn_shell_ == nullptr ||
      zgn_shm_ == nullptr) {
    LOG_DEBUG("Server does not support zigen protocols");
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
  if (zgn_shm_) zgn_shm_destroy(zgn_shm_);
  zgn_shm_ = nullptr;

  if (zgn_shell_) zgn_shell_destroy(zgn_shell_);
  zgn_shell_ = nullptr;

  if (zgn_gles_v32_) zgn_gles_v32_destroy(zgn_gles_v32_);
  zgn_gles_v32_ = nullptr;

  if (zgn_seat_) zgn_seat_destroy(zgn_seat_);
  zgn_seat_ = nullptr;

  if (zgn_compositor_) zgn_compositor_destroy(zgn_compositor_);
  zgn_compositor_ = nullptr;

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
        break;
      }
      LOG_DEBUG("Failed to connect to %s", candidate);
    }
  }

  if (connected) LOG_INFO("Connected to %s", socket);

  return connected;
}

ZUKOU_EXPORT bool
System::Run() const
{
  wl_display_flush(pimpl->display_);
  return pimpl->loop_.Run();
}

ZUKOU_EXPORT
System::System(ISystemDelegate *delegate) : pimpl(new Impl(delegate)) {}

ZUKOU_EXPORT
System::~System() = default;

System::Impl::Impl(ISystemDelegate *delegate) : delegate_(delegate) {}

System::Impl::~Impl()
{
  if (zgn_shm_) zgn_shm_destroy(zgn_shm_);
  if (zgn_shell_) zgn_shell_destroy(zgn_shell_);
  if (zgn_gles_v32_) zgn_gles_v32_destroy(zgn_gles_v32_);
  if (zgn_seat_) zgn_seat_destroy(zgn_seat_);
  if (zgn_compositor_) zgn_compositor_destroy(zgn_compositor_);
  if (registry_) wl_registry_destroy(registry_);
  if (display_) wl_display_disconnect(display_);
}

}  // namespace zukou
