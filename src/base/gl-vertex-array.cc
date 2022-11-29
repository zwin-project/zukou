#include "gl-vertex-array.h"

#include "common.h"
#include "convert.h"
#include "gl-buffer.h"
#include "logger.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT bool
GlVertexArray::Init()
{
  return pimpl->Init();
}

ZUKOU_EXPORT void
GlVertexArray::Enable(GLuint index)
{
  pimpl->Enable(index);
}

ZUKOU_EXPORT void
GlVertexArray::Disable(GLuint index)
{
  pimpl->Disable(index);
}

ZUKOU_EXPORT void
GlVertexArray::VertexAttribPointer(GLuint index, GLint size, GLenum type,
    GLboolean normalized, GLsizei stride, uint64_t offset, GlBuffer *gl_buffer)
{
  pimpl->VertexAttribPointer(
      index, size, type, normalized, stride, offset, gl_buffer);
}

ZUKOU_EXPORT
GlVertexArray::GlVertexArray(System *system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
GlVertexArray::~GlVertexArray() = default;

bool
GlVertexArray::Impl::Init()
{
  proxy_ = zgn_gles_v32_create_gl_vertex_array(system_->pimpl->gles_v32());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create zgn_gl_vertex_array proxy");
    return false;
  }

  return true;
}

void
GlVertexArray::Impl::Enable(GLuint index)
{
  zgn_gl_vertex_array_enable_vertex_attrib_array(proxy_, index);
}

void
GlVertexArray::Impl::Disable(GLuint index)
{
  zgn_gl_vertex_array_disable_vertex_attrib_array(proxy_, index);
}

void
GlVertexArray::Impl::VertexAttribPointer(GLuint index, GLint size, GLenum type,
    GLboolean normalized, GLsizei stride, uint64_t offset, GlBuffer *gl_buffer)
{
  struct wl_array offset_wl_array;

  to_array(offset, &offset_wl_array);

  zgn_gl_vertex_array_vertex_attrib_pointer(proxy_, index, size, type,
      normalized, stride, &offset_wl_array, gl_buffer->pimpl->proxy());

  wl_array_release(&offset_wl_array);
}

GlVertexArray::Impl::Impl(System *system) : system_(system) {}

GlVertexArray::Impl::~Impl()
{
  if (proxy_) {
    zgn_gl_vertex_array_destroy(proxy_);
  }
}

}  // namespace zukou
