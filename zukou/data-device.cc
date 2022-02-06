#include "data-device.h"

#include "glm-helper.h"
#include "wayland-helper.h"

namespace zukou {

DataDevice::DataDevice(struct zgn_data_device_manager *data_device_manager,
    struct zgn_seat *seat, std::shared_ptr<Application> app)
    : app_(app), focus_() {
  proxy_ = zgn_data_device_manager_get_data_device(data_device_manager, seat);
  if (proxy_ == nullptr) throw ZukouException("failed to get data device");

  zgn_data_device_add_listener(
      proxy_, &DataDevice::data_device_listener_, this);
}

DataDevice::~DataDevice() { zgn_data_device_destroy(proxy_); }

const struct zgn_data_device_listener DataDevice::data_device_listener_ = {
    DataDevice::DataOffer,
    DataDevice::Enter,
    DataDevice::Leave,
    DataDevice::Motion,
    DataDevice::Drop,
};

void DataDevice::DataOffer(void *data, struct zgn_data_device *zgn_data_device,
    struct zgn_data_offer *id) {
  (void)zgn_data_device;
  DataDevice *data_device = reinterpret_cast<DataDevice *>(data);
  data_device->data_offer_ = DataOffer::Create(id, data_device->app_);
}

void DataDevice::Enter(void *data,
    [[maybe_unused]] struct zgn_data_device *zgn_data_device, uint32_t serial,
    struct zgn_virtual_object *zgn_virtual_object, struct wl_array *origin,
    struct wl_array *direction, struct zgn_data_offer *id) {
  DataDevice *data_device = reinterpret_cast<DataDevice *>(data);
  auto data_offer = wayland_helper::proxy_get_user_data<zukou::DataOffer>(id)
                        ->shared_from_this();

  if (data_device->data_offer_ != data_offer) return;

  auto virtual_object =
      wayland_helper::proxy_get_user_data<VirtualObject>(zgn_virtual_object);

  data_device->focus_ = virtual_object->shared_from_this();

  virtual_object->DataDeviceEnter(serial,
      glm_helper::vec3_from_wl_array(origin),
      glm_helper::vec3_from_wl_array(direction), data_offer);
}

void DataDevice::Leave(
    void *data, [[maybe_unused]] struct zgn_data_device *zgn_data_device) {
  DataDevice *data_device = reinterpret_cast<DataDevice *>(data);

  if (auto virtual_object = data_device->focus_.lock()) {
    virtual_object->DataDeviceLeave();
  }

  data_device->data_offer_.reset();

  data_device->focus_.reset();
}

void DataDevice::Motion(void *data,
    [[maybe_unused]] struct zgn_data_device *zgn_data_device, uint32_t time,
    struct wl_array *origin, struct wl_array *direction) {
  DataDevice *data_device = reinterpret_cast<DataDevice *>(data);

  if (auto virtual_object = data_device->focus_.lock()) {
    virtual_object->DataDeviceMotion(time,
        glm_helper::vec3_from_wl_array(origin),
        glm_helper::vec3_from_wl_array(direction));
  }
}

void DataDevice::Drop(
    void *data, [[maybe_unused]] struct zgn_data_device *zgn_data_device) {
  DataDevice *data_device = reinterpret_cast<DataDevice *>(data);

  if (auto virtual_object = data_device->focus_.lock()) {
    virtual_object->DataDeviceDrop();
  }
}

}  // namespace zukou
