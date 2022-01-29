#include <zigen-shell-client-protocol.h>
#include <zukou.h>

#include <glm/glm.hpp>

#include "glm-helper.h"

namespace zukou {

CuboidWindow::CuboidWindow(
    std::shared_ptr<Application> app, glm::vec3 half_size, glm::quat quaternion)
    : VirtualObject(app),
      proxy_(nullptr),
      half_size_(glm::vec3()),
      quaternion_(glm::quat()) {
  struct wl_array wl_half_size, wl_quaternion;
  wl_array_init(&wl_half_size);
  wl_array_init(&wl_quaternion);

  glm_helper::to_wl_array(half_size, &wl_half_size);
  glm_helper::to_wl_array(quaternion, &wl_quaternion);

  proxy_ = zgn_shell_get_cuboid_window(
      app->shell_, this->virtual_object_proxy_, &wl_half_size, &wl_quaternion);
  zgn_cuboid_window_add_listener(
      proxy_, &CuboidWindow::cuboid_window_listener_, this);

  wl_array_release(&wl_half_size);
  wl_array_release(&wl_quaternion);
}

CuboidWindow::CuboidWindow(
    std::shared_ptr<Application> app, glm::vec3 half_size)
    : CuboidWindow(app, half_size, glm::quat()) {}

CuboidWindow::~CuboidWindow() { zgn_cuboid_window_destroy(proxy_); }

const struct zgn_cuboid_window_listener CuboidWindow::cuboid_window_listener_ =
    {
        CuboidWindow::Configure,
        CuboidWindow::Moved,
};

void CuboidWindow::Move(uint32_t serial) {
  zgn_cuboid_window_move(proxy_, app_->seat_, serial);
}

void CuboidWindow::Rotate(glm::quat quaternion) {
  struct wl_array wl_quaternion;
  wl_array_init(&wl_quaternion);

  glm_helper::to_wl_array(quaternion, &wl_quaternion);
  zgn_cuboid_window_rotate(proxy_, &wl_quaternion);

  wl_array_release(&wl_quaternion);
}

void CuboidWindow::Configured([[maybe_unused]] uint32_t serial) {}

void CuboidWindow::Moved([[maybe_unused]] glm::vec3 face_direction) {}

void CuboidWindow::Configure(void *data,
    [[maybe_unused]] struct zgn_cuboid_window *zgn_cuboid_window,
    uint32_t serial, struct wl_array *wl_half_size,
    struct wl_array *wl_quaternion) {
  CuboidWindow *cuboid_window = reinterpret_cast<CuboidWindow *>(data);
  cuboid_window->half_size_ = glm_helper::vec3_from_wl_array(wl_half_size);
  cuboid_window->quaternion_ = glm_helper::quat_from_wl_array(wl_quaternion);
  zgn_cuboid_window_ack_configure(cuboid_window->proxy_, serial);
  cuboid_window->Configured(serial);
}

void CuboidWindow::Moved(void *data,
    [[maybe_unused]] struct zgn_cuboid_window *zgn_cuboid_window,
    struct wl_array *wl_face_direction) {
  CuboidWindow *cuboid_window = reinterpret_cast<CuboidWindow *>(data);
  glm::vec3 face_direction = glm_helper::vec3_from_wl_array(wl_face_direction);
  cuboid_window->Moved(face_direction);
}

}  // namespace zukou
