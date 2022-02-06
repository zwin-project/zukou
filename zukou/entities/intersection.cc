#include <zukou/entities.h>

namespace zukou {
namespace entities {
namespace intersection {

static int ray_obb_axis_test(glm::vec3 axis, glm::vec3 translation,
    glm::vec3 direction, float axis_min, float axis_max, float *t_min,
    float *t_max) {
  float e = glm::dot(axis, translation);
  float f = glm::dot(direction, axis);

  if (fabs(f) > 0.001f) {
    float t1 = (e + axis_min) / f;
    float t2 = (e + axis_max) / f;
    if (t1 > t2) std::swap(t1, t2);
    if (t2 < *t_max) *t_max = t2;
    if (*t_min < t1) *t_min = t1;
    if (*t_max < *t_min) return -1;
  } else {
    if (-e + axis_min > 0.0f || -e + axis_max < 0.0f) return -1;
  }
  return 0;
}

float ray_obb(glm::vec3 origin, glm::vec3 direction, glm::vec3 aabb_half_size,
    glm::mat4 transform) {
  float t_min = 0.0f, t_max = FLT_MAX;

  glm::vec3 obb_position_worldspace = transform[3];
  glm::vec3 translation = obb_position_worldspace - origin;

  for (int i = 0; i < 3; i++) {
    glm::vec3 axis = transform[i];
    if (ray_obb_axis_test(axis, translation, direction, -aabb_half_size[i],
            aabb_half_size[i], &t_min, &t_max) == -1)
      return -1;
  }

  return t_min;
}

}  // namespace intersection
}  // namespace entities
}  // namespace zukou
