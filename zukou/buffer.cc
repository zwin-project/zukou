#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>
#include <zukou.h>

#include "os-helper.h"

namespace zukou {

Buffer::Buffer(
    std::shared_ptr<Application> app, int32_t width, int32_t height) {
  const char *err_message;
  size_t stride = width * sizeof(ColorBGRA);
  size_ = stride * height;

  fd_ = os_helper::create_shared_fd(size_);
  if (fd_ < 0) {
    err_message = "failed to open shm file";
    goto err;
  }

  data_ = mmap(NULL, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (data_ == MAP_FAILED) {
    err_message = "failed to mmap for Buffer";
    goto err_mmap;
  }

  pool_proxy_ = wl_shm_create_pool(app->shm_, fd_, size_);
  if (pool_proxy_ == nullptr) {
    err_message = "failed to create a wl_shm_pool";
    goto err_pool;
  }

  buffer_proxy_ = wl_shm_pool_create_buffer(
      pool_proxy_, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
  if (buffer_proxy_ == nullptr) {
    err_message = "failed to create a wl_buffer";
    goto err_buffer;
  }
  // TODO: listen events

  return;

err_buffer:
  wl_shm_pool_destroy(pool_proxy_);

err_pool:
  munmap(data_, size_);

err_mmap:
  close(fd_);

err:
  throw ZukouException(err_message);
}

Buffer::Buffer(std::shared_ptr<Application> app, size_t size)
    : Buffer(app, size / sizeof(ColorBGRA), 1) {}

Buffer::~Buffer() {
  wl_buffer_destroy(buffer_proxy_);
  wl_shm_pool_destroy(pool_proxy_);
  munmap(data_, size_);
  close(fd_);
}

}  // namespace zukou
