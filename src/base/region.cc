#include "region.h"

#include "common.h"
#include "convert.h"
#include "logger.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT
Region::Region(System* system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
Region::~Region() = default;

ZUKOU_EXPORT bool
Region::Init()
{
  return pimpl->Init();
}

ZUKOU_EXPORT void
Region::AddCuboid(const glm::vec3& half_size, const glm::vec3& center,
    const glm::quat& quaternion)
{
  pimpl->AddCuboid(half_size, center, quaternion);
}

void
Region::Impl::AddCuboid(const glm::vec3& half_size, const glm::vec3& center,
    const glm::quat& quaternion)
{
  struct wl_array half_size_wl_array, center_wl_array, quaternion_wl_array;

  to_array(half_size, &half_size_wl_array);
  to_array(center, &center_wl_array);
  to_array(quaternion, &quaternion_wl_array);

  zwn_region_add_cuboid(
      proxy_, &half_size_wl_array, &center_wl_array, &quaternion_wl_array);

  wl_array_release(&half_size_wl_array);
  wl_array_release(&center_wl_array);
  wl_array_release(&quaternion_wl_array);
}

bool
Region::Impl::Init()
{
  proxy_ = zwn_compositor_create_region(system_->pimpl->compositor());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create region proxy");
    return false;
  }

  return true;
}

Region::Impl::Impl(System* system) : system_(system) {}

Region::Impl::~Impl()
{
  if (proxy_) {
    zwn_region_destroy(proxy_);
  }
}

}  // namespace zukou
