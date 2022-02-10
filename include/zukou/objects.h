#ifndef ZUKOU_OBJECTS_H
#define ZUKOU_OBJECTS_H

#include <zukou/zukou.h>

#include <list>

namespace zukou {
namespace objects {

class IObject {
 public:
  virtual bool Init() = 0;
  virtual bool Draw() = 0;
  virtual float Intersect(glm::vec3 origin, glm::vec3 direction) = 0;

  virtual void RayEnter() = 0;
  virtual void RayLeave() = 0;
  virtual void RayMotion(
      glm::vec3 origin, glm::vec3 direction, uint32_t time) = 0;
  virtual void RayButton(
      uint32_t serial, uint32_t time, uint32_t button, bool pressed) = 0;
  virtual void RayAxis(uint32_t time, uint32_t axis, float value) = 0;
  virtual void RayFrame() = 0;
  virtual void RayAxisDiscrete(uint32_t axis, int32_t discrete) = 0;

  virtual void DataDeviceEnter(
      uint32_t serial, std::weak_ptr<DataOffer> data_offer) = 0;
  virtual void DataDeviceLeave() = 0;
  virtual void DataDeviceMotion(
      uint32_t time, glm::vec3 origin, glm::vec3 direction) = 0;
  virtual void DataDeviceDrop() = 0;

  virtual ~IObject(){};
};

class ObjectGroup {
 public:
  bool Init();
  void RayLeave();
  void RayMotion(glm::vec3 origin, glm::vec3 direction, uint32_t time);
  void RayButton(uint32_t serial, uint32_t time, uint32_t button, bool pressed);
  void RayAxis(uint32_t time, uint32_t axis, float value);
  void RayFrame();
  void RayAxisDiscrete(uint32_t axis, int32_t discrete);

  void AddObject(std::weak_ptr<IObject> object);
  bool Draw();

  void DataDeviceEnter(uint32_t serial, std::weak_ptr<DataOffer> data_offer);
  void DataDeviceLeave();
  void DataDeviceMotion(uint32_t time, glm::vec3 origin, glm::vec3 direction);
  void DataDeviceDrop();

 private:
  void SetRayFocus(std::weak_ptr<IObject> object);
  void SetDataDeviceFocus(std::weak_ptr<IObject> object);
  std::weak_ptr<IObject> PickObject(glm::vec3 origin, glm::vec3 direction);

 private:
  std::list<std::weak_ptr<IObject>> objects_;
  std::weak_ptr<IObject> ray_focus_;

  uint32_t data_device_enter_serial_;
  std::weak_ptr<IObject> data_device_focus_;
  std::weak_ptr<zukou::DataOffer> dnd_data_offer_;
};

}  // namespace objects
}  // namespace zukou

#endif  //  ZUKOU_OBJECTS_H
