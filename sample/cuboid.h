//                                      /
//                                     /
//                 E.-------------------------.H
//                 /|                /       /|
//                / |        ^y     /       / |
//               /  |        |     /       /  |
//              /   |        |    /       /   |
//             /    |        |   /       /    |
//            /     |        |  /       /     |
//           /      |        | /       /      |
//         A.-------------------------.D      |
//  --------|--------------- / -------|------------>x
//          |      F.-------/---------|-------.G
//          |      /       / |        |      /
//          |     /       /  |        |     /
//          |    /       /   |        |    /
//          |   /       /    |        |   /
//          |  /       /     |        |  /
//          | /       /z+    |        | /
//          |/               |        |/
//         B.-------------------------.C
//                           |
//                           |
//                           |

#pragma once

#include <zukou.h>

#include <array>

/**
 * The virtual object passed to the constructor must be valid during the
 * lifetime of this Cuboid object.
 */
class Cuboid
{
  struct Vertex {
    float x, y, z;
    float u, v;
  };

 public:
  DISABLE_MOVE_AND_COPY(Cuboid);
  Cuboid() = delete;
  Cuboid(zukou::System* system, zukou::VirtualObject* virtual_object);
  ~Cuboid();

  bool Render(glm::vec3 half_size, glm::mat4 transform, glm::vec4 color);

 private:
  bool Init();

  bool initialized_ = false;

  zukou::VirtualObject* virtual_object_;

  int fd_ = 0;
  zukou::ShmPool pool_;
  zukou::Buffer vertex_buffer_;
  zukou::Buffer element_array_buffer_;

  zukou::GlBuffer gl_vertex_buffer_;
  zukou::GlBuffer gl_element_array_buffer_;
  zukou::GlVertexArray vertex_array_;
  zukou::GlShader vertex_shader_;
  zukou::GlShader fragment_shader_;
  zukou::GlProgram program_;

  zukou::RenderingUnit rendering_unit_;
  zukou::GlBaseTechnique base_technique_;

  const Vertex A = {-1, +1, -1, 0, 1};
  const Vertex B = {-1, -1, -1, 0, 0};
  const Vertex C = {+1, -1, -1, 1, 0};
  const Vertex D = {+1, +1, -1, 1, 1};
  const Vertex E = {-1, +1, +1, 1, 0};
  const Vertex F = {-1, -1, +1, 1, 1};
  const Vertex G = {+1, -1, +1, 0, 1};
  const Vertex H = {+1, +1, +1, 0, 0};
  const ushort a = 0, b = 1, c = 2, d = 3, e = 4, f = 5, g = 6, h = 7;

  std::array<Vertex, 8> vertices_ = {A, B, C, D, E, F, G, H};
  std::array<ushort, 24> elements_ = {
      a, b, /**/ b, c, /**/ c, d, /**/ d, a,  //
      e, f, /**/ f, g, /**/ g, h, /**/ h, e,  //
      a, e, /**/ b, f, /**/ c, g, /**/ d, h,  //
  };

  inline size_t vertex_buffer_size()
  {
    return sizeof(decltype(vertices_)::value_type) * vertices_.size();
  }

  inline size_t element_array_buffer_size()
  {
    return sizeof(decltype(elements_)::value_type) * elements_.size();
  }

  inline size_t pool_size()
  {
    return vertex_buffer_size() + element_array_buffer_size();
  }
};
