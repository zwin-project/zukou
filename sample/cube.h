#ifndef ZUKOU_SAMPLE_CUBE_H
#define ZUKOU_SAMPLE_CUBE_H

#include <zukou/zukou.h>

class Cube {
 public:
  Cube(std::shared_ptr<zukou::Application> app,
      std::shared_ptr<zukou::VirtualObject> virtual_object);

  void SetPos(glm::vec3 pos);
  void SetSize(float size);
  void SetQuaternion(glm::quat quaternion);
  void SetColor(glm::vec4 color);
  inline float size() { return size_; };
  inline glm::vec3 pos() { return pos_; };
  inline glm::quat quaternion() { return quaternion_; };

 private:
  void SetTransform();

  float size_;
  glm::vec3 pos_;
  glm::quat quaternion_;

  std::shared_ptr<zukou::OpenGLComponent> component_;
  std::shared_ptr<zukou::OpenGLShaderProgram> shader_;
  std::shared_ptr<zukou::OpenGLVertexBuffer> vertex_buffer_;
  std::shared_ptr<zukou::OpenGLElementArrayBuffer> element_array_buffer_;
};

#endif  //  ZUKOU_SAMPLE_CUBE_H
