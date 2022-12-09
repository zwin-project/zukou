#include <zukou.h>

#include "jpeg-texture.h"
#include "sphere.h"

class CelestialBody final : public zukou::IBoundedDelegate,
                            public zukou::ISystemDelegate
{
 public:
  CelestialBody(const char* texture_path)
      : system_(this),
        bounded_(&system_, this),
        texture_path_(texture_path),
        sphere_(&system_, &bounded_, 8)
  {}

  bool Init(float radius)
  {
    if (!system_.Init()) return false;
    if (!bounded_.Init(glm::vec3(radius))) return false;

    auto jpeg_texture = std::make_unique<JpegTexture>(&system_);

    if (!jpeg_texture->Init()) return false;
    if (!jpeg_texture->Load(texture_path_)) return false;

    sphere_.Bind(std::move(jpeg_texture));

    return true;
  }

  int Run() { return system_.Run(); }

  void Configure(glm::vec3 half_size, uint32_t serial) override
  {
    float radius = glm::min(half_size.x, glm::min(half_size.y, half_size.z));
    sphere_.Render(radius, glm::mat4(1));

    bounded_.AckConfigure(serial);
    bounded_.Commit();
  }

 private:
  zukou::System system_;
  zukou::Bounded bounded_;

  const char* texture_path_;

  Sphere sphere_;
};

const char* usage =
    "Usage: %s <texture>\n"
    "\n"
    "    texture: Surface texture in JPEG format"
    "\n";

int
main(int argc, char const* argv[])
{
  if (argc != 2) {
    fprintf(stderr, "jpeg-file is not specified\n\n");
    fprintf(stderr, usage, argv[0]);
    return EXIT_FAILURE;
  }

  CelestialBody celestial_body(argv[1]);

  if (!celestial_body.Init(0.1)) return EXIT_FAILURE;

  return celestial_body.Run();
}
