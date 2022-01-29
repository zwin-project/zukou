#ifndef ZUKOU_WAYLAND_HELPER_H
#define ZUKOU_WAYLAND_HELPER_H

#include <wayland-client.h>

namespace zukou {
namespace wayland_helper {

template <typename T>
static inline T* proxy_get_user_data(void* proxy) {
  return reinterpret_cast<T*>(
      wl_proxy_get_user_data(reinterpret_cast<wl_proxy*>(proxy)));
}

}  // namespace wayland_helper
}  // namespace zukou

#endif  //  ZUKOU_WAYLAND_HELPER_H
