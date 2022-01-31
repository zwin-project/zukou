#ifndef ZUKOU_OBJECTS_H
#define ZUKOU_OBJECTS_H

#include <zukou/zukou.h>

#include "primitives.h"

namespace zukou {
namespace entities {

class IEntity {
 public:
  virtual bool Init() = 0;
  virtual bool Draw() = 0;
  virtual float Intersect(glm::vec3 origin, glm::vec3 direction) = 0;
  virtual ~IEntity(){};
};

class FrameCuboid : public IEntity {
 public:
  FrameCuboid(std::shared_ptr<zukou::Application> app,
      std::shared_ptr<zukou::VirtualObject> virtual_object);

  virtual bool Init() override { return primitive_->Init(); };
  virtual bool Draw() override { return primitive_->Draw(); };
  virtual float Intersect(glm::vec3 origin, glm::vec3 direction) override;

  inline glm::vec3 position() { return primitive_->position(); }
  inline glm::vec3 half_size() { return primitive_->half_size(); }
  inline glm::quat quaternion() { return primitive_->quaternion(); }
  inline glm::vec4 frame_color() { return primitive_->frame_color(); }

  inline void set_position(glm::vec3 position);
  inline void set_half_size(glm::vec3 half_size);
  inline void set_quaternion(glm::quat quaternion);
  inline void set_frame_color(glm::vec4 color);

 private:
  std::unique_ptr<primitives::FrameCuboid> primitive_;
};

inline void FrameCuboid::set_position(glm::vec3 position) {
  primitive_->set_position(position);
}
inline void FrameCuboid::set_half_size(glm::vec3 half_size) {
  primitive_->set_half_size(half_size);
}
inline void FrameCuboid::set_quaternion(glm::quat quaternion) {
  primitive_->set_quaternion(quaternion);
}
inline void FrameCuboid::set_frame_color(glm::vec4 color) {
  primitive_->set_frame_color(color);
}

namespace intersection {

float ray_obb(glm::vec3 origin, glm::vec3 direction, glm::vec3 aabb_half_size,
    glm::mat4 transform);

}  // namespace intersection

}  // namespace entities
}  // namespace zukou

#endif  //  ZUKOU_OBJECTS_H
