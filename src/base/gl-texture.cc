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

ZUKOU_EXPORT
GlTexture::GlTexture(System *system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
GlTexture::~GlTexture() = default;

bool
GlTexture::Impl::Init()
{
  proxy_ = zgn_gles_v32_create_gl_texture(system_->pimpl->gles_v32());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create zgn_gl_texture proxy");
    return false;
  }

  return true;
}

void
GlTexture::Impl::Image2D(GLenum target, GLint level, GLint internal_format,
    GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
    Buffer *buffer)
{
  zgn_gl_texture_image_2d(proxy_, target, level, internal_format, width, height,
      border, format, type, buffer->pimpl->proxy());
}

GlTexture::Impl::Impl(System *system) : system_(system) {}

GlTexture::Impl::~Impl()
{
  if (proxy_) {
    zgn_gl_texture_destroy(proxy_);
  }
}

}  // namespace zukou
