#include <zukou.h>

#include <glm/gtx/string_cast.hpp>
#include <memory>

#include "box.h"

int main() {
  std::shared_ptr<zukou::Application> app = zukou::Application::Create();
  std::shared_ptr<zukou::CuboidWindow> cuboid_window;

  app->Connect("zigen-0");

  // Cuboid Window / Virtual Object must be managed by shared_ptr.
  cuboid_window = Box::Create(app, glm::vec3(0.25));

  app->Run();

  return 0;
}
