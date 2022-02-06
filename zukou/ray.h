#ifndef ZUKOU_RAY_H
#define ZUKOU_RAY_H

#include <zigen-client-protocol.h>
#include <zukou/zukou.h>

#include <memory>

namespace zukou {
class Ray {
 public:
  Ray(const Ray &) = delete;
  Ray(Ray &&) = delete;
  Ray &operator=(const Ray &) = delete;
  Ray &operator=(Ray &&) = delete;

  Ray(struct zgn_seat *seat);
  ~Ray();

 private:
  static const struct zgn_ray_listener ray_listener_;
  static void Enter(void *data, struct zgn_ray *ray, uint32_t serial,
      struct zgn_virtual_object *virtual_object, struct wl_array *origin,
      struct wl_array *direction);
  static void Leave(void *data, struct zgn_ray *ray, uint32_t serial,
      struct zgn_virtual_object *virtual_object);
  static void Motion(void *data, struct zgn_ray *ray, uint32_t time,
      struct wl_array *origin, struct wl_array *direction);
  static void Button(void *data, struct zgn_ray *ray, uint32_t serial,
      uint32_t time, uint32_t button, uint32_t state);

 private:
  struct zgn_ray *proxy_;
  std::weak_ptr<VirtualObject> focus_;
};
}  // namespace zukou

#endif  //  ZUKOU_RAY_H
