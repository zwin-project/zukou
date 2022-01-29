#include "cube.h"

#include <zigen-opengl-client-protocol.h>
#include <zukou.h>

#include <cstring>
#include <glm/gtx/quaternion.hpp>

#include "cube-frag.h"
#include "cube-vert.h"

static const u_short frame_indices[24] = {
    0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7, 0, 2, 1, 3, 4, 6, 5, 7};

struct Vertex {
  glm::vec3 p;
};

Cube::Cube(std::shared_ptr<zukou::Application> app,
    std::shared_ptr<zukou::VirtualObject> virtual_object)
    : size_(0) {
  component_.reset(new zukou::OpenGLComponent(app, virtual_object));

  // shader
  shader_ = zukou::OpenGLShaderProgram::Create(app);
  shader_->SetVertexShader(cube_vertex_shader);
  shader_->SetFragmentShader(cube_fragment_shader);
  shader_->Link();
  shader_->SetUniformVariable("color", glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
  component_->Attach(shader_);

  // element array buffer
  element_array_buffer_ =
      zukou::OpenGLElementArrayBuffer::Create(app, sizeof(frame_indices),
          ZGN_OPENGL_ELEMENT_ARRAY_INDICES_TYPE_UNSIGNED_SHORT);
  {
    u_short *indices = element_array_buffer_->GetData<u_short>();
    memcpy(indices, frame_indices, sizeof(frame_indices));
  }
  component_->Attach(element_array_buffer_);

  // vertex buffer
  vertex_buffer_ = zukou::OpenGLVertexBuffer::Create(app, sizeof(Vertex) * 8);
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
}

void Cube::SetTransform() {
  glm::mat4 transform(1);
  transform = glm::translate(transform, pos_);
  transform = glm::scale(transform, glm::vec3(size_));
  transform = transform * glm::toMat4(quaternion_);
  shader_->SetUniformVariable("transform", transform);
}

void Cube::SetPos(glm::vec3 pos) {
  pos_ = pos;
  this->SetTransform();
  component_->Attach(shader_);
}

void Cube::SetSize(float size) {
  size_ = size;
  this->SetTransform();
  component_->Attach(shader_);
}

void Cube::SetQuaternion(glm::quat quaternion) {
  quaternion_ = quaternion;
  this->SetTransform();
  component_->Attach(shader_);
}

void Cube::SetColor(glm::vec4 color) {
  shader_->SetUniformVariable("color", color);
  component_->Attach(shader_);
}
