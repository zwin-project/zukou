#pragma once

#include <zukou.h>

#include <zwin-client-protocol.h>

namespace zukou {

class Region::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(System *system);
  ~Impl();

  bool Init();

  void AddCuboid(const glm::vec3 &half_size, const glm::vec3 &center,
      const glm::quat &quaternion);

  inline zwn_region *proxy();

 private:
  System *system_;

  zwn_region *proxy_ = nullptr;
};

inline zwn_region *
Region::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
