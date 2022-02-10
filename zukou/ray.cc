#include "ray.h"

#include <zigen-client-protocol.h>
#include <zukou/zukou.h>

#include <memory>

#include "glm-helper.h"
#include "wayland-helper.h"

namespace zukou {

Ray::Ray(struct zgn_seat *seat) : focus_() {
  proxy_ = zgn_seat_get_ray(seat);
  if (proxy_ == nullptr) throw ZukouException("failed to get ray");

  zgn_ray_add_listener(proxy_, &Ray::ray_listener_, this);
}

Ray::~Ray() { zgn_ray_destroy(proxy_); }

const struct zgn_ray_listener Ray::ray_listener_ = {
    Ray::Enter,
    Ray::Leave,
    Ray::Motion,
    Ray::Button,
    Ray::Axis,
    Ray::Frame,
    Ray::AxisDiscrete,
};

void Ray::Enter(void *data, [[maybe_unused]] struct zgn_ray *zgn_ray,
    uint32_t serial, struct zgn_virtual_object *virtual_object_proxy,
    struct wl_array *origin, struct wl_array *direction) {
  Ray *ray = reinterpret_cast<Ray *>(data);
  auto virtual_object =
      wayland_helper::proxy_get_user_data<VirtualObject>(virtual_object_proxy);

  ray->focus_ = virtual_object->shared_from_this();

  virtual_object->RayEnter(serial, glm_helper::vec3_from_wl_array(origin),
      glm_helper::vec3_from_wl_array(direction));
}

void Ray::Leave(void *data, [[maybe_unused]] struct zgn_ray *zgn_ray,
    uint32_t serial,
    [[maybe_unused]] struct zgn_virtual_object *virtual_object_proxy) {
  Ray *ray = reinterpret_cast<Ray *>(data);

  if (auto virtual_object = ray->focus_.lock()) {
    virtual_object->RayLeave(serial);
  }

  ray->focus_.reset();
}

void Ray::Motion(void *data, [[maybe_unused]] struct zgn_ray *zgn_ray,
    uint32_t time, struct wl_array *origin, struct wl_array *direction) {
  Ray *ray = reinterpret_cast<Ray *>(data);

  if (auto virtual_object = ray->focus_.lock()) {
    virtual_object->RayMotion(time, glm_helper::vec3_from_wl_array(origin),
        glm_helper::vec3_from_wl_array(direction));
  }
}

void Ray::Button(void *data, [[maybe_unused]] struct zgn_ray *zgn_ray,
    uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
  Ray *ray = reinterpret_cast<Ray *>(data);

  if (auto virtual_object = ray->focus_.lock()) {
    virtual_object->RayButton(
        serial, time, button, state == ZGN_RAY_BUTTON_STATE_PRESSED);
  }
}

void Ray::Axis(void *data, [[maybe_unused]] struct zgn_ray *zgn_ray,
    uint32_t time, uint32_t axis, wl_fixed_t value) {
  Ray *ray = reinterpret_cast<Ray *>(data);

  if (auto virtual_object = ray->focus_.lock()) {
    virtual_object->RayAxis(time, axis, wl_fixed_to_double(value));
  }
}

void Ray::Frame(void *data, [[maybe_unused]] struct zgn_ray *zgn_ray) {
  Ray *ray = reinterpret_cast<Ray *>(data);

  if (auto virtual_object = ray->focus_.lock()) {
    virtual_object->RayFrame();
  }
}

void Ray::AxisDiscrete(void *data, [[maybe_unused]] struct zgn_ray *zgn_ray,
    uint32_t axis, int32_t discrete) {
  Ray *ray = reinterpret_cast<Ray *>(data);

  if (auto virtual_object = ray->focus_.lock()) {
    virtual_object->RayAxisDiscrete(axis, discrete);
  }
}

}  // namespace zukou
