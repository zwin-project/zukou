#ifndef ZUKOU_H
#define ZUKOU_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>

namespace zukou {

class Application;
class CuboidWindow;
class Ray;
class VirtualObject;

class VirtualObject : public std::enable_shared_from_this<VirtualObject> {
  friend Ray;

 public:
  VirtualObject(const VirtualObject &) = delete;
  VirtualObject(VirtualObject &&) = delete;
  VirtualObject &operator=(const VirtualObject &) = delete;
  VirtualObject &operator=(VirtualObject &&) = delete;

  virtual ~VirtualObject();

  virtual void Commit() final;
  virtual void NextFrame() final;

 protected:
  explicit VirtualObject(std::shared_ptr<Application> app);
  virtual void Frame(uint32_t time);
  virtual void RayEnter(uint32_t serial, glm::vec3 origin, glm::vec3 direction);
  virtual void RayLeave(uint32_t serial);
  virtual void RayMotion(uint32_t time, glm::vec3 origin, glm::vec3 direction);
  virtual void RayButton(
      uint32_t serial, uint32_t time, uint32_t button, bool pressed);

 protected:
  struct zgn_virtual_object *virtual_object_proxy_;
  std::shared_ptr<Application> app_;

 private:
  static const struct wl_callback_listener frame_callback_listener_;
  static void FrameCallbackDone(
      void *data, struct wl_callback *callback, uint32_t time);
};

class CuboidWindow : public VirtualObject {
 public:
  CuboidWindow(const CuboidWindow &) = delete;
  CuboidWindow(CuboidWindow &&) = delete;
  CuboidWindow &operator=(const CuboidWindow &) = delete;
  CuboidWindow &operator=(CuboidWindow &&) = delete;

  explicit CuboidWindow(std::shared_ptr<Application> app, glm::vec3 half_size);
  virtual ~CuboidWindow();

  virtual void Move(uint32_t serial) final;
  virtual void Rotate(glm::quat quaternion) final;

  inline glm::vec3 half_size() { return half_size_; }
  inline glm::quat quaternion() { return quaternion_; }

 protected:
  explicit CuboidWindow(std::shared_ptr<Application> app, glm::vec3 half_size,
      glm::quat quaternion);
  virtual void Configured(uint32_t serial);
  virtual void Moved(glm::vec3 face_direction);

 private:
  static const struct zgn_cuboid_window_listener cuboid_window_listener_;
  static void Configure(void *data, struct zgn_cuboid_window *zgn_cuboid_window,
      uint32_t serial, struct wl_array *wl_half_size,
      struct wl_array *wl_quaternion);
  static void Moved(void *data, struct zgn_cuboid_window *zgn_cuboid_window,
      struct wl_array *wl_face_direction);

 private:
  struct zgn_cuboid_window *proxy_;
  glm::vec3 half_size_;
  glm::quat quaternion_;
};

class Application final : public std::enable_shared_from_this<Application> {
  friend CuboidWindow;
  friend VirtualObject;

 public:
  static std::shared_ptr<Application> Create();

 public:
  Application(const Application &) = delete;
  Application(Application &&) = delete;
  Application &operator=(const Application &) = delete;
  Application &operator=(Application &&) = delete;

  ~Application();

  void Connect(std::string socket);
  bool Run();
  void Terminate();

 private:
  static const struct wl_registry_listener registry_listener_;
  static void GlobalRegistry(void *data, struct wl_registry *registry,
      uint32_t id, const char *interface, uint32_t version);
  static void GlobalRegistryRemove(
      void *data, struct wl_registry *registry, uint32_t id);

  static const struct wl_shm_listener shm_listener_;
  static void ShmFormat(void *data, struct wl_shm *shm, uint32_t format);

  static const struct zgn_seat_listener seat_listener_;
  static void SeatCapabilities(
      void *data, struct zgn_seat *seat, uint32_t capability);

 private:
  explicit Application();

 private:
  bool running_;

  // global objects
  struct wl_display *display_;
  struct wl_registry *registry_;
  struct zgn_compositor *compositor_;
  struct zgn_seat *seat_;
  struct wl_shm *shm_;
  struct zgn_opengl *opengl_;
  struct zgn_shell *shell_;

  // other wayland objects
  std::unique_ptr<Ray> ray_;
};

class ZukouException : public std::exception {
 public:
  explicit ZukouException(const char *message) : message_(message){};
  const char *what() const noexcept override { return message_; };

 private:
  const char *message_;
};

}  // namespace zukou

#endif  //  ZUKOU_H
