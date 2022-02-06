#include "cube.h"

Cube::Cube(std::shared_ptr<zukou::Application> app,
    std::shared_ptr<zukou::VirtualObject> virtual_object)
    : entity_(new zukou::entities::FrameCuboid(app, virtual_object)),
      virtual_object_(virtual_object) {}

bool Cube::Init() { return entity_->Init(); }

bool Cube::Draw() {
  entity_->set_frame_color(focus_ ? pressed_ ? red : magenta : cyan);
  return entity_->Draw();
}

float Cube::Intersect(glm::vec3 origin, glm::vec3 direction) {
  return entity_->Intersect(origin, direction);
}

void Cube::RayEnter() {
  focus_ = true;
  virtual_object_->ScheduleNextFrame();
}

void Cube::RayLeave() {
  focus_ = false;
  pressed_ = false;
  virtual_object_->ScheduleNextFrame();
}

void Cube::RayMotion([[maybe_unused]] glm::vec3 origin,
    [[maybe_unused]] glm::vec3 direction, [[maybe_unused]] uint32_t time) {}

void Cube::RayButton([[maybe_unused]] uint32_t serial,
    [[maybe_unused]] uint32_t time, [[maybe_unused]] uint32_t button,
    bool pressed) {
  pressed_ = pressed;
  virtual_object_->ScheduleNextFrame();
}

void Cube::set_position(glm::vec3 position) { entity_->set_position(position); }

void Cube::set_half_size(glm::vec3 half_size) {
  entity_->set_half_size(half_size);
}

void Cube::set_quaternion(glm::quat quaternion) {
  entity_->set_quaternion(quaternion);
}

void Cube::set_frame_color(glm::vec4 color) { entity_->set_frame_color(color); }
