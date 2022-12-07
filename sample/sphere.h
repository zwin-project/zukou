// latitude is divided into 2 * resolution segments
//
// ex) resolution = 4
//
// --- north pole (+y)
// --------
// ----------
// -----------
// ----------- equator
// -----------
// ----------
// --------
// --- south pole (-y)
//
// longitude is divided into 4 * resolution segments
//
// ex) resolution = 3, viewing from +y
//
//  -z
//  --- __
//  |  /   -
//  | /   __--
//  |/ _--    -
// O.---------- +x
//  |\ --__   -
//  | \    ---
//  |  \  _-
//  --- .
//  +z
//
// The north and south poles and the longitudinal line at the texture juncture
// have duplicated vertices. The number of total vertices is
//
//            (4 * resolution + 1) * (2 * resolution + 1)
//
// ex) resolution = 2
//
// i=+3 n=0 v=0  0----1----2----3----4----5----6----7----8  north pole
// i=+2 n=1      9----10---11---12---13---14---15---16---17
// i=+1 n=2      18---19---20---21---22---23---24---25---26
// i=+0 n=3      27---28---29---30---31---32---33---34---35
// i=-1 n=4      36---37---38---39---40---41---42---43---44
// i=-2 n=5      45---46---47---48---49---50---51---52---53
// i=-3 n=6 v=1  54---55---56---57---58---59---60---61---62 south pole
//
//               j=0  j=1  j=2  j=3  j=4  j=5  j=6  j=7  j=8
//               u=0                                     u=1
//
// Vertices are stored in this order.
// We use i,n,j like above to represent longitudinal and latitudinal lines.
//
// The vertices are aligned counterclockwise when viewed from the +y side,
// starting from (x, z) = (+x, 0) (right-handed coordinate)
//
//  j                 ( x,  z)
// ----------------------------
//  0              -> (+x,  0)
//  resolution     -> ( 0, -z)
//  2 * resolution -> (-x,  0)
//  3 * resolution -> ( 0, +z)
//  4 * resolution -> (+x,  0)
//

#pragma once

#include <zukou.h>

#include <vector>

class Sphere
{
  struct Vertex {
    Vertex(float x, float y, float z, float u, float v);
    float x, y, z;
    float u, v;
  };

 public:
  DISABLE_MOVE_AND_COPY(Sphere);
  Sphere() = delete;
  Sphere(zukou::System* system, zukou::VirtualObject* virtual_object,
      int32_t resolution = 8, bool wire = false);
  ~Sphere();

  void Bind(std::unique_ptr<zukou::GlTexture> texture);

  bool Render(float radius, glm::mat4 transform);

 private:
  bool Init();

  void ConstructVertices();
  void ConstructElements();

  bool initialized_ = false;

  zukou::VirtualObject* virtual_object_;

  const int32_t resolution_;
  const bool wire_;

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
  std::unique_ptr<zukou::GlTexture> texture_;
  zukou::GlSampler sampler_;

  zukou::RenderingUnit rendering_unit_;
  zukou::GlBaseTechnique base_technique_;

  std::vector<Vertex> vertices_;
  std::vector<ushort> elements_;

  inline size_t vertex_buffer_size();

  inline size_t element_array_buffer_size();

  inline size_t pool_size();
};

inline size_t
Sphere::vertex_buffer_size()
{
  return sizeof(decltype(vertices_)::value_type) * vertices_.size();
}

inline size_t
Sphere::element_array_buffer_size()
{
  return sizeof(decltype(elements_)::value_type) * elements_.size();
}

inline size_t
Sphere::pool_size()
{
  return vertex_buffer_size() + element_array_buffer_size();
}
