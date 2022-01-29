deps_zukou = [
  dep_glm,
  dep_wayland_client,
]

srcs_zukou = [
  'application.cc',
  'buffer.cc',
  'cuboid-window.cc',
  'opengl-component.cc',
  'opengl-element-array-buffer.cc',
  'opengl-shader-program.cc',
  'opengl-texture.cc',
  'opengl-vertex-buffer.cc',
  'ray.cc',
  'virtual-object.cc',
  zigen_protocol_c,
  zigen_client_protocol_h,
  zigen_shell_protocol_c,
  zigen_shell_client_protocol_h,
  zigen_opengl_protocol_c,
  zigen_opengl_client_protocol_h,
]

lib_zukou = static_library(
  'zukou',
  srcs_zukou,
  install : true,
  dependencies : deps_zukou,
  include_directories : public_inc,
)

dep_zukou = declare_dependency(
  link_with : lib_zukou,
  include_directories : public_inc,
)

pkgconfig.generate(
  lib_zukou,
  name : 'libzukou',
)
