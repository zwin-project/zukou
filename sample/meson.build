default_vert_glsl = custom_target(
  'default.vertex.h',
  command: [textify, 'default_vertex_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'default.vert',
  output: 'default.vert.h',
)

color_frag_glsl = custom_target(
  'color.fragment.h',
  command: [textify, 'color_fragment_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'color.frag',
  output: 'color.fragment.h',
)

_samples = {
  'box': [
    'box.cc',
    'cuboid.cc',
    color_frag_glsl,
    default_vert_glsl,
  ],
}

foreach name, sources : _samples
  executable(
    'zukou-@0@'.format(name),
    sources,
    install: false,
    dependencies: [ zukou_dep ],
  )
endforeach
