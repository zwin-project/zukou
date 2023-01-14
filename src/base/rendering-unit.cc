#include "rendering-unit.h"

#include "common.h"
#include "logger.h"
#include "system.h"
#include "virtual-object.h"

namespace zukou {

ZUKOU_EXPORT bool
RenderingUnit::Init(VirtualObject* virtual_object)
{
  return pimpl->Init(virtual_object);
}

ZUKOU_EXPORT
RenderingUnit::RenderingUnit(System* system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
RenderingUnit::~RenderingUnit() = default;

bool
RenderingUnit::Impl::Init(VirtualObject* virtual_object)
{
  proxy_ = zwn_gles_v32_create_rendering_unit(
      system_->pimpl->gles_v32(), virtual_object->pimpl->proxy());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create rendering unit proxy");
    return false;
  }

  return true;
}

RenderingUnit::Impl::Impl(System* system) : system_(system) {}

RenderingUnit::Impl::~Impl()
{
  if (proxy_) {
    zwn_rendering_unit_destroy(proxy_);
  }
}

}  // namespace zukou
