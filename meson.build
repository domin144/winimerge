# SPDX-FileCopyrightText: Copyright © 2023 Dominik Wójt <domin144@o2.pl>
#
# SPDX-License-Identifier: GPL-2.0-or-later

project(
    'winimerge',
    'cpp',
    version : '1.0.40.0',
    license : 'GPL-2.0-or-later',
    license_files : 'GPL.txt',
    default_options : ['cpp_std=vc++17'])

add_project_arguments('-D_UNICODE', '-DUNICODE', language : ['c', 'cpp'])

freeimage_dep = dependency('freeimage', version : '>=3.18.0')
openimageio_dep = dependency('OpenImageIO', version : '>=2.2')
boost_dep = dependency('boost', modules : ['nowide'], version : '>=1.73')

subdir('src')
subdir('tests')