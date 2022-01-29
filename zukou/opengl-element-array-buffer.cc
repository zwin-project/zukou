#include <zigen-opengl-client-protocol.h>
#include <zukou.h>

namespace zukou {

std::shared_ptr<OpenGLElementArrayBuffer> OpenGLElementArrayBuffer::Create(
    std::shared_ptr<Application> app, size_t size, uint32_t type) {
  return std::shared_ptr<OpenGLElementArrayBuffer>(
      new OpenGLElementArrayBuffer(app, size, type));
}

OpenGLElementArrayBuffer::OpenGLElementArrayBuffer(
    std::shared_ptr<Application> app, size_t size, uint32_t type)
    : Buffer(app, size), type_(type) {
  proxy_ = zgn_opengl_create_element_array_buffer(app->opengl_);
  if (proxy_ == nullptr)
    throw ZukouException("failed to create a opengl element array buffer");

  zgn_opengl_element_array_buffer_attach(proxy_, buffer_proxy_, type_);
}

OpenGLElementArrayBuffer::~OpenGLElementArrayBuffer() {
  zgn_opengl_element_array_buffer_destroy(proxy_);
}

void OpenGLElementArrayBuffer::ReattachBuffer() {
  zgn_opengl_element_array_buffer_attach(proxy_, buffer_proxy_, type_);
}

}  // namespace zukou
