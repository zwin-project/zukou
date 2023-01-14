#include "bounded.h"

#include "common.h"
#include "convert.h"
#include "logger.h"
#include "region.h"
#include "system.h"
#include "virtual-object.h"

namespace zukou {

ZUKOU_EXPORT bool
Bounded::Init(const glm::vec3 &half_size)
{
  if (!VirtualObject::Init()) return false;

  return pimpl->Init(this, half_size);
}

ZUKOU_EXPORT void
Bounded::AckConfigure(uint32_t serial)
{
  pimpl->AckConfigure(serial);
}

ZUKOU_EXPORT void
Bounded::SetTitle(const std::string &title)
{
  pimpl->SetTitle(title);
}

ZUKOU_EXPORT void
Bounded::SetRegion(Region *region)
{
  pimpl->SetRegion(region);
}

ZUKOU_EXPORT void
Bounded::Move(uint32_t serial)
{
  pimpl->Move(serial);
}

ZUKOU_EXPORT
Bounded::Bounded(System *system, IBoundedDelegate *delegate)
    : VirtualObject(system, delegate), pimpl(new Impl(system, delegate))
{}

ZUKOU_EXPORT
Bounded::~Bounded() = default;

const zgn_bounded_listener Bounded::Impl::listener_ = {
    Bounded::Impl::HandleConfigure,
};

void
Bounded::Impl::HandleConfigure(void *data, zgn_bounded * /*zgn_bounded*/,
    struct wl_array *half_size, uint32_t serial)
{
  auto self = static_cast<Bounded::Impl *>(data);

  glm::vec3 half_size_vec;
  to_vec3(half_size, half_size_vec);

  self->delegate_->Configure(half_size_vec, serial);
}

bool
Bounded::Impl::Init(VirtualObject *virtual_object, const glm::vec3 &half_size)
{
  wl_array array;
  to_array(half_size, &array);

  proxy_ = zgn_shell_get_bounded(
      system_->pimpl->shell(), virtual_object->pimpl->proxy(), &array);
  wl_array_release(&array);

  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create bounded proxy");
    return false;
  }

  zgn_bounded_add_listener(proxy_, &Bounded::Impl::listener_, this);

  return true;
}

void
Bounded::Impl::AckConfigure(uint32_t serial)
{
  zgn_bounded_ack_configure(proxy_, serial);
}

void
Bounded::Impl::SetTitle(const std::string &title)
{
  zgn_bounded_set_title(proxy_, title.c_str());
}

void
Bounded::Impl::SetRegion(Region *region)
{
  zgn_bounded_set_region(proxy_, region->pimpl->proxy());
}

void
Bounded::Impl::Move(uint32_t serial)
{
  zgn_bounded_move(proxy_, system_->pimpl->seat(), serial);
}

Bounded::Impl::Impl(System *system, IBoundedDelegate *delegate)
    : system_(system), delegate_(delegate)
{}

Bounded::Impl::~Impl()
{
  if (proxy_) {
    zgn_bounded_destroy(proxy_);
  }
}

}  // namespace zukou
