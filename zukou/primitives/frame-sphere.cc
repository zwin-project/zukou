#include <zigen-opengl-client-protocol.h>
#include <zukou/zukou.h>

#include <glm/gtx/quaternion.hpp>

#include "primitive-frame-sphere-frag.h"
#include "primitive-frame-sphere-vert.h"

struct Vertex {
  glm::vec3 p;
  glm::vec3 n;
  glm::vec2 uv;
};

namespace zukou {
namespace primitives {

FrameSphere::FrameSphere(std::shared_ptr<zukou::Application> app,
    std::shared_ptr<zukou::VirtualObject> virtual_object, uint32_t resolution)
    : app_(app),
      virtual_object_(virtual_object),
      needs_redraw_(false),
      resolution_(resolution),
      frame_color_(1.0) {}

bool FrameSphere::Init() {
  component_.reset(new zukou::OpenGLComponent(app_, virtual_object_));
  uint32_t element_count;

  // shader
  shader_ = zukou::OpenGLShaderProgram::Create(app_);
  shader_->SetVertexShader(primitive_frame_sphere_vertex_shader);
  shader_->SetFragmentShader(primitive_frame_sphere_fragment_shader);
  shader_->Link();
  shader_->SetUniformVariable("color", frame_color_);
  component_->Attach(shader_);

  // element array buffer
  element_array_buffer_ =
      CreateFrameElementArrayBuffer(resolution_, &element_count);
  component_->Attach(element_array_buffer_);

  vertex_buffer_ = CreateVertexBuffer(resolution_);
  component_->Attach(vertex_buffer_);

  // component
  component_->SetCount(element_count);
  component_->SetTopology(ZGN_OPENGL_TOPOLOGY_LINES);
  component_->AddVertexAttribute(
      0, 3, ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(Vertex), 0);
  component_->AddVertexAttribute(1, 3, ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT,
      false, sizeof(Vertex), offsetof(Vertex, n));
  component_->AddVertexAttribute(2, 2, ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT,
      false, sizeof(Vertex), offsetof(Vertex, uv));

  needs_redraw_ = true;

  return true;
}

bool FrameSphere::Draw() {
  if (!needs_redraw_) return false;
  glm::mat4 transform(1);
  transform = glm::translate(transform, position_);
  transform = transform * glm::toMat4(quaternion_);
  transform = glm::scale(transform, glm::vec3(radius_));
  shader_->SetUniformVariable("transform", transform);
  shader_->SetUniformVariable("color", frame_color_);
  component_->Attach(shader_);
  needs_redraw_ = false;
  return true;
}

void FrameSphere::set_position(glm::vec3 position) {
  if (position_ == position) return;
  position_ = position;
  needs_redraw_ = true;
}

void FrameSphere::set_radius(float radius) {
  if (radius_ == radius) return;
  radius_ = radius;
  needs_redraw_ = true;
}

void FrameSphere::set_quaternion(glm::quat quaternion) {
  if (quaternion_ == quaternion) return;
  quaternion_ = quaternion;
  needs_redraw_ = true;
}

void FrameSphere::set_resolution(uint32_t resolution) {
  if (resolution_ == resolution) return;
  resolution_ = resolution;
  needs_redraw_ = true;
}

void FrameSphere::set_frame_color(glm::vec4 color) {
  if (frame_color_ == color) return;
  frame_color_ = color;
  needs_redraw_ = true;
}

std::shared_ptr<zukou::OpenGLElementArrayBuffer>
FrameSphere::CreateFrameElementArrayBuffer(
    uint32_t resolution, uint32_t *count) {
  uint32_t longitude_count = (4 * resolution) * (resolution * 2);
  uint32_t latitude_count = (2 * resolution - 1) * (4 * resolution);
  *count = (longitude_count + latitude_count) * 2;
  auto element_array_buffer =
      zukou::OpenGLElementArrayBuffer::Create(app_, sizeof(ushort) * *count,
          ZGN_OPENGL_ELEMENT_ARRAY_INDICES_TYPE_UNSIGNED_SHORT);

  int i = 0;
  ushort *indices = element_array_buffer->GetData<ushort>();
  uint32_t last_index = (4 * resolution + 1) * (2 * resolution - 1) + 1;

  // clang-format off
  // longitude
  for (uint32_t j = 0; j < 4 * resolution; j++) {
    // top
    indices[i] = 0; i++;
    indices[i] = j + 1; i++;

    for (uint32_t k = 0; k < 2 * resolution - 2; k++) {
      indices[i] = indices[i-1]; i++;
      indices[i] = indices[i-1] + (4 * resolution + 1); i++;
    }

    // bottom
    indices[i] = indices[i-1]; i++;
    indices[i] = last_index; i++;
  }

  // latitude
  for (uint32_t j = 0; j < 2 * resolution - 1; j++) {
    for (uint32_t k = 0; k < 4 * resolution; k++) {
      indices[i] = (4 * resolution + 1) * j + k + 1; i++;
      indices[i] = (4 * resolution + 1) * j + k + 2; i++;
    }
  }
  // clang-format on

  return element_array_buffer;
}

/**
 * Create uv sphere vertices
 * 4 * resolution lines for longitude
 * (2 * resolution - 1) lines for latitude
 */
std::shared_ptr<zukou::OpenGLVertexBuffer> FrameSphere::CreateVertexBuffer(
    uint32_t resolution) {
  uint32_t vertex_count = (4 * resolution + 1) * (2 * resolution - 1) + 2;
  auto vertex_buffer =
      zukou::OpenGLVertexBuffer::Create(app_, sizeof(Vertex) * vertex_count);

  Vertex *vertices = vertex_buffer->GetData<Vertex>();

  int i = 0;
  float delta = M_PI_2 / resolution;

  // top
  vertices[i].p = glm::vec3(0, 1, 0);
  vertices[i].n = vertices[i].p;
  vertices[i].uv = glm::vec2(0.5, 0);
  i++;

  for (int j = resolution - 1; j > -(int)resolution; j--) {
    float theta = delta * j;
    float y = sin(theta);
    float cos_theta = cos(theta);
    float v = (float)(-j + resolution) / (float)(2 * resolution);
    for (int k = 0; k < (int)(4 * resolution + 1); k++) {
      float phi = delta * k;
      float x = -cos_theta * sin(phi);
      float z = -cos_theta * cos(phi);
      float u = (float)k / (float)(2 * resolution);
      vertices[i].p = glm::vec3(x, y, z);
      vertices[i].n = vertices[i].p;
      vertices[i].uv = glm::vec2(u, v);
      i++;
    }
  }

  // bottom
  vertices[i].p = glm::vec3(0, -1, 0);
  vertices[i].n = vertices[i].p;
  vertices[i].uv = glm::vec2(0.5, 1);

  return vertex_buffer;
}

}  // namespace primitives
}  // namespace zukou
