#pragma once

#include <zukou.h>

#include <zigen-gles-v32-client-protocol.h>

namespace zukou {

class GlVertexArray::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl() = delete;
  Impl(System *system);
  ~Impl();

  bool Init();

  void Enable(GLuint index);

  void Disable(GLuint index);

  void VertexAttribPointer(GLuint index, GLint size, GLenum type,
      GLboolean normalized, GLsizei stride, uint64_t offset,
      GlBuffer *gl_buffer);

  inline zgn_gl_vertex_array *proxy();

 private:
  System *system_;
  zgn_gl_vertex_array *proxy_;
};

inline zgn_gl_vertex_array *
GlVertexArray::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
