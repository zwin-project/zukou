#pragma once

#include <zukou.h>

#include <wayland-client.h>
#include <zigen-client-protocol.h>

namespace zukou {

class VirtualObject::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(System* system, IVirtualObjectDelegate* delegate);
  ~Impl();

  bool Init();

  void Commit();

  void NextFrame();

  inline zgn_virtual_object* proxy();

 private:
  static const wl_callback_listener callback_listener_;
  static void Callback(
      void* data, struct wl_callback* wl_callback, uint32_t callback_data);

  System* system_;
  IVirtualObjectDelegate* delegate_;

  zgn_virtual_object* proxy_ = nullptr;
  wl_callback* callback_ = nullptr;
};

inline zgn_virtual_object*
VirtualObject::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
