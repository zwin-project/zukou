srcs_zukou_objects = [
  'object-group.cc',
]

lib_zukou_objects = static_library(
  'zukou-objects',
  srcs_zukou_objects,
  install : false,
  include_directories : public_inc,
)

dep_zukou_objects = declare_dependency(
  link_with : lib_zukou_objects,
  include_directories : public_inc,
)
