project(
  'zukou', 'c', 'cpp',
  version: '0.0.1',
  license: 'MIT',
  meson_version: '>= 0.58.0',
  default_options: ['warning_level=3', 'werror=true', 'optimization=2', 'c_std=gnu11', 'cpp_std=gnu++17'],
)

cxx = meson.get_compiler('cpp')
cdata = configuration_data()

global_args = []
global_args_maybe = [
  '-D_GNU_SOURCE',
  '-DWL_HIDE_DEPRECATED',
  '-fvisibility=hidden',
  '-Wstrict-prototypes',
  '-Wno-gnu-zero-variadic-macro-arguments',
]

foreach arg : global_args_maybe
  if cxx.has_argument(arg)
    global_args += arg
  endif
endforeach
add_project_arguments(global_args, language: 'cpp')

have_funcs = [
  'memfd_create',
  'posix_fallocate',
]

foreach func : have_funcs
  cdata.set('HAVE_' + func.underscorify().to_upper(), cxx.has_function(func))
endforeach

wayland_req = '>= 1.18.0'
zen_protocols_req = '0.0.1'

glm_dep = dependency('glm')
wayland_client_dep = dependency('wayland-client', version: wayland_req)
wayland_scanner_dep = dependency('wayland-scanner')
zen_protocols_dep = dependency('zen-protocols', version: zen_protocols_req)
zigen_protocols_dep = dependency('zigen-protocols')
jpeg_dep = dependency('libjpeg')

public_inc = include_directories('include')

pkgconfig = import('pkgconfig')

textify = files('tools/textify.sh')[0]

configure_file(
  output: 'config.h',
  configuration: cdata,
)

install_headers('include/zukou.h')

subdir('protocol')
subdir('src')
subdir('sample')
