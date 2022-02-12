#ifndef ZUKOU_PRIMITIVES_H
#define ZUKOU_PRIMITIVES_H

#include <zukou/zukou.h>

namespace zukou {
namespace primitives {

class IPrimitive {
 public:
  virtual bool Init() = 0;
  virtual bool Draw() = 0; /* return true when its content has changed */
  virtual ~IPrimitive(){};
};

class FrameCuboid : public IPrimitive {
 public:
  FrameCuboid(std::shared_ptr<zukou::Application> app,
      std::shared_ptr<zukou::VirtualObject> virtual_object);

  virtual bool Init() override;
  virtual bool Draw() override;

  inline glm::vec3 position() { return position_; }
  inline glm::vec3 half_size() { return half_size_; };
  inline glm::quat quaternion() { return quaternion_; }
  inline glm::vec4 frame_color() { return frame_color_; }

  void set_position(glm::vec3 position);
  void set_half_size(glm::vec3 half_size);
  void set_quaternion(glm::quat quaternion);
  void set_frame_color(glm::vec4 color);

 private:
  std::shared_ptr<zukou::Application> app_;
  std::shared_ptr<zukou::VirtualObject> virtual_object_;

  bool needs_redraw_;
  glm::vec3 half_size_;
  glm::vec3 position_;
  glm::quat quaternion_;
  glm::vec4 frame_color_;

  std::shared_ptr<zukou::OpenGLComponent> component_;
  std::shared_ptr<zukou::OpenGLShaderProgram> shader_;
  std::shared_ptr<zukou::OpenGLVertexBuffer> vertex_buffer_;
  std::shared_ptr<zukou::OpenGLElementArrayBuffer> element_array_buffer_;
};

class FrameSphere : public IPrimitive {
 public:
  FrameSphere(std::shared_ptr<zukou::Application> app,
      std::shared_ptr<zukou::VirtualObject> virtual_object,
      uint32_t resolution);

  virtual bool Init() override;
  virtual bool Draw() override;

  inline glm::vec3 position() { return position_; }
  inline float radius() { return radius_; }
  inline glm::quat quaternion() { return quaternion_; }
  inline uint32_t resolution() { return resolution_; }
  inline glm::vec4 frame_color() { return frame_color_; }

  void set_position(glm::vec3 position);
  void set_radius(float radius);
  void set_quaternion(glm::quat quaternion);
  void set_resolution(uint32_t resolution);
  void set_frame_color(glm::vec4 color);

 private:
  std::shared_ptr<zukou::OpenGLElementArrayBuffer>
  CreateFrameElementArrayBuffer(uint32_t resolution, uint32_t *count);

  std::shared_ptr<zukou::OpenGLVertexBuffer> CreateVertexBuffer(
      uint32_t resolution);

 private:
  std::shared_ptr<zukou::Application> app_;
  std::shared_ptr<zukou::VirtualObject> virtual_object_;

  bool needs_redraw_;
  float radius_;
  glm::quat quaternion_;
  uint32_t resolution_;
  glm::vec3 position_;
  glm::vec4 frame_color_;

  std::shared_ptr<zukou::OpenGLComponent> component_;
  std::shared_ptr<zukou::OpenGLShaderProgram> shader_;
  std::shared_ptr<zukou::OpenGLVertexBuffer> vertex_buffer_;
  std::shared_ptr<zukou::OpenGLElementArrayBuffer> element_array_buffer_;
};

}  // namespace primitives
}  // namespace zukou

#endif  //  ZUKOU_PRIMITIVES_H
