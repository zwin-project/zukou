#pragma once

#include <zukou.h>

#include <zigen-client-protocol.h>

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

  inline zgn_region *proxy();

 private:
  System *system_;

  zgn_region *proxy_ = nullptr;
};

inline zgn_region *
Region::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
