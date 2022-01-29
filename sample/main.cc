#include <zukou.h>

#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <memory>

class Box : public zukou::CuboidWindow {
 public:
  static std::shared_ptr<Box> Create(
      std::shared_ptr<zukou::Application> app, glm::vec3 half_size) {
    return std::shared_ptr<Box>(new Box(app, half_size));
  }

 protected:
  Box(std::shared_ptr<zukou::Application> app, glm::vec3 half_size)
      : CuboidWindow(app, half_size) {}

  void RayButton(uint32_t serial, [[maybe_unused]] uint32_t time,
      [[maybe_unused]] uint32_t button, bool pressed) override {
    if (pressed) this->Move(serial);
  }
};

int main() {
  std::shared_ptr<zukou::Application> app = zukou::Application::Create();
  std::shared_ptr<zukou::CuboidWindow> cuboid_window;

  app->Connect("zigen-0");

  // Cuboid Window / Virtual Object must be managed by shared_ptr.
  cuboid_window = Box::Create(app, glm::vec3(0.25));

  app->Run();

  return 0;
}
