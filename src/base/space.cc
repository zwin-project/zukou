#include "space.h"

#include "common.h"
#include "logger.h"
#include "system.h"
#include "virtual-object.h"

namespace zukou {

ZUKOU_EXPORT bool
Space::Init()
{
  if (!VirtualObject::Init()) return false;

  return pimpl->Init(this);
}

ZUKOU_EXPORT
Space::Space(System* system, ISpaceDelegate* delegate)
    : VirtualObject(system, delegate), pimpl(new Impl(system, delegate))
{}

ZUKOU_EXPORT
Space::~Space() = default;

const zen_space_listener Space::Impl::listener_ = {
    Space::Impl::HandleEnter,
    Space::Impl::HandleLeave,
    Space::Impl::HandleShutdown,
};

void
Space::Impl::HandleEnter(void* data, struct zen_space* /*zen_space*/)
{
  auto self = static_cast<Space::Impl*>(data);

  self->delegate_->Enter();
}

void
Space::Impl::HandleLeave(void* data, struct zen_space* /*zen_space*/)
{
  auto self = static_cast<Space::Impl*>(data);

  self->delegate_->Leave();
}

void
Space::Impl::HandleShutdown(void* data, struct zen_space* /*zen_space*/)
{
  auto self = static_cast<Space::Impl*>(data);

  self->delegate_->Shutdown();
}

bool
Space::Impl::Init(VirtualObject* virtual_object)
{
  if (!system_->pimpl->space_manager()) return false;

  proxy_ = zen_space_manager_get_space(
      system_->pimpl->space_manager(), virtual_object->pimpl->proxy());

  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create space proxy");
    return false;
  }

  zen_space_add_listener(proxy_, &Space::Impl::listener_, this);

  return true;
}

Space::Impl::Impl(System* system, ISpaceDelegate* delegate)
    : system_(system), delegate_(delegate)
{}

Space::Impl::~Impl()
{
  if (proxy_) {
    zen_space_destroy(proxy_);
  }
}

}  // namespace zukou
