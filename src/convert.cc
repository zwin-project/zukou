#include "convert.h"

#include <cstring>

namespace zukou {

bool
to_vec3(struct wl_array *array, glm::vec3 &vec)
{
  if (array->size != sizeof(glm::vec3)) return false;

  std::memcpy(&vec, array->data, sizeof(glm::vec3));

  return true;
}

bool
to_array(const glm::vec3 &vec, struct wl_array *array)
{
  wl_array_init(array);

  void *container = wl_array_add(array, sizeof(glm::vec3));
  if (container == nullptr) return false;

  std::memcpy(container, &vec, sizeof(glm::vec3));

  return true;
}

bool
to_array(const glm::quat &quaternion, struct wl_array *array)
{
  wl_array_init(array);

  void *container = wl_array_add(array, sizeof(glm::quat));
  if (container == nullptr) return false;

  std::memcpy(container, &quaternion, sizeof(glm::quat));

  return true;
}

bool
to_array(const float value, struct wl_array *array)
{
  wl_array_init(array);

  auto container = static_cast<float *>(wl_array_add(array, sizeof(float)));
  if (container == nullptr) return false;

  *container = value;
  return true;
}

bool
to_array(const off_t value, struct wl_array *array)
{
  wl_array_init(array);

  auto container = static_cast<off_t *>(wl_array_add(array, sizeof(off_t)));
  if (container == nullptr) return false;

  *container = value;
  return true;
}

bool
to_array(const uint64_t value, struct wl_array *array)
{
  wl_array_init(array);

  auto container =
      static_cast<uint64_t *>(wl_array_add(array, sizeof(uint64_t)));
  if (container == nullptr) return false;

  *container = value;
  return true;
}

}  // namespace zukou
