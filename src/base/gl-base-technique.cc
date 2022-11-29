#include "gl-base-technique.h"

#include <cstring>

#include "common.h"
#include "convert.h"
#include "gl-buffer.h"
#include "gl-program.h"
#include "gl-texture.h"
#include "gl-vertex-array.h"
#include "logger.h"
#include "rendering-unit.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT bool
GlBaseTechnique::Init(RenderingUnit *unit)
{
  return pimpl->Init(unit);
}

ZUKOU_EXPORT void
GlBaseTechnique::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
  pimpl->DrawArrays(mode, first, count);
}

ZUKOU_EXPORT void
GlBaseTechnique::DrawElements(GLenum mode, GLsizei count, GLenum type,
    uint64_t offset, GlBuffer *element_array_buffer)
{
  pimpl->DrawElements(mode, count, type, offset, element_array_buffer);
}

ZUKOU_EXPORT void
GlBaseTechnique::Bind(GlProgram *program)
{
  pimpl->Bind(program);
}

ZUKOU_EXPORT void
GlBaseTechnique::Bind(GlVertexArray *vertex_array)
{
  pimpl->Bind(vertex_array);
}

ZUKOU_EXPORT void
GlBaseTechnique::Bind(
    uint32_t binding, std::string name, GlTexture *texture, GLenum target)
{
  pimpl->Bind(binding, name, texture, target);
}

ZUKOU_EXPORT void
GlBaseTechnique::UniformVector(uint32_t location, const std::string &name,
    enum UniformVariableType type, uint32_t size, uint32_t count, void *value)
{
  pimpl->UniformVector(location, name, type, size, count, value);
}

ZUKOU_EXPORT void
GlBaseTechnique::UniformMatrix(uint32_t location, const std::string &name,
    uint32_t col, uint32_t row, uint32_t count, bool transpose, void *value)
{
  pimpl->UniformMatrix(location, name, col, row, count, transpose, value);
}

ZUKOU_EXPORT
GlBaseTechnique::GlBaseTechnique(System *system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
GlBaseTechnique::~GlBaseTechnique() = default;

bool
GlBaseTechnique::Impl::Init(RenderingUnit *unit)
{
  proxy_ = zgn_gles_v32_create_gl_base_technique(
      system_->pimpl->gles_v32(), unit->pimpl->proxy());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create base technique proxy");
    return false;
  }

  return true;
}

void
GlBaseTechnique::Impl::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
  zgn_gl_base_technique_draw_arrays(proxy_, mode, first, count);
}

void
GlBaseTechnique::Impl::DrawElements(GLenum mode, GLsizei count, GLenum type,
    uint64_t offset, GlBuffer *element_array_buffer)
{
  struct wl_array offset_wl_array;

  to_array(offset, &offset_wl_array);

  zgn_gl_base_technique_draw_elements(proxy_, mode, count, type,
      &offset_wl_array, element_array_buffer->pimpl->proxy());

  wl_array_release(&offset_wl_array);
}

void
GlBaseTechnique::Impl::Bind(GlProgram *program)
{
  zgn_gl_base_technique_bind_program(proxy_, program->pimpl->proxy());
}

void
GlBaseTechnique::Impl::Bind(GlVertexArray *vertex_array)
{
  zgn_gl_base_technique_bind_vertex_array(proxy_, vertex_array->pimpl->proxy());
}

void
GlBaseTechnique::Impl::Bind(
    uint32_t binding, std::string name, GlTexture *texture, GLenum target)
{
  zgn_gl_base_technique_bind_texture(
      proxy_, binding, name.c_str(), texture->pimpl->proxy(), target);
}

void
GlBaseTechnique::Impl::UniformVector(uint32_t location, const std::string &name,
    enum UniformVariableType type, uint32_t size, uint32_t count, void *value)
{
  wl_array array;

  enum zgn_gl_base_technique_uniform_variable_type zgn_type;
  switch (type) {
    case kFloat:
      zgn_type = ZGN_GL_BASE_TECHNIQUE_UNIFORM_VARIABLE_TYPE_FLOAT;
      break;
    case kInt:
      zgn_type = ZGN_GL_BASE_TECHNIQUE_UNIFORM_VARIABLE_TYPE_INT;
      break;
    case kUint:
      zgn_type = ZGN_GL_BASE_TECHNIQUE_UNIFORM_VARIABLE_TYPE_UINT;
      break;
  }

  wl_array_init(&array);

  {
    size_t value_size = 4 * size * count;
    void *data = wl_array_add(&array, value_size);
    std::memcpy(data, value, value_size);
  }

  zgn_gl_base_technique_uniform_vector(
      proxy_, location, name.c_str(), zgn_type, size, count, &array);

  wl_array_release(&array);
}

void
GlBaseTechnique::Impl::UniformMatrix(uint32_t location, const std::string &name,
    uint32_t col, uint32_t row, uint32_t count, bool transpose, void *value)
{
  wl_array array;
  wl_array_init(&array);

  {
    size_t value_size = sizeof(float) * col * row * count;
    void *data = wl_array_add(&array, value_size);
    std::memcpy(data, value, value_size);
  }

  zgn_gl_base_technique_uniform_matrix(
      proxy_, location, name.c_str(), col, row, count, transpose, &array);

  wl_array_release(&array);
}

GlBaseTechnique::Impl::Impl(System *system) : system_(system) {}

GlBaseTechnique::Impl::~Impl()
{
  if (proxy_) {
    zgn_gl_base_technique_destroy(proxy_);
  }
}

}  // namespace zukou
