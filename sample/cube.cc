#include "cube.h"

#include <iostream>
#include <string>

Cube::Cube(std::shared_ptr<zukou::Application> app,
    std::shared_ptr<zukou::VirtualObject> virtual_object)
    : focus_color_(magenta),
      entity_(new zukou::entities::FrameCuboid(app, virtual_object)),
      virtual_object_(virtual_object) {}

bool Cube::Init() { return entity_->Init(); }

bool Cube::Draw() {
  entity_->set_frame_color(focus_ ? pressed_ ? red : focus_color_ : cyan);
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

void Cube::DataDeviceEnter(
    uint32_t serial, std::weak_ptr<zukou::DataOffer> data_offer) {
  data_device_enter_serial_ = serial;
  dnd_data_offer_ = data_offer;
}

void Cube::DataDeviceLeave() {}

void Cube::DataDeviceMotion([[maybe_unused]] uint32_t time,
    [[maybe_unused]] glm::vec3 origin, [[maybe_unused]] glm::vec3 direction) {
  if (auto data_offer = dnd_data_offer_.lock()) {
    for (auto mime_type : data_offer->mime_types()) {
      if (mime_type == "text/plain") {
        data_offer->Accept(data_device_enter_serial_, mime_type);
        return;
      }
    }
  }
}

void Cube::DataDeviceDrop() {
  if (auto data_offer = dnd_data_offer_.lock()) {
    for (auto mime_type : data_offer->mime_types()) {
      if (mime_type == "text/plain") {
        data_offer->Receive(mime_type,
            std::bind(&Cube::DndDataReceived, this, std::placeholders::_1));
        return;
      }
    }
  }
}

void Cube::DndDataReceived(int fd) {
  char buf[100];
  std::string str;
  while (true) {
    int size = read(fd, buf, 100);
    if (size <= 0) break;
    std::string fragment(buf, size);
    str += fragment;
  }

  // read color code
  if (str.size() != 7 || str[0] != '#') return;

  try {
    float r = std::stoi(str.substr(1, 2), nullptr, 16);
    float g = std::stoi(str.substr(3, 2), nullptr, 16);
    float b = std::stoi(str.substr(5, 2), nullptr, 16);
    focus_color_ = glm::vec4(r / 256.0f, g / 256.0f, b / 256.0f, 1.0f);
    virtual_object_->ScheduleNextFrame();
  } catch (const std::invalid_argument &e) {
    std::cerr << e.what() << std::endl;
  }
}

void Cube::set_position(glm::vec3 position) { entity_->set_position(position); }

void Cube::set_half_size(glm::vec3 half_size) {
  entity_->set_half_size(half_size);
}

void Cube::set_quaternion(glm::quat quaternion) {
  entity_->set_quaternion(quaternion);
}

void Cube::set_frame_color(glm::vec4 color) { entity_->set_frame_color(color); }
