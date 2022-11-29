#include "virtual-object.h"

#include "common.h"
#include "logger.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT bool
VirtualObject::Init()
{
  return pimpl->Init();
}

ZUKOU_EXPORT void
VirtualObject::Commit()
{
  pimpl->Commit();
}

ZUKOU_EXPORT void
VirtualObject::NextFrame()
{
  pimpl->NextFrame();
}

ZUKOU_EXPORT
VirtualObject::VirtualObject(System* system, IVirtualObjectDelegate* delegate)
    : pimpl(new Impl(system, delegate))
{}

ZUKOU_EXPORT
VirtualObject::~VirtualObject() = default;

const wl_callback_listener VirtualObject::Impl::callback_listener_ = {
    VirtualObject::Impl::Callback,
};

bool
VirtualObject::Impl::Init()
{
  proxy_ = zgn_compositor_create_virtual_object(system_->pimpl->compositor());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create virtual object proxy");
    return false;
  }

  wl_proxy_set_user_data((wl_proxy*)proxy_, this);

  return true;
}

void
VirtualObject::Impl::Commit()
{
  zgn_virtual_object_commit(proxy_);
}

void
VirtualObject::Impl::NextFrame()
{
  if (callback_) return;

  callback_ = zgn_virtual_object_frame(proxy_);
  wl_callback_add_listener(
      callback_, &VirtualObject::Impl::callback_listener_, this);
}

void
VirtualObject::Impl::Callback(
    void* data, struct wl_callback* /*wl_callback*/, uint32_t callback_data)
{
  auto self = static_cast<VirtualObject::Impl*>(data);

  wl_callback_destroy(self->callback_);
  self->callback_ = nullptr;

  self->delegate_->Frame(callback_data);
}

VirtualObject::Impl::Impl(System* system, IVirtualObjectDelegate* delegate)
    : system_(system), delegate_(delegate)
{}

VirtualObject::Impl::~Impl()
{
  if (callback_) {
    wl_callback_destroy(callback_);
  }

  if (proxy_) {
    zgn_virtual_object_destroy(proxy_);
  }
}

}  // namespace zukou
