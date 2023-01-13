#include <zukou.h>

#include <linux/input.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <random>

#include "cuboid.h"

class Box : public zukou::IBoundedDelegate, public zukou::ISystemDelegate
{
 public:
  Box()
      : system_(this),
        bounded_(&system_, this),
        outer_cuboid_(&system_, &bounded_),
        inner_cuboid_(&system_, &bounded_)
  {}

  bool Init(glm::vec3 half_size)
  {
    if (!system_.Init()) return false;
    if (!bounded_.Init(half_size)) return false;

    float x, y, z;
    std::random_device seed_gen;
    std::mt19937 random_engine(seed_gen());
    std::uniform_real_distribution<> rand(-1.0f, 1.0f);

    x = rand(random_engine);
    y = rand(random_engine);
    z = rand(random_engine);
    inner_cuboid_center_relative_ = glm::vec3(x, y, z) * 0.75f;

    x = rand(random_engine);
    y = rand(random_engine);
    z = rand(random_engine);
    inner_cuboid_quaternion_ = glm::quat(glm::vec3(x, y, z) * 2.0f * M_PIf);

    return true;
  }

  void RayEnter(uint32_t /*serial*/, zukou::VirtualObject* /*virtual_object*/,
      glm::vec3 /*origin*/, glm::vec3 /*direction*/) override
  {
    intersecting_ = true;
    bounded_.NextFrame();
    bounded_.SetTitle("drag to move!");
    bounded_.Commit();
  };

  void RayLeave(
      uint32_t /*serial*/, zukou::VirtualObject* /*virtual_object*/) override
  {
    intersecting_ = false;
    bounded_.NextFrame();
    bounded_.SetTitle("Zukou Simple Box");
    bounded_.Commit();
  };

  void RayButton(uint32_t serial, uint32_t /*time*/, uint32_t button,
      bool pressed) override
  {
    if (button == BTN_LEFT && pressed) {
      bounded_.Move(serial);
    }
  };

  void Configure(glm::vec3 half_size, uint32_t serial) override
  {
    CalcInnerCuboid(half_size);

    outer_cuboid_.Render(half_size, glm::mat4(1), outer_cuboid_color_);
    inner_cuboid_.Render(
        inner_cuboid_half_size_, inner_cuboid_transform_, inner_cuboid_color_);

    zukou::Region region(&system_);
    region.Init();

    region.AddCuboid(inner_cuboid_half_size_, inner_cuboid_center_,
        inner_cuboid_quaternion_);

    bounded_.SetTitle("Zukou Simple Box");
    bounded_.SetRegion(&region);

    bounded_.AckConfigure(serial);
    bounded_.Commit();
  }

  void Frame(uint32_t /*time*/) override
  {
    bool need_next_frame = false;

    if (intersecting_ && inner_cuboid_color_.b < 1) {
      inner_cuboid_color_.b += 0.04;
      need_next_frame = inner_cuboid_color_.b < 1;
    }

    if (!intersecting_ && inner_cuboid_color_.b > 0) {
      inner_cuboid_color_.b -= 0.04;
      need_next_frame = inner_cuboid_color_.b > 0;
    }

    inner_cuboid_.Render(
        inner_cuboid_half_size_, inner_cuboid_transform_, inner_cuboid_color_);

    if (need_next_frame) bounded_.NextFrame();

    bounded_.Commit();
  }

  int Run() { return system_.Run(); }

 private:
  void CalcInnerCuboid(glm::vec3 half_size)
  {
    float shortest_edge =
        glm::min(half_size.x, glm::min(half_size.y, half_size.z));

    inner_cuboid_half_size_ = glm::vec3(shortest_edge / 7.0f);

    inner_cuboid_center_ = half_size * inner_cuboid_center_relative_;

    inner_cuboid_transform_ =
        glm::translate(glm::mat4(1), inner_cuboid_center_) *
        glm::toMat4(inner_cuboid_quaternion_);
  }

  zukou::System system_;
  zukou::Bounded bounded_;

  Cuboid outer_cuboid_;
  glm::vec4 outer_cuboid_color_ = {1, 0, 0, 1};

  Cuboid inner_cuboid_;
  glm::vec3 inner_cuboid_half_size_;
  glm::vec3 inner_cuboid_center_relative_;
  glm::vec3 inner_cuboid_center_;
  glm::quat inner_cuboid_quaternion_;
  glm::mat4 inner_cuboid_transform_;
  glm::vec4 inner_cuboid_color_ = {1, 0, 0, 1};

  bool intersecting_ = false;
};

int
main()
{
  Box box;

  glm::vec3 half_size(0.25, 0.25, 0.25);

  if (!box.Init(half_size)) return EXIT_FAILURE;

  return box.Run();
}
