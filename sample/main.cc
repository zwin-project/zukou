#include <zukou/zukou.h>

#include <glm/gtx/string_cast.hpp>
#include <memory>

class WindowA : public zukou::CuboidWindow {
 public:
  static std::shared_ptr<WindowA> Create(
      std::shared_ptr<zukou::Application> app, glm::vec3 half_size) {
    return std::shared_ptr<WindowA>(new WindowA(app, half_size));
  }

  void Init() {
    auto self = shared_from_this();
    cube1_.reset(new zukou::primitives::FrameCuboid(app_, self));
    cube1_->Init();
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
    cube1_->set_position(glm::vec3(0, 0.05f, 0.0f));
    cube1_->set_size(glm::vec3(min_half / 1.8f));
    cube1_->set_quaternion(glm::quat(glm::vec3(M_PI / 6, M_PI / 9, 0)));
    cube1_->Draw();
    this->Commit();
  }

  std::unique_ptr<zukou::primitives::FrameCuboid> cube1_;
};

class WindowB : public zukou::CuboidWindow {
 public:
  static std::shared_ptr<WindowB> Create(
      std::shared_ptr<zukou::Application> app, glm::vec3 half_size) {
    return std::shared_ptr<WindowB>(new WindowB(app, half_size));
  }

  void Init() {
    auto self = shared_from_this();
    cube1_.reset(new zukou::primitives::FrameCuboid(app_, self));
    cube2_.reset(new zukou::primitives::FrameCuboid(app_, self));
    cube1_->Init();
    cube2_->Init();
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
    cube1_->set_position(glm::vec3(-0.03, 0.0f, 0.0f));
    cube1_->set_size(glm::vec3(min_half / 3.5f));
    cube1_->set_quaternion(glm::quat(glm::vec3(-M_PI / 10, 0, M_PI / 8)));
    cube1_->set_frame_color(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

    cube2_->set_position(glm::vec3(0, -0.05f, 0.02f));
    cube2_->set_size(glm::vec3(min_half / 2.2f));
    cube2_->set_quaternion(glm::quat(glm::vec3(0, -M_PI / 8, M_PI / 10)));
    cube2_->set_frame_color(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    cube1_->Draw();
    cube2_->Draw();
    this->Commit();
  }

  void RayEnter([[maybe_unused]] uint32_t serial,
      [[maybe_unused]] glm::vec3 origin,
      [[maybe_unused]] glm::vec3 direction) override {
    cube2_->set_frame_color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    cube2_->Draw();
    this->Commit();
  }

  void RayLeave([[maybe_unused]] uint32_t serial) override {
    cube2_->set_frame_color(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    cube2_->Draw();
    this->Commit();
  }

  std::unique_ptr<zukou::primitives::FrameCuboid> cube1_;
  std::unique_ptr<zukou::primitives::FrameCuboid> cube2_;
};

int main() {
  std::shared_ptr<zukou::Application> app = zukou::Application::Create();
  std::shared_ptr<WindowA> window_a;
  std::shared_ptr<WindowB> window_b;

  app->Connect("zigen-0");

  // FrameCuboid Window / Virtual Object must be managed by shared_ptr.
  window_a = WindowA::Create(app, glm::vec3(0.25));
  window_b = WindowB::Create(app, glm::vec3(0.15));
  window_a->Init();
  window_b->Init();

  app->Run();

  return 0;
}
