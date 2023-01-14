#pragma once

#include <zukou.h>

#include <zwin-shell-client-protocol.h>

#include <glm/vec3.hpp>

namespace zukou {

class Bounded::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(System* system, IBoundedDelegate* delegate);
  ~Impl();

  bool Init(VirtualObject* virtual_object, const glm::vec3& half_size);

  void AckConfigure(uint32_t serial);

  void SetTitle(const std::string& title);

  void SetRegion(Region* region);

  void Move(uint32_t serial);

  inline zwn_bounded* proxy();

 private:
  static const zwn_bounded_listener listener_;
  static void HandleConfigure(void* data, zwn_bounded* zwn_bounded,
      struct wl_array* half_size, uint32_t serial);

  System* system_;
  IBoundedDelegate* delegate_;

  zwn_bounded* proxy_ = nullptr;
};

inline zwn_bounded*
Bounded::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
