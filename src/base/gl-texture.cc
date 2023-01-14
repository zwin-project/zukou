#include "gl-texture.h"

#include "buffer.h"
#include "common.h"
#include "logger.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT bool
GlTexture::Init()
{
  return pimpl->Init();
}

ZUKOU_EXPORT void
GlTexture::Image2D(GLenum target, GLint level, GLint internal_format,
    GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
    Buffer *buffer)
{
  pimpl->Image2D(target, level, internal_format, width, height, border, format,
      type, buffer);
}

ZUKOU_EXPORT void
GlTexture::GenerateMipmap(GLenum target)
{
  pimpl->GenerateMipmap(target);
}

ZUKOU_EXPORT
GlTexture::GlTexture(System *system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
GlTexture::~GlTexture() = default;

bool
GlTexture::Impl::Init()
{
  proxy_ = zwn_gles_v32_create_gl_texture(system_->pimpl->gles_v32());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create zwn_gl_texture proxy");
    return false;
  }

  return true;
}

void
GlTexture::Impl::Image2D(GLenum target, GLint level, GLint internal_format,
    GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
    Buffer *buffer)
{
  zwn_gl_texture_image_2d(proxy_, target, level, internal_format, width, height,
      border, format, type, buffer->pimpl->proxy());
}

void
GlTexture::Impl::GenerateMipmap(GLenum target)
{
  zwn_gl_texture_generate_mipmap(proxy_, target);
}

GlTexture::Impl::Impl(System *system) : system_(system) {}

GlTexture::Impl::~Impl()
{
  if (proxy_) {
    zwn_gl_texture_destroy(proxy_);
  }
}

}  // namespace zukou
