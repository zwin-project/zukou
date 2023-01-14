#pragma once

#include <zukou.h>

#include <zwin-shell-client-protocol.h>

namespace zukou {

class Expansive::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(System* system, IExpansiveDelegate* delegate);
  ~Impl();

  bool Init(VirtualObject* virtual_object);

  void SetRegion(Region* region);

  inline zwn_expansive* proxy();

 private:
  static const zwn_expansive_listener listener_;
  static void HandleEnter(void* data, struct zwn_expansive* zwn_expansive);
  static void HandleLeave(void* data, struct zwn_expansive* zwn_expansive);
  static void HandleShutdown(void* data, struct zwn_expansive* zwn_expansive);

  System* system_;
  IExpansiveDelegate* delegate_;

  zwn_expansive* proxy_;
};

inline zwn_expansive*
Expansive::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
