# SPDX-FileCopyrightText: Copyright © 2023 Dominik Wójt <domin144@o2.pl>
#
# SPDX-License-Identifier: GPL-2.0-or-later

winimerge_rc = windows.compile_resources(
    'WinIMerge.rc',
    depends : version_header,
    include_directories : version_header_inc
)

executable(
    'WinIMerge',
    'WinIMerge.cpp',
    winimerge_rc,
    win_subsystem : 'windows',
    dependencies : [winimergelib_dep]
)
