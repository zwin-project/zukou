#include <zigen-opengl-client-protocol.h>
#include <zukou.h>

namespace zukou {

std::shared_ptr<OpenGLVertexBuffer> OpenGLVertexBuffer::Create(
    std::shared_ptr<Application> app, size_t size) {
  return std::shared_ptr<OpenGLVertexBuffer>(new OpenGLVertexBuffer(app, size));
}

OpenGLVertexBuffer::OpenGLVertexBuffer(
    std::shared_ptr<Application> app, size_t size)
    : Buffer(app, size) {
  proxy_ = zgn_opengl_create_vertex_buffer(app->opengl_);
  if (proxy_ == nullptr)
    throw ZukouException("failed to create a vertex buffer");

  zgn_opengl_vertex_buffer_attach(proxy_, buffer_proxy_);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
  zgn_opengl_vertex_buffer_destroy(proxy_);
}

void OpenGLVertexBuffer::ReattachBuffer() {
  zgn_opengl_vertex_buffer_attach(proxy_, buffer_proxy_);
}

}  // namespace zukou
