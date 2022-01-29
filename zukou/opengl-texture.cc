#include <zigen-opengl-client-protocol.h>
#include <zukou.h>

namespace zukou {

std::shared_ptr<OpenGLTexture> OpenGLTexture::Create(
    std::shared_ptr<Application> app, uint32_t width, uint32_t height) {
  return std::shared_ptr<OpenGLTexture>(new OpenGLTexture(app, width, height));
}

OpenGLTexture::OpenGLTexture(
    std::shared_ptr<Application> app, uint32_t width, uint32_t height)
    : Buffer(app, width, height) {
  proxy_ = zgn_opengl_create_texture(app->opengl_);
  if (proxy_ == nullptr)
    throw ZukouException("failed to create a opengl texture");

  zgn_opengl_texture_attach_2d(proxy_, buffer_proxy_);
}

OpenGLTexture::~OpenGLTexture() { zgn_opengl_texture_destroy(proxy_); }

void OpenGLTexture::ReattachBuffer() {
  zgn_opengl_texture_attach_2d(proxy_, buffer_proxy_);
}

ColorBGRA *OpenGLTexture::GetData() {
  return reinterpret_cast<ColorBGRA *>(data_);
}

}  // namespace zukou
