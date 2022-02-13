#ifndef ZUKOU_DATA_DEVICE_H
#define ZUKOU_DATA_DEVICE_H

#include <zigen-client-protocol.h>
#include <zukou/zukou.h>

namespace zukou {

class DataDevice {
 public:
  DISABLE_MOVE_AND_COPY(DataDevice)

  DataDevice(struct zgn_data_device_manager *data_device_manager,
      struct zgn_seat *seat, std::shared_ptr<Application> app);
  ~DataDevice();

  void SetLength(float length);

 private:
  static const struct zgn_data_device_listener data_device_listener_;
  static void DataOffer(void *data, struct zgn_data_device *zgn_data_device,
      struct zgn_data_offer *id);
  static void Enter(void *data, struct zgn_data_device *zgn_data_device,
      uint32_t serial, struct zgn_virtual_object *virtual_object,
      struct wl_array *origin, struct wl_array *direction,
      struct zgn_data_offer *id);
  static void Leave(void *data, struct zgn_data_device *zgn_data_device);
  static void Motion(void *data, struct zgn_data_device *zgn_data_device,
      uint32_t time, struct wl_array *origin, struct wl_array *direction);
  static void Drop(void *data, struct zgn_data_device *zgn_data_device);

 private:
  struct zgn_data_device *proxy_;
  std::shared_ptr<Application> app_;
  std::shared_ptr<zukou::DataOffer> data_offer_; /* nullable */
  std::weak_ptr<VirtualObject> focus_;
  uint32_t enter_serial_;
};

}  // namespace zukou

#endif  //  ZUKOU_DATA_DEVICE_H
