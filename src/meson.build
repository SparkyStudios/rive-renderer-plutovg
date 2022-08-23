# Copyright (c) 2021-present Sparky Studios. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake = import('cmake')

compiler_flags = ['-DRIVE_BUILD']

cc = meson.get_compiler('cpp')
override_options = []
if (cc.get_id() == 'clang-cl')
   override_options += ['cpp_eh=none', 'cpp_rtti=false']
elif (cc.get_id() != 'msvc')
    compiler_flags += ['-fno-exceptions', '-fno-rtti',
                       '-fno-unwind-tables' , '-fno-asynchronous-unwind-tables',
                       '-Woverloaded-virtual', '-Wno-unused-parameter']
endif

plutovg_proj = cmake.subproject('plutovg')
plutovg_dep = plutovg_proj.dependency('plutovg')

source_files = [
    'plutonriver.cpp',
    'stb_image.h'
]

plutonriver_dep = declare_dependency(
    include_directories : include_directories('.'),
    sources : source_files,
    dependencies : plutovg_dep
)

plutonriver_lib_shared = library(
    'plutonriver',
    include_directories : headers,
    version             : meson.project_version(),
    dependencies        : [rive_dep, plutovg_dep, plutonriver_dep],
    install             : true,
    cpp_args            : compiler_flags,
    override_options    : override_options
)

plutonriver_lib_static = static_library(
    'plutonriver',
    include_directories : headers,
    dependencies        : [rive_dep, plutovg_dep, plutonriver_dep],
    install             : true,
    cpp_args            : compiler_flags,
    override_options    : override_options
)

plutonriver_lib_shared_dep = declare_dependency(
    include_directories : headers,
    link_with           : plutonriver_lib_shared
)

plutonriver_lib_static_dep = declare_dependency(
    include_directories : headers,
    link_with           : plutonriver_lib_static
)

pkg_mod = import('pkgconfig')

pkg_mod.generate(
    libraries    : [plutonriver_lib_shared, plutonriver_lib_static],
    version      : meson.project_version(),
    name         : 'libplutonriver',
    filebase     : 'plutonriver',
    description  : 'A Rive Animation Runtime Renderer using PlutoVG'
)
