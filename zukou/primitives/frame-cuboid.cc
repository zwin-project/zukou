#include <zigen-opengl-client-protocol.h>
#include <zukou/zukou.h>

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "primitive-frame-cuboid-frag.h"
#include "primitive-frame-cuboid-vert.h"

struct Vertex {
  glm::vec3 p;
};

static const u_short frame_indices[24] = {
    0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7, 0, 2, 1, 3, 4, 6, 5, 7};

namespace zukou {

namespace primitives {

FrameCuboid::FrameCuboid(std::shared_ptr<zukou::Application> app,
    std::shared_ptr<zukou::VirtualObject> virtual_object)
    : app_(app),
      virtual_object_(virtual_object),
      needs_redraw_(false),
      frame_color_(1.0) {}

bool FrameCuboid::Init() {
  component_.reset(new zukou::OpenGLComponent(app_, virtual_object_));

  // shader
  shader_ = zukou::OpenGLShaderProgram::Create(app_);
  shader_->SetVertexShader(primitive_frame_cuboid_vertex_shader);
  shader_->SetFragmentShader(primitive_frame_cuboid_fragment_shader);
  shader_->Link();
  shader_->SetUniformVariable("color", glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
  component_->Attach(shader_);

  // element array buffer
  element_array_buffer_ =
      zukou::OpenGLElementArrayBuffer::Create(app_, sizeof(frame_indices),
          ZGN_OPENGL_ELEMENT_ARRAY_INDICES_TYPE_UNSIGNED_SHORT);
  {
    u_short *indices = element_array_buffer_->GetData<u_short>();
    memcpy(indices, frame_indices, sizeof(frame_indices));
  }
  component_->Attach(element_array_buffer_);

  // vertex buffer
  vertex_buffer_ = zukou::OpenGLVertexBuffer::Create(app_, sizeof(Vertex) * 8);
  {
    Vertex *vertices = vertex_buffer_->GetData<Vertex>();
    int i = 0;
    for (int x = -1; x < 2; x += 2) {
      for (int y = -1; y < 2; y += 2) {
        for (int z = -1; z < 2; z += 2) {
          vertices[i].p.x = x;
          vertices[i].p.y = y;
          vertices[i].p.z = z;
          i++;
        }
      }
    }
  }
  component_->Attach(vertex_buffer_);

  // component
  component_->SetCount(24);
  component_->SetTopology(ZGN_OPENGL_TOPOLOGY_LINES);
  component_->AddVertexAttribute(
      0, 3, ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(Vertex), 0);

  needs_redraw_ = true;

  return true;
}

bool FrameCuboid::Draw() {
  if (!needs_redraw_) return false;
  glm::mat4 transform(1);
  transform = glm::translate(transform, position_);
  transform = glm::scale(transform, glm::vec3(size_));
  transform = transform * glm::toMat4(quaternion_);
  shader_->SetUniformVariable("transform", transform);
  shader_->SetUniformVariable("color", frame_color_);
  component_->Attach(shader_);
  needs_redraw_ = false;
  return true;
}

void FrameCuboid::set_position(glm::vec3 position) {
  position_ = position;
  needs_redraw_ = true;
}

void FrameCuboid::set_size(glm::vec3 size) {
  size_ = size;
  needs_redraw_ = true;
}

void FrameCuboid::set_quaternion(glm::quat quaternion) {
  quaternion_ = quaternion;
  needs_redraw_ = true;
}

void FrameCuboid::set_frame_color(glm::vec4 color) {
  frame_color_ = color;
  needs_redraw_ = true;
}

}  // namespace primitives
}  // namespace zukou
