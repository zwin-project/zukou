#include "sphere.h"

#include <sys/mman.h>
#include <unistd.h>

#include <cstring>

#include "color.fragment.h"
#include "default.vert.h"
#include "texture.fragment.h"

Sphere::Sphere(zukou::System* system, zukou::VirtualObject* virtual_object,
    int32_t resolution, bool wire)
    : virtual_object_(virtual_object),
      resolution_(resolution),
      wire_(wire),
      pool_(system),
      gl_vertex_buffer_(system),
      gl_element_array_buffer_(system),
      vertex_array_(system),
      vertex_shader_(system),
      fragment_shader_(system),
      program_(system),
      sampler_(system),
      rendering_unit_(system),
      base_technique_(system)
{}

Sphere::~Sphere()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

Sphere::Vertex::Vertex(float x, float y, float z, float u, float v)
    : x(x), y(y), z(z), u(u), v(v)
{}

bool
Sphere::Render(float radius, glm::mat4 transform)
{
  if (!initialized_ && Init() == false) {
    return false;
  }

  auto local_model = glm::scale(transform, glm::vec3(radius));
  base_technique_.Uniform(0, "local_model", local_model);

  if (wire_) base_technique_.Uniform(0, "color", glm::vec4(1, 0, 0, 1));

  return true;
}

void
Sphere::Bind(std::unique_ptr<zukou::GlTexture> texture)
{
  texture_ = std::move(texture);
}

bool
Sphere::Init()
{
  ConstructVertices();
  ConstructElements();

  const char* fragment_shader_source =
      wire_ ? color_fragment_shader_source : texture_fragment_shader_source;

  fd_ = zukou::Util::CreateAnonymousFile(pool_size());
  if (!pool_.Init(fd_, pool_size())) return false;
  if (!vertex_buffer_.Init(&pool_, 0, vertex_buffer_size())) return false;
  if (!element_array_buffer_.Init(
          &pool_, vertex_buffer_size(), element_array_buffer_size()))
    return false;

  if (!gl_vertex_buffer_.Init()) return false;
  if (!gl_element_array_buffer_.Init()) return false;
  if (!vertex_array_.Init()) return false;
  if (!vertex_shader_.Init(GL_VERTEX_SHADER, default_vertex_shader_source))
    return false;
  if (!fragment_shader_.Init(GL_FRAGMENT_SHADER, fragment_shader_source))
    return false;
  if (!program_.Init()) return false;
  if (!sampler_.Init()) return false;

  if (!rendering_unit_.Init(virtual_object_)) return false;
  if (!base_technique_.Init(&rendering_unit_)) return false;

  {
    auto vertex_buffer_data = static_cast<char*>(
        mmap(nullptr, pool_size(), PROT_WRITE, MAP_SHARED, fd_, 0));
    auto element_array_buffer_data = vertex_buffer_data + vertex_buffer_size();

    std::memcpy(vertex_buffer_data, vertices_.data(), vertex_buffer_size());
    std::memcpy(element_array_buffer_data, elements_.data(),
        element_array_buffer_size());

    munmap(vertex_buffer_data, pool_size());
  }

  gl_vertex_buffer_.Data(GL_ARRAY_BUFFER, &vertex_buffer_, GL_STATIC_DRAW);
  gl_element_array_buffer_.Data(
      GL_ELEMENT_ARRAY_BUFFER, &element_array_buffer_, GL_STATIC_DRAW);

  program_.AttachShader(&vertex_shader_);
  program_.AttachShader(&fragment_shader_);
  program_.Link();

  vertex_array_.Enable(0);
  vertex_array_.VertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0, &gl_vertex_buffer_);
  vertex_array_.Enable(1);
  vertex_array_.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      offsetof(Vertex, u), &gl_vertex_buffer_);

  base_technique_.Bind(&vertex_array_);
  base_technique_.Bind(&program_);

  if (wire_) {
    base_technique_.DrawElements(GL_LINES, elements_.size(), GL_UNSIGNED_SHORT,
        0, &gl_element_array_buffer_);
  } else {
    if (texture_) {
      base_technique_.Bind(0, "", texture_.get(), GL_TEXTURE_2D, &sampler_);
      sampler_.Parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      sampler_.Parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    base_technique_.DrawElements(GL_TRIANGLES, elements_.size(),
        GL_UNSIGNED_SHORT, 0, &gl_element_array_buffer_);
  }

  initialized_ = true;

  return true;
}

void
Sphere::ConstructVertices()
{
  for (int i = +resolution_; i >= -resolution_; i--) {
    float y = sinf(M_PI_2 * (float)i / (float)resolution_);
    float v = (float)(resolution_ - i) / (2.f * (float)resolution_);
    float r = sqrtf(1 - y * y);
    for (int j = 0; j <= resolution_ * 4; j++) {
      float u = (float)j / (4.f * (float)resolution_);
      float theta = M_PI_2 * u * 4.f;
      float x = r * cosf(theta);
      float z = r * -sinf(theta);

      vertices_.emplace_back(x, y, z, u, v);
    }
  }
}

void
Sphere::ConstructElements()
{
  for (int i = +resolution_ - 1; i >= -resolution_ + 1; i--) {
    int n = resolution_ - i;
    for (int j = 0; j < resolution_ * 4; j++) {
      ushort num_vertices_in_a_latitudinal_line = (4 * resolution_ + 1);
      ushort A = n * num_vertices_in_a_latitudinal_line + j;
      ushort B = A + 1;
      ushort C = A - num_vertices_in_a_latitudinal_line;
      ushort D = B - num_vertices_in_a_latitudinal_line;
      ushort E = A + num_vertices_in_a_latitudinal_line;
      ushort F = B + num_vertices_in_a_latitudinal_line;

      if (wire_) {
        elements_.push_back(A);
        elements_.push_back(B);

        elements_.push_back(C);
        elements_.push_back(D);

        elements_.push_back(E);
        elements_.push_back(F);

        elements_.push_back(A);
        elements_.push_back(C);

        elements_.push_back(A);
        elements_.push_back(E);

        elements_.push_back(B);
        elements_.push_back(D);

        elements_.push_back(B);
        elements_.push_back(F);
      } else {
        elements_.push_back(A);
        elements_.push_back(E);
        elements_.push_back(B);

        elements_.push_back(B);
        elements_.push_back(D);
        elements_.push_back(A);
      }
    }
  }
}

//           j       j+1
//           .       .
// n-1 . . . C . . . D . .
//           .       .
//           .       .
//           .       .
//           .       .
// n . . . . A . . . B . . .
//           .       .
//           .       .
//           .       .
//           .       .
// n+1 . . . E . . . F . .
//           .       .
//           .       .
