#pragma once

#include <zukou.h>

#include <zigen-shell-client-protocol.h>

namespace zukou {

class Expansive::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(System* system, IExpansiveDelegate* delegate);
  ~Impl();

  bool Init(VirtualObject* virtual_object);

  void SetRegion(Region* region);

  inline zgn_expansive* proxy();

 private:
  static const zgn_expansive_listener listener_;
  static void HandleEnter(void* data, struct zgn_expansive* zgn_expansive);
  static void HandleLeave(void* data, struct zgn_expansive* zgn_expansive);
  static void HandleShutdown(void* data, struct zgn_expansive* zgn_expansive);

  System* system_;
  IExpansiveDelegate* delegate_;

  zgn_expansive* proxy_;
};

inline zgn_expansive*
Expansive::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
