#pragma once

#include <zukou.h>

#include <zigen-client-protocol.h>

namespace zukou {

class Buffer::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl() = delete;
  Impl(IBufferDelegate *delegate);
  ~Impl();

  bool Init(ShmPool *pool, off_t offset, off_t size);

  inline zgn_buffer *proxy();

 private:
  static const zgn_buffer_listener listener_;
  static void HandleRelease(void *data, zgn_buffer *zgn_buffer);

  IBufferDelegate *delegate_;  // nullable

  zgn_buffer *proxy_ = nullptr;
};

inline zgn_buffer *
Buffer::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
