#include <zigen-opengl-client-protocol.h>
#include <zukou/zukou.h>

namespace zukou {

OpenGLComponent::OpenGLComponent(std::shared_ptr<Application> app,
    std::shared_ptr<VirtualObject> virtual_object) {
  proxy_ = zgn_opengl_create_opengl_component(
      app->opengl_, virtual_object->virtual_object_proxy_);
  if (proxy_ == nullptr)
    throw ZukouException("failed to create a opengl component");

  zgn_opengl_component_set_user_data(proxy_, this);
}

OpenGLComponent::~OpenGLComponent() { zgn_opengl_component_destroy(proxy_); }

void OpenGLComponent::Attach(
    std::shared_ptr<OpenGLVertexBuffer> vertex_buffer) {
  zgn_opengl_component_attach_vertex_buffer(proxy_, vertex_buffer->proxy_);
}

void OpenGLComponent::Attach(
    std::shared_ptr<OpenGLElementArrayBuffer> element_array_buffer) {
  zgn_opengl_component_attach_element_array_buffer(
      proxy_, element_array_buffer->proxy_);
}

void OpenGLComponent::Attach(
    std::shared_ptr<OpenGLShaderProgram> shader_program) {
  zgn_opengl_component_attach_shader_program(proxy_, shader_program->proxy_);
}

void OpenGLComponent::Attach(std::shared_ptr<OpenGLTexture> texture) {
  zgn_opengl_component_attach_texture(proxy_, texture->proxy_);
}

void OpenGLComponent::SetMin(uint32_t min) {
  zgn_opengl_component_set_min(proxy_, min);
}

void OpenGLComponent::SetCount(uint32_t count) {
  zgn_opengl_component_set_count(proxy_, count);
}

void OpenGLComponent::SetTopology(uint32_t topology) {
  zgn_opengl_component_set_topology(proxy_, topology);
}

void OpenGLComponent::AddVertexAttribute(uint32_t index, uint32_t size,
    uint16_t type, uint32_t normalized, uint32_t stride, uint32_t pointer) {
  zgn_opengl_component_add_vertex_attribute(
      proxy_, index, size, type, normalized, stride, pointer);
}

}  // namespace zukou
