#include <zigen-client-protocol.h>
#include <zukou.h>

#include <memory>

namespace zukou {

VirtualObject::VirtualObject(std::shared_ptr<Application> app) : app_(app) {
  virtual_object_proxy_ =
      zgn_compositor_create_virtual_object(app->compositor_);
  if (virtual_object_proxy_ == nullptr)
    throw ZukouException("failed to create a virtual object");

  wl_proxy_set_user_data((wl_proxy *)virtual_object_proxy_, this);
}

VirtualObject::~VirtualObject() {
  zgn_virtual_object_destroy(virtual_object_proxy_);
}

void VirtualObject::Commit() {
  zgn_virtual_object_commit(virtual_object_proxy_);
}

void VirtualObject::NextFrame() {
  struct wl_callback *callback =
      zgn_virtual_object_frame(virtual_object_proxy_);
  wl_callback_add_listener(
      callback, &VirtualObject::frame_callback_listener_, this);
  this->Commit();
}

void VirtualObject::RayEnter([[maybe_unused]] uint32_t serial,
    [[maybe_unused]] glm::vec3 origin, [[maybe_unused]] glm::vec3 direction) {}

void VirtualObject::RayLeave([[maybe_unused]] uint32_t serial) {}

void VirtualObject::RayMotion([[maybe_unused]] uint32_t time,
    [[maybe_unused]] glm::vec3 origin, [[maybe_unused]] glm::vec3 direction) {}

void VirtualObject::RayButton([[maybe_unused]] uint32_t serial,
    [[maybe_unused]] uint32_t time, [[maybe_unused]] uint32_t button,
    [[maybe_unused]] bool pressed) {}

void VirtualObject::Frame([[maybe_unused]] uint32_t time) {}

const struct wl_callback_listener VirtualObject::frame_callback_listener_ = {
    VirtualObject::FrameCallbackDone,
};

void VirtualObject::FrameCallbackDone(
    void *data, struct wl_callback *callback, uint32_t time) {
  VirtualObject *virtual_object = reinterpret_cast<VirtualObject *>(data);
  wl_callback_destroy(callback);
  virtual_object->Frame(time);
}

}  // namespace zukou
