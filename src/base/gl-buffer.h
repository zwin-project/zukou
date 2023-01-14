#pragma once

#include <zukou.h>

#include <zwin-gles-v32-client-protocol.h>

namespace zukou {

class GlBuffer::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl() = delete;
  Impl(System* system);
  ~Impl();

  bool Init();

  void Data(GLenum target, Buffer* buffer, GLenum usage);

  inline zwn_gl_buffer* proxy();

 private:
  System* system_;
  zwn_gl_buffer* proxy_ = nullptr;
};

inline zwn_gl_buffer*
GlBuffer::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
