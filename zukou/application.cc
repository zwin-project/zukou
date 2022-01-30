#include <wayland-client.h>
#include <zigen-client-protocol.h>
#include <zigen-opengl-client-protocol.h>
#include <zigen-shell-client-protocol.h>
#include <zukou/zukou.h>

#include <cstring>

#include "ray.h"

namespace zukou {

std::shared_ptr<Application> Application::Create() {
  std::shared_ptr<Application> app(new Application());
  return app;
}

Application::Application()
    : running_(false),
      display_(nullptr),
      registry_(nullptr),
      compositor_(nullptr),
      seat_(nullptr),
      shm_(nullptr),
      opengl_(nullptr),
      shell_(nullptr) {}

Application::~Application() {
  if (shell_) zgn_shell_destroy(shell_);
  if (opengl_) zgn_opengl_destroy(opengl_);
  if (shm_) wl_shm_destroy(shm_);
  if (seat_) zgn_seat_destroy(seat_);
  if (compositor_) zgn_compositor_destroy(compositor_);
  if (registry_) wl_registry_destroy(registry_);
  if (display_) wl_display_disconnect(display_);
}

void Application::Connect(std::string socket) {
  display_ = wl_display_connect(socket.c_str());
  if (display_ == nullptr)
    throw ZukouException("failed to connect to zigen server");

  registry_ = wl_display_get_registry(display_);
  if (registry_ == nullptr) throw ZukouException("failed to get wl_registry");

  wl_registry_add_listener(registry_, &Application::registry_listener_, this);

  wl_display_dispatch(display_);
  wl_display_roundtrip(display_);

  if (compositor_ == nullptr || seat_ == nullptr || shm_ == nullptr ||
      opengl_ == nullptr || shell_ == nullptr)
    throw ZukouException("unsupported zigen server");
}

bool Application::Run() {
  int ret;
  running_ = true;
  while (running_) {
    while (wl_display_prepare_read(display_) != 0) {
      if (errno != EAGAIN) return false;
      wl_display_dispatch_pending(display_);
    }

    ret = wl_display_flush(display_);
    if (ret == -1) return false;
    wl_display_read_events(display_);
    wl_display_dispatch_pending(display_);
  }
  return true;
}

void Application::Terminate() { running_ = false; }

const struct wl_registry_listener Application::registry_listener_ = {
    Application::GlobalRegistry,
    Application::GlobalRegistryRemove,
};

void Application::GlobalRegistry(void *data, struct wl_registry *registry,
    uint32_t id, const char *interface, uint32_t version) {
  Application *app = reinterpret_cast<Application *>(data);
  if (strcmp(interface, "zgn_compositor") == 0) {
    app->compositor_ = reinterpret_cast<zgn_compositor *>(
        wl_registry_bind(registry, id, &zgn_compositor_interface, version));
  } else if (strcmp(interface, "zgn_seat") == 0) {
    app->seat_ = reinterpret_cast<zgn_seat *>(
        wl_registry_bind(registry, id, &zgn_seat_interface, version));
    zgn_seat_add_listener(app->seat_, &Application::seat_listener_, app);
  } else if (strcmp(interface, "wl_shm") == 0) {
    app->shm_ = reinterpret_cast<wl_shm *>(
        wl_registry_bind(registry, id, &wl_shm_interface, version));
    wl_shm_add_listener(app->shm_, &Application::shm_listener_, app);
  } else if (strcmp(interface, "zgn_opengl") == 0) {
    app->opengl_ = reinterpret_cast<zgn_opengl *>(
        wl_registry_bind(registry, id, &zgn_opengl_interface, version));
  } else if (strcmp(interface, "zgn_shell") == 0) {
    app->shell_ = reinterpret_cast<zgn_shell *>(
        wl_registry_bind(registry, id, &zgn_shell_interface, version));
  }
}

void Application::GlobalRegistryRemove([[maybe_unused]] void *data,
    [[maybe_unused]] struct wl_registry *registry,
    [[maybe_unused]] uint32_t id) {
  // FIXME:
}

const struct wl_shm_listener Application::shm_listener_ = {
    Application::ShmFormat,
};

void Application::ShmFormat([[maybe_unused]] void *data,
    [[maybe_unused]] struct wl_shm *shm, [[maybe_unused]] uint32_t format) {
  // FIXME:
}

const struct zgn_seat_listener Application::seat_listener_ = {
    Application::SeatCapabilities,
};

void Application::SeatCapabilities(
    void *data, struct zgn_seat *seat, uint32_t capability) {
  Application *app = reinterpret_cast<Application *>(data);

  if (capability & ZGN_SEAT_CAPABILITY_RAY && app->ray_ == nullptr) {
    app->ray_.reset(new Ray(seat));
  }
}

}  // namespace zukou
