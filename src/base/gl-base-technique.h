#pragma once

#include <zukou.h>

#include <zigen-gles-v32-client-protocol.h>

namespace zukou {

class GlBaseTechnique::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(System *system);
  ~Impl();

  bool Init(RenderingUnit *unit);

  void DrawArrays(GLenum mode, GLint first, GLsizei count);

  void DrawElements(GLenum mode, GLsizei count, GLenum type, uint64_t offset,
      GlBuffer *element_array_buffer);

  void Bind(GlProgram *program);

  void Bind(GlVertexArray *vertex_array);

  void Bind(uint32_t binding, std::string name, GlTexture *texture,
      GLenum target, GlSampler *sampler);

  void UniformVector(uint32_t location, const std::string &name,
      enum UniformVariableType type, uint32_t size, uint32_t count,
      void *value);

  void UniformMatrix(uint32_t location, const std::string &name, uint32_t col,
      uint32_t row, uint32_t count, bool transpose, void *value);

 private:
  System *system_;

  zgn_gl_base_technique *proxy_ = nullptr;
};

}  // namespace zukou
