srcs_zukou_entities = [
  'frame-cuboid.cc',
  'intersection.cc',
]

lib_zukou_entities = static_library(
  'zukou-entities',
  srcs_zukou_entities,
  install : false,
  include_directories : public_inc,
)

dep_zukou_entities = declare_dependency(
  link_with : lib_zukou_entities,
  include_directories : public_inc,
)
