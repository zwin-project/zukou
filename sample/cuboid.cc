#include "cuboid.h"

#include <sys/mman.h>
#include <unistd.h>

#include <cstring>

#include "color.fragment.h"
#include "default.vert.h"

Cuboid::Cuboid(zukou::System* system, zukou::VirtualObject* virtual_object)
    : virtual_object_(virtual_object),
      pool_(system),
      gl_vertex_buffer_(system),
      gl_element_array_buffer_(system),
      vertex_array_(system),
      vertex_shader_(system),
      fragment_shader_(system),
      program_(system),
      rendering_unit_(system),
      base_technique_(system)
{}

Cuboid::~Cuboid()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

bool
Cuboid::Render(glm::vec3 half_size, glm::mat4 transform, glm::vec4 color)
{
  if (!initialized_ && Init() == false) {
    return false;
  }

  auto local_model = glm::scale(transform, half_size);
  base_technique_.Uniform(0, "local_model", local_model);
  base_technique_.Uniform(0, "color", color);

  return true;
}

bool
Cuboid::Init()
{
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
  if (!fragment_shader_.Init(GL_FRAGMENT_SHADER, color_fragment_shader_source))
    return false;
  if (!program_.Init()) return false;

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

  base_technique_.DrawElements(GL_LINES, elements_.size(), GL_UNSIGNED_SHORT, 0,
      &gl_element_array_buffer_);

  initialized_ = true;

  return true;
}
