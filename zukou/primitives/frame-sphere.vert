#version 410

uniform mat4 zMVP;
uniform mat4 transform;
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
out vec2 uvOut;

void main() {
  uvOut = uv;
  gl_Position = zMVP * transform * position;
}
