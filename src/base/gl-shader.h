#pragma once

#include <zukou.h>

#include <zigen-gles-v32-client-protocol.h>

namespace zukou {

class GlShader::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl() = delete;
  Impl(System* system);
  ~Impl();

  bool Init(GLenum type, const std::string& source);

  inline zgn_gl_shader* proxy();

 private:
  System* system_;
  zgn_gl_shader* proxy_ = nullptr;

  ShmPool pool_;
  Buffer buffer_;
  int fd_ = 0;
};

inline zgn_gl_shader*
GlShader::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
