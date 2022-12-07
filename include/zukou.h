#pragma once

#include <GLES3/gl32.h>

#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <string>

#define DISABLE_MOVE_AND_COPY(Class)        \
  Class(const Class &) = delete;            \
  Class(Class &&) = delete;                 \
  Class &operator=(const Class &) = delete; \
  Class &operator=(Class &&) = delete

namespace zukou {

class VirtualObject;

struct ISystemDelegate {
  virtual ~ISystemDelegate() = default;

  virtual void RayEnter(uint32_t /*serial*/, VirtualObject * /*virtual_object*/,
      glm::vec3 /*origin*/, glm::vec3 /*direction*/){};

  virtual void RayLeave(
      uint32_t /*serial*/, VirtualObject * /*virtual_object*/){};

  virtual void RayMotion(
      uint32_t /*time*/, glm::vec3 /*origin*/, glm::vec3 /*direction*/){};

  virtual void RayButton(uint32_t /*serial*/, uint32_t /*time*/,
      uint32_t /*button*/, bool /*pressed*/){};
};

class System final
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(System);
  System(ISystemDelegate *delegate = nullptr);
  ~System();

  bool Init(const char *socket = nullptr) const;

  bool Run() const;

  const std::unique_ptr<Impl> pimpl;
};

class Util
{
 public:
  static int CreateAnonymousFile(off_t size);
};

class Region
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(Region);
  Region() = delete;
  Region(System *system);
  ~Region();

  bool Init();

  void AddCuboid(const glm::vec3 &half_size, const glm::vec3 &center,
      const glm::quat &quaternion);

  const std::unique_ptr<Impl> pimpl;
};

class ShmPool
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(ShmPool);
  ShmPool() = delete;
  ShmPool(System *system);
  ~ShmPool();

  bool Init(int fd, off_t size);

  const std::unique_ptr<Impl> pimpl;
};

struct IBufferDelegate {
  virtual ~IBufferDelegate() = default;

  virtual void Release(){};
};

class Buffer
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(Buffer);
  Buffer(IBufferDelegate *delegate = nullptr);
  ~Buffer();

  bool Init(ShmPool *pool, off_t offset, off_t size);

  const std::unique_ptr<Impl> pimpl;
};

struct IVirtualObjectDelegate {
  virtual ~IVirtualObjectDelegate() = default;

  virtual void Frame(uint32_t /*time*/){};
};

class VirtualObject
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(VirtualObject);
  VirtualObject() = delete;
  VirtualObject(System *system, IVirtualObjectDelegate *delegate);
  virtual ~VirtualObject();

  bool Init();

  void Commit();

  void NextFrame();

  const std::unique_ptr<Impl> pimpl;
};

struct IBoundedDelegate : public IVirtualObjectDelegate {
  virtual ~IBoundedDelegate() = default;

  virtual void Configure(glm::vec3 /*half_size*/, uint32_t /*serial*/) {}
};

class Bounded final : public VirtualObject
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(Bounded);
  Bounded() = delete;
  Bounded(System *system, IBoundedDelegate *delegate);
  ~Bounded();

  bool Init(const glm::vec3 &half_size);

  void AckConfigure(uint32_t serial);

  void SetRegion(Region *region);

  void Move(uint32_t serial);

  const std::unique_ptr<Impl> pimpl;
};

class RenderingUnit final
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(System *system);
  ~RenderingUnit();

  bool Init(VirtualObject *virtual_object);

  const std::unique_ptr<Impl> pimpl;
};

class GlBuffer
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(GlBuffer);
  GlBuffer() = delete;
  GlBuffer(System *system);
  ~GlBuffer();

  bool Init();

  void Data(GLenum target, Buffer *buffer, GLenum usage);

  const std::unique_ptr<Impl> pimpl;
};

class GlShader
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(GlShader);
  GlShader() = delete;
  GlShader(System *system);
  ~GlShader();

  bool Init(GLenum type, const std::string &source);

  const std::unique_ptr<Impl> pimpl;
};

