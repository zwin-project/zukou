#pragma once

#include <zukou.h>

#include <zwin-gles-v32-client-protocol.h>

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

  inline zwn_gl_program *proxy();

 private:
  System *system_;
  zwn_gl_program *proxy_ = nullptr;
};

inline zwn_gl_program *
GlProgram::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
