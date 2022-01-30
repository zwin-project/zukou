#include <zukou/zukou.h>

#include <glm/gtx/string_cast.hpp>
#include <memory>

#include "cube.h"

class WindowA : public zukou::CuboidWindow {
 public:
  static std::shared_ptr<WindowA> Create(
      std::shared_ptr<zukou::Application> app, glm::vec3 half_size) {
    return std::shared_ptr<WindowA>(new WindowA(app, half_size));
  }

  void Init() {
    auto self = shared_from_this();
    cube1_.reset(new Cube(app_, self));
  }

 private:
  WindowA(std::shared_ptr<zukou::Application> app, glm::vec3 half_size)
      : CuboidWindow(app, half_size) {}

  void RayButton([[maybe_unused]] uint32_t serial,
      [[maybe_unused]] uint32_t time, [[maybe_unused]] uint32_t button,
      bool pressed) override {
    if (pressed) this->Move(serial);
  }

  void Configured([[maybe_unused]] uint32_t serial) override {
    float min_half =
        half_size().x > half_size().y
            ? (half_size().y > half_size().z ? half_size().z : half_size().y)
            : (half_size().x > half_size().z ? half_size().z : half_size().x);
    cube1_->SetPos(glm::vec3(0, 0.05f, 0.0f));
    cube1_->SetSize(min_half / 1.8f);
    cube1_->SetQuaternion(glm::quat(glm::vec3(M_PI / 6, M_PI / 9, 0)));
    this->Commit();
  }

  std::unique_ptr<Cube> cube1_;
};

class WindowB : public zukou::CuboidWindow {
 public:
  static std::shared_ptr<WindowB> Create(
      std::shared_ptr<zukou::Application> app, glm::vec3 half_size) {
    return std::shared_ptr<WindowB>(new WindowB(app, half_size));
  }

  void Init() {
    auto self = shared_from_this();
    cube1_.reset(new Cube(app_, self));
    cube2_.reset(new Cube(app_, self));
  }

 private:
  WindowB(std::shared_ptr<zukou::Application> app, glm::vec3 half_size)
      : CuboidWindow(app, half_size) {}

  void RayButton([[maybe_unused]] uint32_t serial,
      [[maybe_unused]] uint32_t time, [[maybe_unused]] uint32_t button,
      bool pressed) override {
    if (pressed) this->Move(serial);
  }

  void Configured([[maybe_unused]] uint32_t serial) override {
    float min_half =
        half_size().x > half_size().y
            ? (half_size().y > half_size().z ? half_size().z : half_size().y)
            : (half_size().x > half_size().z ? half_size().z : half_size().x);
    cube1_->SetPos(glm::vec3(-0.03, 0.0f, 0.0f));
    cube1_->SetSize(min_half / 3.5f);
    cube1_->SetQuaternion(glm::quat(glm::vec3(-M_PI / 10, 0, M_PI / 8)));

    cube2_->SetPos(glm::vec3(0, -0.05f, 0.02f));
    cube2_->SetSize(min_half / 2.2f);
    cube2_->SetQuaternion(glm::quat(glm::vec3(0, -M_PI / 8, M_PI / 10)));
    cube2_->SetColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    this->Commit();
  }

  void RayEnter([[maybe_unused]] uint32_t serial,
      [[maybe_unused]] glm::vec3 origin,
      [[maybe_unused]] glm::vec3 direction) override {
    cube2_->SetColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    this->Commit();
  }

  void RayLeave([[maybe_unused]] uint32_t serial) override {
    cube2_->SetColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    this->Commit();
  }

  std::unique_ptr<Cube> cube1_;
  std::unique_ptr<Cube> cube2_;
};

int main() {
  std::shared_ptr<zukou::Application> app = zukou::Application::Create();
  std::shared_ptr<WindowA> window_a;
  std::shared_ptr<WindowB> window_b;

  app->Connect("zigen-0");

  // Cuboid Window / Virtual Object must be managed by shared_ptr.
  window_a = WindowA::Create(app, glm::vec3(0.25));
  window_b = WindowB::Create(app, glm::vec3(0.15));
  window_a->Init();
  window_b->Init();

  app->Run();

  return 0;
}