class GlProgram
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(GlProgram);
  GlProgram() = delete;
  GlProgram(System *system);
  ~GlProgram();

  bool Init();

  void AttachShader(GlShader *shader);

  void Link();

  const std::unique_ptr<Impl> pimpl;
};

class GlTexture
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(GlTexture);
  GlTexture() = delete;
  GlTexture(System *system);
  ~GlTexture();

  bool Init();

  void Image2D(GLenum target, GLint level, GLint internal_format, GLsizei width,
      GLsizei height, GLint border, GLenum format, GLenum type, Buffer *buffer);

  const std::unique_ptr<Impl> pimpl;
};

class GlSampler
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(GlSampler);
  GlSampler() = delete;
  GlSampler(System *system);
  ~GlSampler();

  bool Init();

  void Parameteri(GLenum pname, GLint param);

  void Parameterf(GLenum pname, GLfloat param);

  const std::unique_ptr<Impl> pimpl;
};

class GlVertexArray
{
  class Impl;

 public:
  DISABLE_MOVE_AND_COPY(GlVertexArray);
  GlVertexArray() = delete;
  GlVertexArray(System *system);
  ~GlVertexArray();

  bool Init();

  void Enable(GLuint index);

  void Disable(GLuint index);

  void VertexAttribPointer(GLuint index, GLint size, GLenum type,
      GLboolean normalized, GLsizei stride, uint64_t offset,
      GlBuffer *gl_buffer);

  const std::unique_ptr<Impl> pimpl;
};

class GlBaseTechnique
{
  class Impl;

  enum UniformVariableType {
    kFloat,
    kUint,
    kInt,
  };

 public:
  DISABLE_MOVE_AND_COPY(GlBaseTechnique);
  GlBaseTechnique() = delete;
  GlBaseTechnique(System *system);
  ~GlBaseTechnique();

  bool Init(RenderingUnit *unit);

  template <glm::length_t length, typename T>
  void Uniform(
      uint32_t location, const std::string &name, glm::vec<length, T> value)
  {
    static_assert(std::is_same<T, float>::value ||
                      std::is_same<T, int32_t>::value ||
                      std::is_same<T, uint32_t>::value,
        "Vector type must be one of [float, int32_t, uint32_t]");
    static_assert(
        0 < length && length <= 4, "Vector length must be between 1 to 4");

    UniformVariableType type;
    if (std::is_same<T, float>::value) {
      type = kFloat;
    } else if (std::is_same<T, int32_t>::value) {
      type = kInt;
    } else if (std::is_same<T, uint32_t>::value) {
      type = kUint;
    } else {
      assert(false && "unknown vector type");
    }

    UniformVector(location, name, type, length, 1, &value);
  }

  template <glm::length_t C, glm::length_t R>
  void Uniform(
      uint32_t location, const std::string &name, glm::mat<C, R, float> value)
  {
    static_assert(1 < C && C <= 4 && 1 < R && R <= 4,
        "Matrix must be between 2x2 and 4x4");

    UniformMatrix(location, name, C, R, 1, false, &value);
  }

  void DrawArrays(GLenum mode, GLint first, GLsizei count);

  void DrawElements(GLenum mode, GLsizei count, GLenum type, uint64_t offset,
      GlBuffer *element_array_buffer);

  void Bind(GlProgram *program);

  void Bind(GlVertexArray *vertex_array);

  void Bind(uint32_t binding, std::string name, GlTexture *texture,
      GLenum target, GlSampler *sampler);

  const std::unique_ptr<Impl> pimpl;

 private:
  void UniformVector(uint32_t location, const std::string &name,
      enum UniformVariableType type, uint32_t size, uint32_t count,
      void *value);

  void UniformMatrix(uint32_t location, const std::string &name, uint32_t col,
      uint32_t row, uint32_t count, bool transpose, void *value);
};

}  // namespace zukou
