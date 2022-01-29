project(
  'zukou', 'c', 'cpp',
  version: '0.0.1',
  license: 'Apache-2.0',
  meson_version: '>= 0.52.1',
  default_options:  [ 'warning_level=3', 'werror=true', 'optimization=2', 'cpp_std=gnu++14' ],
)

public_inc = include_directories('include')

# dependencies
dep_wayland_scanner = dependency('wayland-scanner')
dep_wayland_client = dependency('wayland-client')
dep_zgn_protocol = dependency('zigen-protocols')
dep_glm = dependency('glm')

pkgconfig = import('pkgconfig')

subdir('include')
subdir('protocol')
subdir('zukou')
subdir('sample')