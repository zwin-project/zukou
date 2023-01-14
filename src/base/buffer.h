#pragma once

#include <zukou.h>

#include <zwin-client-protocol.h>

namespace zukou {

class Buffer::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl() = delete;
  Impl(IBufferDelegate *delegate);
  ~Impl();

  bool Init(ShmPool *pool, off_t offset, off_t size);

  inline zwn_buffer *proxy();

 private:
  static const zwn_buffer_listener listener_;
  static void HandleRelease(void *data, zwn_buffer *zwn_buffer);

  IBufferDelegate *delegate_;  // nullable

  zwn_buffer *proxy_ = nullptr;
};

inline zwn_buffer *
Buffer::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
