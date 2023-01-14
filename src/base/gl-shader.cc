#include "gl-shader.h"

#include <sys/mman.h>
#include <unistd.h>

#include <cstring>

#include "buffer.h"
#include "common.h"
#include "logger.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT bool
GlShader::Init(GLenum type, const std::string &source)
{
  return pimpl->Init(type, source);
}

ZUKOU_EXPORT
GlShader::GlShader(System *system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
GlShader::~GlShader() = default;

bool
GlShader::Impl::Init(GLenum type, const std::string &source)
{
  fd_ = Util::CreateAnonymousFile(source.size());
  if (fd_ < 0) {
    LOG_ERROR("Failed to create an anonymous file");
    return false;
  }

  {
    auto buffer = mmap(nullptr, source.size(), PROT_WRITE, MAP_SHARED, fd_, 0);
    std::memcpy(buffer, source.data(), source.size());
    munmap(buffer, source.size());
  }

  pool_.Init(fd_, source.size());
  buffer_.Init(&pool_, 0, source.size());

  proxy_ = zwn_gles_v32_create_gl_shader(
      system_->pimpl->gles_v32(), buffer_.pimpl->proxy(), type);

  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to creat zwn_gl_shader proxy");
    close(fd_);
    fd_ = 0;
    return false;
  }

  return true;
}

GlShader::Impl::Impl(System *system) : system_(system), pool_(system), buffer_()
{}

GlShader::Impl::~Impl()
{
  if (proxy_) {
    zwn_gl_shader_destroy(proxy_);
  }

  if (fd_ != 0) close(fd_);
}

}  // namespace zukou
