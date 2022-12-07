#pragma once

#include <zukou.h>

#include <zigen-gles-v32-client-protocol.h>

namespace zukou {

class GlSampler::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl() = delete;
  Impl(System *system);
  ~Impl();

  bool Init();

  void Parameteri(GLenum pname, GLint param);

  void Parameterf(GLenum pname, GLfloat param);

  inline zgn_gl_sampler *proxy();

 private:
  System *system_;
  zgn_gl_sampler *proxy_ = nullptr;
};

inline zgn_gl_sampler *
GlSampler::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
