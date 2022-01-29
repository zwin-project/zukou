#ifndef ZUKOU_SAMPLE_BOX_H
#define ZUKOU_SAMPLE_BOX_H

#include <zukou.h>

class Box : public zukou::CuboidWindow {
 public:
  static std::shared_ptr<Box> Create(
      std::shared_ptr<zukou::Application> app, glm::vec3 half_size);

 private:
  Box(std::shared_ptr<zukou::Application> app, glm::vec3 half_size);

  void RayButton(
      uint32_t serial, uint32_t time, uint32_t button, bool pressed) override;

  void Configured(uint32_t serial) override;

 private:
  std::shared_ptr<zukou::OpenGLComponent> component_;
  std::shared_ptr<zukou::OpenGLShaderProgram> shader_;
  std::shared_ptr<zukou::OpenGLVertexBuffer> vertex_buffer_;
  std::shared_ptr<zukou::OpenGLElementArrayBuffer> element_array_buffer_;
};

#endif  //  ZUKOU_SAMPLE_BOX_H
