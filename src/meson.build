_source = [
  'convert.cc',
  'logger.cc',
  'loop.cc',
  'system.cc',
  'util.cc',

  'base/bounded.cc',
  'base/buffer.cc',
  'base/gl-base-technique.cc',
  'base/gl-buffer.cc',
  'base/gl-program.cc',
  'base/gl-sampler.cc',
  'base/gl-shader.cc',
  'base/gl-texture.cc',
  'base/gl-vertex-array.cc',
  'base/region.cc',
  'base/rendering-unit.cc',
  'base/shm-pool.cc',
  'base/space.cc',
  'base/virtual-object.cc',

  protocols['zen-space']['private-code'],
  protocols['zen-space']['client-header'],
  protocols['zigen']['private-code'],
  protocols['zigen']['client-header'],
  protocols['zigen-shell']['private-code'],
  protocols['zigen-shell']['client-header'],
  protocols['zigen-gles-v32']['private-code'],
  protocols['zigen-gles-v32']['client-header'],
]

_zukou_deps = [
  wayland_client_dep,
  glm_dep,
]

_zukou_lib = library(
  'zukou',
  _source,
  install: true,
  cpp_args: ['-include', 'config.h'],
  dependencies: _zukou_deps,
  include_directories: public_inc,
)

zukou_dep = declare_dependency(
  link_with: _zukou_lib,
  include_directories: public_inc,
)

pkgconfig.generate(
  _zukou_lib,
  name: 'zukou',
  description: 'Client library for ZIGEN windowing system',
  version: meson.project_version(),
)
