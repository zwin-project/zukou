#ifndef ZUKOU_GLM_HELPER_H
#define ZUKOU_GLM_HELPER_H

#include <wayland-client.h>

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace zukou {
namespace glm_helper {

static inline glm::vec3 vec3_from_wl_array(struct wl_array *array) {
  float *data = (float *)array->data;
  return glm::vec3(data[0], data[1], data[2]);
}

static inline void to_wl_array(glm::vec3 v, struct wl_array *array) {
  size_t size = sizeof(float) * 3;
  float *data = (float *)wl_array_add(array, size);
  memcpy(data, &v, size);
}

static inline void to_wl_array(glm::vec4 v, struct wl_array *array) {
  size_t size = sizeof(float) * 4;
  float *data = (float *)wl_array_add(array, size);
  memcpy(data, &v, size);
}

static inline void to_wl_array(glm::mat4 m, struct wl_array *array) {
  size_t size = sizeof(float) * 16;
  float *data = (float *)wl_array_add(array, size);
  memcpy(data, &m, size);
}

static inline glm::quat quat_from_wl_array(struct wl_array *array) {
  float *data = (float *)array->data;
  return glm::quat(data[3], data[0], data[1], data[2]);
}

static inline void to_wl_array(glm::quat quaternion, struct wl_array *array) {
  size_t size = sizeof(glm::quat);
  float *data = (float *)wl_array_add(array, size);
  memcpy(data, &quaternion, size);
}

}  // namespace glm_helper
}  // namespace zukou

#endif  //  ZUKOU_GLM_HELPER_H
