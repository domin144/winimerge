# SPDX-FileCopyrightText: Copyright © 2024 Dominik Wójt <domin144@o2.pl>
#
# SPDX-License-Identifier: GPL-2.0-or-later

gtest_dep = dependency('gtest', main : true)

test(
    'winimergelib test',
    executable(
        'image_test',
        ['image_test.cpp'],
        dependencies : [winimergelib_dep, gtest_dep]))