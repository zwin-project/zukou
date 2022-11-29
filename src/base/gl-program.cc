#include "gl-program.h"

#include "common.h"
#include "gl-shader.h"
#include "logger.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT bool
GlProgram::Init()
{
  return pimpl->Init();
}

ZUKOU_EXPORT void
GlProgram::AttachShader(GlShader *shader)
{
  pimpl->AttachShader(shader);
}

ZUKOU_EXPORT void
GlProgram::Link()
{
  pimpl->Link();
}

ZUKOU_EXPORT
GlProgram::GlProgram(System *system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
GlProgram::~GlProgram() = default;

bool
GlProgram::Impl::Init()
{
  proxy_ = zgn_gles_v32_create_gl_program(system_->pimpl->gles_v32());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create zgn_gl_program proxy");
    return false;
  }

  return true;
}

void
GlProgram::Impl::AttachShader(GlShader *shader)
{
  zgn_gl_program_attach_shader(proxy_, shader->pimpl->proxy());
}

void
GlProgram::Impl::Link()
{
  zgn_gl_program_link(proxy_);
}

GlProgram::Impl::Impl(System *system) : system_(system) {}

GlProgram::Impl::~Impl()
{
  if (proxy_) {
    zgn_gl_program_destroy(proxy_);
  }
}

}  // namespace zukou
