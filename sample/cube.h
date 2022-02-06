#ifndef ZUKOU_SAMPLE_CUBE_H
#define ZUKOU_SAMPLE_CUBE_H

#include <linux/input.h>
#include <zukou/zukou.h>

const glm::vec4 cyan(0.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 magenta(1.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
const glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 green(0.0f, 1.0f, 0.0f, 1.0f);
const glm::vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);

class Cube : public zukou::objects::IObject {
 public:
  Cube(std::shared_ptr<zukou::Application> app,
      std::shared_ptr<zukou::VirtualObject> virtual_object);

  virtual bool Init() override;
  virtual bool Draw() override;

  virtual float Intersect(glm::vec3 origin, glm::vec3 direction) override;
  virtual void RayEnter() override;
  virtual void RayLeave() override;
  virtual void RayMotion(
      glm::vec3 origin, glm::vec3 direction, uint32_t time) override;
  virtual void RayButton(
      uint32_t serial, uint32_t time, uint32_t button, bool pressed) override;

  inline glm::vec3 position() { return entity_->position(); }
  inline glm::vec3 half_size() { return entity_->half_size(); }
  inline glm::quat quaternion() { return entity_->quaternion(); }
  inline glm::vec4 frame_color() { return entity_->frame_color(); }

  void set_position(glm::vec3 position);
  void set_half_size(glm::vec3 half_size);
  void set_quaternion(glm::quat quaternion);
  void set_frame_color(glm::vec4 color);

 private:
  bool pressed_;
  bool focus_;
  std::unique_ptr<zukou::entities::FrameCuboid> entity_;
  std::shared_ptr<zukou::VirtualObject> virtual_object_;
};

#endif  //  ZUKOU_SAMPLE_CUBE_H
