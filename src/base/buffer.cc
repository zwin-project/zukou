#include "buffer.h"

#include "common.h"
#include "convert.h"
#include "logger.h"
#include "shm-pool.h"

namespace zukou {

ZUKOU_EXPORT bool
Buffer::Init(ShmPool *pool, off_t offset, off_t size)
{
  return pimpl->Init(pool, offset, size);
}

ZUKOU_EXPORT
Buffer::Buffer(IBufferDelegate *delegate) : pimpl(new Impl(delegate)) {}

ZUKOU_EXPORT
Buffer::~Buffer() = default;

const zwn_buffer_listener Buffer::Impl::listener_ = {
    Buffer::Impl::HandleRelease,
};

void
Buffer::Impl::HandleRelease(void *data, zwn_buffer * /*zwn_buffer*/)
{
  auto self = static_cast<Buffer::Impl *>(data);

  if (self->delegate_) self->delegate_->Release();
}

bool
Buffer::Impl::Init(ShmPool *pool, off_t offset, off_t size)
{
  struct wl_array offset_wl_array, size_wl_array;

  to_array(offset, &offset_wl_array);
  to_array(size, &size_wl_array);

  proxy_ = zwn_shm_pool_create_buffer(
      pool->pimpl->proxy(), &offset_wl_array, &size_wl_array);

  wl_array_release(&offset_wl_array);
  wl_array_release(&size_wl_array);

  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create zwn_buffer proxy");
    return false;
  }

  zwn_buffer_add_listener(proxy_, &Buffer::Impl::listener_, this);

  return true;
}

Buffer::Impl::Impl(IBufferDelegate *delegate) : delegate_(delegate) {}

Buffer::Impl::~Impl()
{
  if (proxy_) {
    zwn_buffer_destroy(proxy_);
  }
}

}  // namespace zukou
