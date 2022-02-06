#ifndef ZUKOU_FILE_HELPER_H
#define ZUKOU_FILE_HELPER_H

#include <sys/mman.h>
#include <unistd.h>

namespace zukou {
namespace os_helper {

static int create_shared_fd(off_t size) {
  const char *name = "zukou-shm";
  int fd = memfd_create(name, MFD_CLOEXEC | MFD_ALLOW_SEALING);
  if (fd < 0) return fd;
  unlink(name);

  if (ftruncate(fd, size) < 0) {
    close(fd);
    return -1;
  }

  return fd;
}

}  // namespace os_helper
}  // namespace zukou

#endif  //  ZUKOU_FILE_HELPER_H
