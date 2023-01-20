deps_libkms_dumb = [
  dep_libdrm,
  dep_libdrm_headers,
]

srcs_libkms_dumb = [
  'kms_dumb.c',
  'kms_dumb.h',
]

lib_kms_dumb = shared_library(
  'kms_dumb',
  srcs_libkms_dumb,
  install: true,
  version: meson.project_version(),
  dependencies: deps_libkms_dumb,
)

pkgconfig.generate(
  lib_kms_dumb,
  name: 'libkms-dumb',
  version: meson.project_version(),
  description: 'libkms compatible library for dumb BO',
  requires_private: [ dep_libdrm ],
)

install_headers([
  'kms_dumb.h'
])
