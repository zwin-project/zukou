#pragma once

#include <zukou.h>

#include <zwin-gles-v32-client-protocol.h>

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

  inline zwn_gl_vertex_array *proxy();

 private:
  System *system_;
  zwn_gl_vertex_array *proxy_;
};

inline zwn_gl_vertex_array *
GlVertexArray::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
