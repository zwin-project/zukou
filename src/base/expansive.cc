#include "expansive.h"

#include "common.h"
#include "logger.h"
#include "region.h"
#include "system.h"
#include "virtual-object.h"

namespace zukou {

ZUKOU_EXPORT bool
Expansive::Init()
{
  if (!VirtualObject::Init()) return false;

  return pimpl->Init(this);
}

ZUKOU_EXPORT void
Expansive::SetRegion(Region* region)
{
  pimpl->SetRegion(region);
}

ZUKOU_EXPORT
Expansive::Expansive(System* system, IExpansiveDelegate* delegate)
    : VirtualObject(system, delegate), pimpl(new Impl(system, delegate))
{}

ZUKOU_EXPORT
Expansive::~Expansive() = default;

const zwn_expansive_listener Expansive::Impl::listener_ = {
    Expansive::Impl::HandleEnter,
    Expansive::Impl::HandleLeave,
    Expansive::Impl::HandleShutdown,
};

void
Expansive::Impl::HandleEnter(
    void* data, struct zwn_expansive* /*zwn_expansive*/)
{
  auto self = static_cast<Expansive::Impl*>(data);

  self->delegate_->Enter();
}

void
Expansive::Impl::HandleLeave(
    void* data, struct zwn_expansive* /*zwn_expansive*/)
{
  auto self = static_cast<Expansive::Impl*>(data);

  self->delegate_->Leave();
}

void
Expansive::Impl::HandleShutdown(
    void* data, struct zwn_expansive* /*zwn_expansive*/)
{
  auto self = static_cast<Expansive::Impl*>(data);

  self->delegate_->Shutdown();
}

bool
Expansive::Impl::Init(VirtualObject* virtual_object)
{
  proxy_ = zwn_shell_get_expansive(
      system_->pimpl->shell(), virtual_object->pimpl->proxy());

  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create expansive proxy");
    return false;
  }

  zwn_expansive_add_listener(proxy_, &Expansive::Impl::listener_, this);

  return true;
}

void
Expansive::Impl::SetRegion(Region* region)
{
  zwn_expansive_set_region(proxy_, region->pimpl->proxy());
}

Expansive::Impl::Impl(System* system, IExpansiveDelegate* delegate)
    : system_(system), delegate_(delegate)
{}

Expansive::Impl::~Impl()
{
  if (proxy_) {
    zwn_expansive_destroy(proxy_);
  }
}

}  // namespace zukou
