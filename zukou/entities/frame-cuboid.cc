#include <zukou/entities.h>

#include <glm/gtx/quaternion.hpp>

namespace zukou {
namespace entities {

FrameCuboid::FrameCuboid(std::shared_ptr<zukou::Application> app,
    std::shared_ptr<zukou::VirtualObject> virtual_object)
    : primitive_(new primitives::FrameCuboid(app, virtual_object)) {}

float FrameCuboid::Intersect(glm::vec3 origin, glm::vec3 direction) {
  glm::mat4 transform(1);
  transform = glm::translate(transform, primitive_->position());
  transform = transform * glm::toMat4(primitive_->quaternion());

  return intersection::ray_obb(
      origin, direction, primitive_->half_size(), transform);
}

}  // namespace entities
}  // namespace zukou
