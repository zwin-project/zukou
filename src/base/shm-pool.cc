#include "shm-pool.h"

#include "common.h"
#include "convert.h"
#include "logger.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT bool
ShmPool::Init(int fd, off_t size)
{
  return pimpl->Init(fd, size);
}

ZUKOU_EXPORT
ShmPool::ShmPool(System *system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
ShmPool::~ShmPool() = default;

bool
ShmPool::Impl::Init(int fd, off_t size)
{
  wl_array array;
  to_array(size, &array);

  proxy_ = zgn_shm_create_pool(system_->pimpl->shm(), fd, &array);
  wl_array_release(&array);

  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create shm pool proxy");
    return false;
  }

  return true;
}

ShmPool::Impl::Impl(System *system) : system_(system) {}

ShmPool::Impl::~Impl()
{
  if (proxy_) {
    zgn_shm_pool_destroy(proxy_);
  }
}

}  // namespace zukou
