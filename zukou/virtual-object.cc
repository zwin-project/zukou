#include <zigen-client-protocol.h>
#include <zukou/zukou.h>

#include <memory>

namespace zukou {

VirtualObject::VirtualObject(std::shared_ptr<Application> app)
    : app_(app), frame_state_(VirtualObjectFrameState::kWaitingContentUpdate) {
  virtual_object_proxy_ =
      zgn_compositor_create_virtual_object(app->compositor_);
  if (virtual_object_proxy_ == nullptr)
    throw ZukouException("failed to create a virtual object");

  wl_proxy_set_user_data((wl_proxy *)virtual_object_proxy_, this);
}

VirtualObject::~VirtualObject() {
  zgn_virtual_object_destroy(virtual_object_proxy_);
}

void VirtualObject::FrameCallbackDone(
    void *data, struct wl_callback *callback, [[maybe_unused]] uint32_t time) {
  VirtualObject *virtual_object = reinterpret_cast<VirtualObject *>(data);
  wl_callback_destroy(callback);

  bool should_commit;
  switch (virtual_object->frame_state_) {
    case VirtualObjectFrameState::kRepaintScheduled:
      should_commit = virtual_object->Frame();
      virtual_object->frame_state_ =
          VirtualObjectFrameState::kWaitingContentUpdate;
      if (should_commit) virtual_object->Commit();
      break;

    case VirtualObjectFrameState::kWaitingNextFrame:
      virtual_object->frame_state_ =
          VirtualObjectFrameState::kWaitingContentUpdate;
      break;

    case VirtualObjectFrameState::kWaitingContentUpdate:
      assert(false && "not reached");
  }
}

void VirtualObject::Commit() {
  if (frame_state_ == VirtualObjectFrameState::kWaitingContentUpdate) {
    struct wl_callback *callback =
        zgn_virtual_object_frame(virtual_object_proxy_);

    wl_callback_add_listener(
        callback, &VirtualObject::frame_callback_listener_, this);

    frame_state_ = VirtualObjectFrameState::kWaitingNextFrame;
  }

  zgn_virtual_object_commit(virtual_object_proxy_);
  app_->Flush();
}

void VirtualObject::ScheduleNextFrame() {
  switch (frame_state_) {
    case VirtualObjectFrameState::kRepaintScheduled:
      // fall through
    case VirtualObjectFrameState::kWaitingNextFrame:
      frame_state_ = VirtualObjectFrameState::kRepaintScheduled;
      break;

    case VirtualObjectFrameState::kWaitingContentUpdate:
      if (this->Frame()) this->Commit();
      break;
  }
}

void VirtualObject::RayEnter([[maybe_unused]] uint32_t serial,
    [[maybe_unused]] glm::vec3 origin, [[maybe_unused]] glm::vec3 direction) {}

void VirtualObject::RayLeave([[maybe_unused]] uint32_t serial) {}

void VirtualObject::RayMotion([[maybe_unused]] uint32_t time,
    [[maybe_unused]] glm::vec3 origin, [[maybe_unused]] glm::vec3 direction) {}

void VirtualObject::RayButton([[maybe_unused]] uint32_t serial,
    [[maybe_unused]] uint32_t time, [[maybe_unused]] uint32_t button,
    [[maybe_unused]] bool pressed) {}

void VirtualObject::DataDeviceEnter([[maybe_unused]] uint32_t serial,
    [[maybe_unused]] glm::vec3 origin, [[maybe_unused]] glm::vec3 direction,
    [[maybe_unused]] std::weak_ptr<DataOffer> data_offer) {}

void VirtualObject::DataDeviceLeave() {}

void VirtualObject::DataDeviceMotion([[maybe_unused]] uint32_t time,
    [[maybe_unused]] glm::vec3 origin, [[maybe_unused]] glm::vec3 direction) {}

void VirtualObject::DataDeviceDrop() {}

bool VirtualObject::Frame() { return true; }

const struct wl_callback_listener VirtualObject::frame_callback_listener_ = {
    VirtualObject::FrameCallbackDone,
};

}  // namespace zukou
