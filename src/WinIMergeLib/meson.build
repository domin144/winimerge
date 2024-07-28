# SPDX-FileCopyrightText: Copyright © 2023 Dominik Wójt <domin144@o2.pl>
#
# SPDX-License-Identifier: GPL-2.0-or-later

windows = import('windows')

winimergelib_inc = include_directories('.')

winimergelib_rc = windows.compile_resources(
    'WinIMergeLib.rc',
    depends : version_header,
    include_directories : version_header_inc
)

winimergelib = library(
    'WinIMergeLib',
    'Win78Libraries.cpp',
    'WinIMergeLib.cpp',
    winimergelib_rc,
    version_header,
    include_directories : [winimergelib_inc],
    dependencies : [freeimage_dep, openimageio_dep, boost_dep],
    vs_module_defs : 'WinIMergeLib.def'
)

winimergelib_dep = declare_dependency(
    link_with : winimergelib,
    include_directories : [winimergelib_inc],
    dependencies : [freeimage_dep, openimageio_dep, boost_dep]
)
