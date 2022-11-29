#include "loop.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "logger.h"

namespace zukou {

bool
Loop::Init()
{
  epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
  if (epoll_fd_ == -1) {
    LOG_ERROR("Failed to create epoll instance");
    return false;
  }
  return true;
}

int
Loop::Run()
{
  int epoll_count;
  struct epoll_event events[16];
  EventSource* source;

  running_ = true;
  while (running_) {
    epoll_count = epoll_wait(epoll_fd_, events, 16, -1);
    for (int i = 0; i < epoll_count; i++) {
      source = static_cast<EventSource*>(events[i].data.ptr);
      uint32_t mask = 0;
      if (events->events & EPOLLIN) mask |= kEventReadable;
      if (events->events & EPOLLOUT) mask |= kEventWritable;
      if (events->events & EPOLLHUP) mask |= kEventHangUp;
      if (events->events & EPOLLERR) mask |= kEventError;
      source->callback(source->fd, mask, source->data);
    }
  }

  return exit_status_;
}

void
Loop::Terminate(int exit_status)
{
  exit_status_ = exit_status;
  running_ = false;  // TODO: wakeup epoll instance
}

Loop::~Loop()
{
  if (epoll_fd_ > 0) close(epoll_fd_);
}

std::unique_ptr<EventSource>
Loop::AddFd(int fd, uint32_t mask, EventSource::Callback callback, void* data)
{
  struct epoll_event ep;
  auto source = new EventSource();

  source->dupfd = fcntl(fd, F_DUPFD_CLOEXEC, 0);
  if (errno == EINVAL) {
    // TODO: fallback for older linux
    LOG_ERROR("fcntl with FD_DUPFD_CLOEXEC is not supported");
    return std::unique_ptr<EventSource>();
  }

  source->fd = fd;
  source->data = data;
  source->callback = callback;
  source->loop = this;

  ep.events = 0;
  if (mask & kEventReadable) ep.events |= EPOLLIN;
  if (mask & kEventWritable) ep.events |= EPOLLOUT;
  ep.data.ptr = source;

  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, source->dupfd, &ep) < 0) {
    close(source->dupfd);
    delete source;
    return std::unique_ptr<EventSource>();
  }

  return std::unique_ptr<EventSource>(source);
}

EventSource::~EventSource()
{
  epoll_ctl(loop->epoll_fd_, EPOLL_CTL_DEL, dupfd, NULL);
  close(dupfd);
}

}  // namespace zukou
