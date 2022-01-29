#include <glm-helper.h>
#include <zigen-opengl-client-protocol.h>
#include <zukou.h>

#include "os-helper.h"

namespace zukou {

std::shared_ptr<OpenGLShaderProgram> OpenGLShaderProgram::Create(
    std::shared_ptr<Application> app) {
  return std::shared_ptr<OpenGLShaderProgram>(new OpenGLShaderProgram(app));
}

OpenGLShaderProgram::OpenGLShaderProgram(std::shared_ptr<Application> app) {
  proxy_ = zgn_opengl_create_shader_program(app->opengl_);
  if (proxy_ == nullptr)
    throw ZukouException("failed to create opengl shader program");
  zgn_opengl_shader_program_set_user_data(proxy_, this);
}

OpenGLShaderProgram::~OpenGLShaderProgram() {
  zgn_opengl_shader_program_destroy(proxy_);
}

void OpenGLShaderProgram::SetUniformVariable(
    const char *location, glm::mat4 mat) {
  struct wl_array array;
  wl_array_init(&array);
  glm_helper::to_wl_array(mat, &array);
  zgn_opengl_shader_program_set_uniform_float_matrix(
      proxy_, location, 4, 4, false, 1, &array);
  wl_array_release(&array);
}

void OpenGLShaderProgram::SetUniformVariable(
    const char *location, glm::vec4 vec) {
  struct wl_array array;
  wl_array_init(&array);
  glm_helper::to_wl_array(vec, &array);
  zgn_opengl_shader_program_set_uniform_float_vector(
      proxy_, location, 4, 1, &array);
  wl_array_release(&array);
}

void OpenGLShaderProgram::SetUniformVariable(
    const char *location, glm::vec3 vec) {
  struct wl_array array;
  wl_array_init(&array);
  glm_helper::to_wl_array(vec, &array);
  zgn_opengl_shader_program_set_uniform_float_vector(
      proxy_, location, 3, 1, &array);
  wl_array_release(&array);
}

bool OpenGLShaderProgram::SetVertexShader(std::string source) {
  int fd = os_helper::create_shared_fd(source.size());
  char *data = reinterpret_cast<char *>(
      mmap(NULL, source.size(), PROT_WRITE, MAP_SHARED, fd, 0));

  if (data == MAP_FAILED) {
    close(fd);
    return false;
  }

  source.copy(data, source.size(), 0);
  munmap(data, source.size());

  zgn_opengl_shader_program_set_vertex_shader(proxy_, fd, source.size());
  close(fd);

  return true;
}

bool OpenGLShaderProgram::SetFragmentShader(std::string source) {
  int fd = os_helper::create_shared_fd(source.size());
  char *data = reinterpret_cast<char *>(
      mmap(NULL, source.size(), PROT_WRITE, MAP_SHARED, fd, 0));

  if (data == MAP_FAILED) {
    close(fd);
    return false;
  }

  source.copy(data, source.size(), 0);
  munmap(data, source.size());

  zgn_opengl_shader_program_set_fragment_shader(proxy_, fd, source.size());
  close(fd);

  return true;
}

void OpenGLShaderProgram::Link() { zgn_opengl_shader_program_link(proxy_); }

}  // namespace zukou
