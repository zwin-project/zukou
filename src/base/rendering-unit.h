#pragma once

#include <zukou.h>

#include <zwin-gles-v32-client-protocol.h>

namespace zukou {

class RenderingUnit::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(System* system);
  ~Impl();

  bool Init(VirtualObject* virtual_object_);

  inline zwn_rendering_unit* proxy();

 private:
  System* system_;

  zwn_rendering_unit* proxy_ = nullptr;
};

inline zwn_rendering_unit*
RenderingUnit::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
