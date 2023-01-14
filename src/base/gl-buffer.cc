#include "gl-buffer.h"

#include "buffer.h"
#include "common.h"
#include "logger.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT bool
GlBuffer::Init()
{
  return pimpl->Init();
}

ZUKOU_EXPORT void
GlBuffer::Data(GLenum target, Buffer *buffer, GLenum usage)
{
  pimpl->Data(target, buffer, usage);
}

ZUKOU_EXPORT
GlBuffer::GlBuffer(System *system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
GlBuffer::~GlBuffer() = default;

bool
GlBuffer::Impl::Init()
{
  proxy_ = zwn_gles_v32_create_gl_buffer(system_->pimpl->gles_v32());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to creat zwn_gl_buffer proxy");
    return false;
  }

  return true;
}

void
GlBuffer::Impl::Data(GLenum target, Buffer *buffer, GLenum usage)
{
  zwn_gl_buffer_data(proxy_, target, buffer->pimpl->proxy(), usage);
}

GlBuffer::Impl::Impl(System *system) : system_(system) {}

GlBuffer::Impl::~Impl()
{
  if (proxy_) {
    zwn_gl_buffer_destroy(proxy_);
  }
}

}  // namespace zukou
