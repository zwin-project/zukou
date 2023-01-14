#pragma once

#include <zukou.h>

#include <zwin-gles-v32-client-protocol.h>

namespace zukou {

class GlShader::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl() = delete;
  Impl(System* system);
  ~Impl();

  bool Init(GLenum type, const std::string& source);

  inline zwn_gl_shader* proxy();

 private:
  System* system_;
  zwn_gl_shader* proxy_ = nullptr;

  ShmPool pool_;
  Buffer buffer_;
  int fd_ = 0;
};

inline zwn_gl_shader*
GlShader::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
