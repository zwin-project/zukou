#include <zukou/objects.h>

namespace zukou {
namespace objects {

bool ObjectGroup::Init() { return true; }

void ObjectGroup::RayLeave() { this->SetRayFocus(std::weak_ptr<IObject>()); }

void ObjectGroup::RayMotion(
    glm::vec3 origin, glm::vec3 direction, uint32_t time) {
  float distance;
  auto object = this->PickObject(origin, direction, &distance);
  this->SetRayFocus(object);

  if (auto focus = this->ray_focus_.lock())
    focus->RayMotion(origin, direction, time);
}

void ObjectGroup::RayButton(
    uint32_t serial, uint32_t time, uint32_t button, bool pressed) {
  if (auto focus = this->ray_focus_.lock())
    focus->RayButton(serial, time, button, pressed);
}

void ObjectGroup::RayAxis(uint32_t time, uint32_t axis, float value) {
  if (auto focus = this->ray_focus_.lock()) focus->RayAxis(time, axis, value);
}

void ObjectGroup::RayFrame() {
  if (auto focus = this->ray_focus_.lock()) focus->RayFrame();
}

void ObjectGroup::RayAxisDiscrete(uint32_t axis, int32_t discrete) {
  if (auto focus = this->ray_focus_.lock())
    focus->RayAxisDiscrete(axis, discrete);
}

void ObjectGroup::AddObject(std::weak_ptr<IObject> object) {
  objects_.push_back(object);
}

bool ObjectGroup::Draw() {
  bool need_commit = false;

  auto it = objects_.begin();
  while (it != objects_.end()) {
    if (auto object = it->lock()) {
      if (object->Draw()) need_commit = true;
      ++it;
    } else {
      it = objects_.erase(it);
    }
  }

  return need_commit;
}

void ObjectGroup::DataDeviceEnter(
    uint32_t serial, std::weak_ptr<DataOffer> data_offer) {
  data_device_enter_serial_ = serial;
  dnd_data_offer_ = data_offer;
}

void ObjectGroup::DataDeviceLeave() {
  this->SetDataDeviceFocus(std::weak_ptr<IObject>());
}

void ObjectGroup::DataDeviceMotion(
    uint32_t time, glm::vec3 origin, glm::vec3 direction) {
  float distance;
  auto object = this->PickObject(origin, direction, &distance);
  this->SetDataDeviceFocus(object);

  if (auto focus = this->data_device_focus_.lock())
    focus->DataDeviceMotion(time, origin, direction);
}

void ObjectGroup::DataDeviceDrop() {
  if (auto focus = this->data_device_focus_.lock()) focus->DataDeviceDrop();
}

float ObjectGroup::Intersect(glm::vec3 origin, glm::vec3 direction) {
  float distance;
  PickObject(origin, direction, &distance);
  return distance;
}

void ObjectGroup::SetRayFocus(std::weak_ptr<IObject> object_weak_ptr) {
  auto object = object_weak_ptr.lock();
  auto focus = this->ray_focus_.lock();

  if (focus == object) return;

  if (focus) focus->RayLeave();

  if (object) object->RayEnter();

  this->ray_focus_ = object_weak_ptr;
}

void ObjectGroup::SetDataDeviceFocus(std::weak_ptr<IObject> object_weak_ptr) {
  auto object = object_weak_ptr.lock();
  auto focus = this->data_device_focus_.lock();

  if (focus == object) return;

  if (focus) focus->DataDeviceLeave();

  if (object)
    object->DataDeviceEnter(data_device_enter_serial_, dnd_data_offer_);

  this->data_device_focus_ = object_weak_ptr;
}

std::weak_ptr<IObject> ObjectGroup::PickObject(
    glm::vec3 origin, glm::vec3 direction, float *distance) {
  float min_len = FLT_MAX;
  std::weak_ptr<IObject> next_focus;
  *distance = -1;

  auto it = objects_.begin();
  while (it != objects_.end()) {
    if (auto object = it->lock()) {
      float len = object->Intersect(origin, direction);
      if (len > 0 && len < min_len) {
        min_len = len;
        *distance = len;
        next_focus = *it;
      }
      ++it;
    } else {
      it = objects_.erase(it);
    }
  }

  return next_focus;
}
}  // namespace objects
}  // namespace zukou
