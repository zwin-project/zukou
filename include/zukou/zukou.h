#ifndef ZUKOU_H
#define ZUKOU_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>

#define DISABLE_MOVE_AND_COPY(Class)        \
  Class(const Class &) = delete;            \
  Class(Class &&) = delete;                 \
  Class &operator=(const Class &) = delete; \
  Class &operator=(Class &&) = delete;

namespace zukou {

class Application;
class Buffer;
class CuboidWindow;
class OpenGLComponent;
class OpenGLElementArrayBuffer;
class OpenGLShaderProgram;
class OpenGLTexture;
class OpenGLVertexBuffer;
class Ray;
class VirtualObject;

class VirtualObject : public std::enable_shared_from_this<VirtualObject> {
  friend Ray;
  friend OpenGLComponent;

 public:
  DISABLE_MOVE_AND_COPY(VirtualObject)

  virtual ~VirtualObject();

  void Commit();
  void NextFrame();

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
  DISABLE_MOVE_AND_COPY(CuboidWindow)

  explicit CuboidWindow(std::shared_ptr<Application> app, glm::vec3 half_size);
  virtual ~CuboidWindow();

  void Move(uint32_t serial);
  void Rotate(glm::quat quaternion);

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
  friend Buffer;
  friend CuboidWindow;
  friend OpenGLComponent;
  friend OpenGLElementArrayBuffer;
  friend OpenGLShaderProgram;
  friend OpenGLTexture;
  friend OpenGLVertexBuffer;
  friend VirtualObject;

 public:
  static std::shared_ptr<Application> Create();

 public:
  DISABLE_MOVE_AND_COPY(Application)

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

class Buffer {
 public:
  DISABLE_MOVE_AND_COPY(Buffer)

  virtual ~Buffer();

 protected:
  Buffer(std::shared_ptr<Application> app, int32_t width, int32_t height);
  Buffer(std::shared_ptr<Application> app, size_t size);

  struct wl_shm_pool *pool_proxy_;
  struct wl_buffer *buffer_proxy_;
  int fd_;
  size_t size_;
  void *data_;
};

struct ColorBGRA {
  uint8_t b, g, r, a;
};

class OpenGLTexture : public Buffer {
  friend OpenGLComponent;

 public:
  static std::shared_ptr<OpenGLTexture> Create(
      std::shared_ptr<Application> app, uint32_t width, uint32_t height);

 public:
  DISABLE_MOVE_AND_COPY(OpenGLTexture)

  ~OpenGLTexture();

  void ReattachBuffer();
  ColorBGRA *GetData();

 private:
  OpenGLTexture(
      std::shared_ptr<Application> app, uint32_t width, uint32_t height);

 private:
  struct zgn_opengl_texture *proxy_;
};

class OpenGLVertexBuffer : public Buffer {
  friend OpenGLComponent;

 public:
  static std::shared_ptr<OpenGLVertexBuffer> Create(
      std::shared_ptr<Application> app, size_t size);

 public:
  DISABLE_MOVE_AND_COPY(OpenGLVertexBuffer)

  ~OpenGLVertexBuffer();

  void ReattachBuffer();

  template <typename T>
  T *GetData() {
    return reinterpret_cast<T *>(data_);
  }

 private:
  OpenGLVertexBuffer(std::shared_ptr<Application> app, size_t size);

 private:
  struct zgn_opengl_vertex_buffer *proxy_;
};

class OpenGLElementArrayBuffer : public Buffer {
  friend OpenGLComponent;

 public:
  static std::shared_ptr<OpenGLElementArrayBuffer> Create(
      std::shared_ptr<Application> app, size_t size, uint32_t type);

 public:
  DISABLE_MOVE_AND_COPY(OpenGLElementArrayBuffer)

  ~OpenGLElementArrayBuffer();

  void ReattachBuffer();

  template <typename T>
  T *GetData() {
    return reinterpret_cast<T *>(data_);
  }

 private:
  OpenGLElementArrayBuffer(
      std::shared_ptr<Application> app, size_t size, uint32_t type);
  uint32_t type_;

 private:
  struct zgn_opengl_element_array_buffer *proxy_;
};

class OpenGLShaderProgram {
  friend OpenGLComponent;

 public:
  static std::shared_ptr<OpenGLShaderProgram> Create(
      std::shared_ptr<Application> app);

 public:
  DISABLE_MOVE_AND_COPY(OpenGLShaderProgram)

  ~OpenGLShaderProgram();

  void SetUniformVariable(const char *location, glm::mat4 mat);
  void SetUniformVariable(const char *location, glm::vec4 vec);
  void SetUniformVariable(const char *location, glm::vec3 vec);
  bool SetVertexShader(std::string source);
  bool SetFragmentShader(std::string source);
  void Link();

 private:
  OpenGLShaderProgram(std::shared_ptr<Application> app);

 private:
  struct zgn_opengl_shader_program *proxy_;
};

class OpenGLComponent {
 public:
  DISABLE_MOVE_AND_COPY(OpenGLComponent)

  OpenGLComponent(std::shared_ptr<Application> app,
      std::shared_ptr<VirtualObject> virtual_object);
  ~OpenGLComponent();

  void Attach(std::shared_ptr<OpenGLVertexBuffer> vertex_buffer);
  void Attach(std::shared_ptr<OpenGLElementArrayBuffer> element_array_buffer);
  void Attach(std::shared_ptr<OpenGLShaderProgram> shader_program);
  void Attach(std::shared_ptr<OpenGLTexture> texture);
  void SetMin(uint32_t min);
  void SetCount(uint32_t count);
  void SetTopology(uint32_t topology);
  void AddVertexAttribute(uint32_t index, uint32_t size, uint16_t type,
      uint32_t normalized, uint32_t stride, uint32_t pointer);

 private:
  struct zgn_opengl_component *proxy_;
};

class ZukouException : public std::exception {
 public:
  explicit ZukouException(const char *message) : message_(message){};
  const char *what() const noexcept override { return message_; };

 private:
  const char *message_;
};

}  // namespace zukou

#include "primitives.h"

#endif  //  ZUKOU_H
