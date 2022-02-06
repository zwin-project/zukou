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

  virtual ~IObject(){};
};

class ObjectGroup {
 public:
  bool Init();
  void RayMotion(glm::vec3 origin, glm::vec3 direction, uint32_t time);
  void RayButton(uint32_t serial, uint32_t time, uint32_t button, bool pressed);
  void RayLeave();
  void AddObject(std::weak_ptr<IObject> object);
  bool Draw();

 private:
  void SetFocus(std::weak_ptr<IObject> object);
  std::weak_ptr<IObject> PickObject(glm::vec3 origin, glm::vec3 direction);

 private:
  std::list<std::weak_ptr<IObject>> objects_;
  std::weak_ptr<IObject> ray_focus_;
};

}  // namespace objects
}  // namespace zukou

#endif  //  ZUKOU_OBJECTS_H
