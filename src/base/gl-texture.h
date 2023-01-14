#pragma once

#include <zukou.h>

#include <zwin-gles-v32-client-protocol.h>

namespace zukou {

class GlTexture::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl() = delete;
  Impl(System *system);
  ~Impl();

  bool Init();

  void Image2D(GLenum target, GLint level, GLint internal_format, GLsizei width,
      GLsizei height, GLint border, GLenum format, GLenum type, Buffer *buffer);

  void GenerateMipmap(GLenum target);

  inline zwn_gl_texture *proxy();

 private:
  System *system_;
  zwn_gl_texture *proxy_;
};

inline zwn_gl_texture *
GlTexture::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
