#version 410

uniform mat4 zMVP;
uniform mat4 transform;
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 v2UVcoordsIn;
out vec2 v2UVcoords;

void main() {
  v2UVcoords = v2UVcoordsIn;
  gl_Position = zMVP * transform * position;
}
