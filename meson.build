project(
  'libkms-dumb', 'c',
  version: '1.0.0',
  license: 'MIT',
  meson_version: '>= 0.54.0',
)

config_h = configuration_data()
config_h.set_quoted('PACKAGE', meson.project_name())
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())

cc = meson.get_compiler('c')
add_project_arguments(
  language: 'c'
)

pkgconfig = import('pkgconfig')

dep_libdrm = dependency('libdrm')
dep_libdrm_headers = dep_libdrm.partial_dependency(compile_args: true)

subdir('src')
