#pragma once

#include <zukou.h>

#include <zen-space-client-protocol.h>

namespace zukou {

class Space::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(System* system, ISpaceDelegate* delegate);
  ~Impl();

  bool Init(VirtualObject* virtual_object);

  inline zen_space* proxy();

 private:
  static const zen_space_listener listener_;
  static void HandleEnter(void* data, struct zen_space* zen_space);
  static void HandleLeave(void* data, struct zen_space* zen_space);
  static void HandleShutdown(void* data, struct zen_space* zen_space);

  System* system_;
  ISpaceDelegate* delegate_;

  zen_space* proxy_;
};

inline zen_space*
Space::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
