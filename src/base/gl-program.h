#pragma once

#include <zukou.h>

#include <zigen-gles-v32-client-protocol.h>

namespace zukou {

class GlProgram::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl() = delete;
  Impl(System *system);
  ~Impl();

  bool Init();

  void AttachShader(GlShader *shader);

  void Link();

  inline zgn_gl_program *proxy();

 private:
  System *system_;
  zgn_gl_program *proxy_ = nullptr;
};

inline zgn_gl_program *
GlProgram::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
