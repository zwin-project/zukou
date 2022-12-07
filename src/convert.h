#pragma once

#include <wayland-client.h>

#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>

namespace zukou {

bool to_vec3(struct wl_array *array, glm::vec3 &vec);

bool to_array(const glm::vec3 &vec, struct wl_array *array);

bool to_array(const glm::quat &quaternion, struct wl_array *array);

bool to_array(const float value, struct wl_array *array);

bool to_array(const off_t value, struct wl_array *array);

bool to_array(const uint64_t value, struct wl_array *array);

}  // namespace zukou
