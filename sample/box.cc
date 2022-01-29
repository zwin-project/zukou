#include "box.h"

#include <zigen-opengl-client-protocol.h>

#include <cstring>
#include <vector>

extern std::string vertex_shader;
extern std::string fragment_shader;

std::shared_ptr<Box> Box::Create(
    std::shared_ptr<zukou::Application> app, glm::vec3 half_size) {
  return std::shared_ptr<Box>(new Box(app, half_size));
}

struct Vertex {
  glm::vec3 point;
};

Box::Box(std::shared_ptr<zukou::Application> app, glm::vec3 half_size)
    : CuboidWindow(app, half_size) {}

void Box::Configured([[maybe_unused]] uint32_t serial) {
  auto self = std::dynamic_pointer_cast<Box>(shared_from_this());
  const int vertex_count = 8;

  float min_half =
      half_size().x > half_size().y
          ? (half_size().y > half_size().z ? half_size().z : half_size().y)
          : (half_size().x > half_size().z ? half_size().z : half_size().x);
  float edge_length = min_half / 1.8f;

  vertex_buffer_ =
      zukou::OpenGLVertexBuffer::Create(app_, vertex_count * sizeof(Vertex));
  element_array_buffer_ =
      zukou::OpenGLElementArrayBuffer::Create(app_, sizeof(u_short) * 24,
          ZGN_OPENGL_ELEMENT_ARRAY_INDICES_TYPE_UNSIGNED_SHORT);
  shader_ = zukou::OpenGLShaderProgram::Create(app_);
  component_.reset(new zukou::OpenGLComponent(app_, self));

  {
    Vertex *vertices;
    vertices = vertex_buffer_->GetData<Vertex>();

    int i = 0;
    for (int x = -1; x < 2; x += 2) {
      for (int y = -1; y < 2; y += 2) {
        for (int z = -1; z < 2; z += 2) {
          vertices[i].point.x = edge_length * x;
          vertices[i].point.y = edge_length * y;
          vertices[i].point.z = edge_length * z;
          i++;
        }
      }
    }
  }

  {
    u_short frame_indices[24] = {
        0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7, 0, 2, 1, 3, 4, 6, 5, 7};
    u_short *indices = element_array_buffer_->GetData<u_short>();
    memcpy(indices, frame_indices, sizeof(frame_indices));
  }

  shader_->SetVertexShader(vertex_shader);
  shader_->SetFragmentShader(fragment_shader);
  shader_->Link();
  shader_->SetUniformVariable("color", glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

  component_->Attach(shader_);
  component_->Attach(vertex_buffer_);
  component_->Attach(element_array_buffer_);
  component_->SetCount(24);
  component_->SetTopology(ZGN_OPENGL_TOPOLOGY_LINES);
  component_->AddVertexAttribute(
      0, 3, ZGN_OPENGL_VERTEX_ATTRIBUTE_TYPE_FLOAT, false, sizeof(Vertex), 0);

  this->Commit();
}

void Box::RayButton(uint32_t serial, [[maybe_unused]] uint32_t time,
    [[maybe_unused]] uint32_t button, [[maybe_unused]] bool pressed) {
  if (pressed) this->Move(serial);
}

std::string vertex_shader =
    "#version 410\n"
    "uniform mat4 zMVP;\n"
    "layout(location = 0) in vec4 position;\n"
    "layout(location = 1) in vec2 v2UVcoordsIn;\n"
    "out vec2 v2UVcoords;\n"
    "void main()\n"
    "{\n"
    "  v2UVcoords = v2UVcoordsIn;\n"
    "  gl_Position = zMVP * position;\n"
    "}\n";

std::string fragment_shader =
    "#version 410 core\n"
    "uniform vec4 color;\n"
    "out vec4 outputColor;\n"
    "void main()\n"
    "{\n"
    "  outputColor = color;\n"
    "}\n";
