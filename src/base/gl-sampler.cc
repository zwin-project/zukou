#include "gl-sampler.h"

#include "common.h"
#include "convert.h"
#include "logger.h"
#include "system.h"

namespace zukou {

ZUKOU_EXPORT bool
GlSampler::Init()
{
  return pimpl->Init();
}

ZUKOU_EXPORT void
GlSampler::Parameteri(GLenum pname, GLint param)
{
  pimpl->Parameteri(pname, param);
}

ZUKOU_EXPORT void
GlSampler::Parameterf(GLenum pname, GLfloat param)
{
  pimpl->Parameterf(pname, param);
}

ZUKOU_EXPORT
GlSampler::GlSampler(System *system) : pimpl(new Impl(system)) {}

ZUKOU_EXPORT
GlSampler::~GlSampler() = default;

bool
GlSampler::Impl::Init()
{
  proxy_ = zwn_gles_v32_create_gl_sampler(system_->pimpl->gles_v32());
  if (proxy_ == nullptr) {
    LOG_ERROR("Failed to create zwn_gl_shader proxy");
    return false;
  }

  return true;
}

void
GlSampler::Impl::Parameteri(GLenum pname, GLint param)
{
  zwn_gl_sampler_parameteri(proxy_, pname, param);
}

void
GlSampler::Impl::Parameterf(GLenum pname, GLfloat param)
{
  wl_array param_wl_array;
  to_array(param, &param_wl_array);

  zwn_gl_sampler_parameterf(proxy_, pname, &param_wl_array);

  wl_array_release(&param_wl_array);
}

GlSampler::Impl::Impl(System *system) : system_(system) {}

GlSampler::Impl::~Impl()
{
  if (proxy_) {
    zwn_gl_sampler_destroy(proxy_);
  }
}

}  // namespace zukou
