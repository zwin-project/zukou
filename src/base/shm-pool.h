#pragma once

#include <zukou.h>

#include <zwin-client-protocol.h>

namespace zukou {

class ShmPool::Impl
{
 public:
  DISABLE_MOVE_AND_COPY(Impl);
  Impl(System* system);
  ~Impl();

  bool Init(int fd, off_t size);

  inline zwn_shm_pool* proxy();

 private:
  System* system_;
  zwn_shm_pool* proxy_ = nullptr;
};

zwn_shm_pool*
ShmPool::Impl::proxy()
{
  return proxy_;
}

}  // namespace zukou
