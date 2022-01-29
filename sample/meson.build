cube_vertex_glsl = custom_target(
  'cube-vert.h',
  command: cmd_textify + [ '-n', 'cube_vertex_shader' ],
  input: 'cube.vert',
  output: 'cube-vert.h'
)

cube_fragment_glsl = custom_target(
  'cube-frag.h',
  command: cmd_textify + [ '-n', 'cube_fragment_shader' ],
  input: 'cube.frag',
  output: 'cube-frag.h'
)

deps_zukou_sample = [
  dep_zukou,
]

srcs_zukou_sample = [
  'cube.cc',
  'main.cc',
  cube_fragment_glsl,
  cube_vertex_glsl,
  zigen_client_protocol_h,
  zigen_opengl_client_protocol_h,
]

executable(
  'zukou-sample',
  srcs_zukou_sample,
  install : false,
  dependencies : deps_zukou_sample,
)
