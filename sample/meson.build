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

texture_frag_glsl = custom_target(
  'texture.fragment.h',
  command: [textify, 'texture_fragment_shader_source', '@INPUT@', '@OUTPUT@'],
  input: 'texture.frag',
  output: 'texture.fragment.h',
)

_samples = {
  'box': [
    'box.cc',
    'cuboid.cc',
    color_frag_glsl,
    default_vert_glsl,
  ],
  'celestial-body': [
    'celestial-body.cc',
    'sphere.cc',
    'jpeg-texture.cc',
    color_frag_glsl,
    texture_frag_glsl,
    default_vert_glsl,
  ]
}

_samples_dep = [
  jpeg_dep,
]

foreach name, sources : _samples
  executable(
    'zukou-@0@'.format(name),
    sources,
    install: false,
    dependencies: [ zukou_dep, _samples_dep ],
  )
endforeach
