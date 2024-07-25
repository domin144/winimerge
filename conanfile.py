# SPDX-FileCopyrightText: Copyright © 2023 Dominik Wójt <domin144@o2.pl>
#
# SPDX-License-Identifier: GPL-2.0-or-later

from conan import ConanFile
from conan.tools.meson import Meson

class WinIMergeRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "MesonToolchain", "PkgConfigDeps"

    def requirements(self):
        self.requires("freeimage/[>=3.18]")
        # jasper 4.2.0 does not compile with new VC++
        self.requires("jasper/[>=4.2.0]", override=True)

        # Boost.Nowide
        self.requires("boost/[>=1.73]")
        self.requires("gtest/[>=1.12]")

    def build_requirements(self):
        self.tool_requires("meson/[>=1.1.0]")
        self.tool_requires("pkgconf/[>=1.9]")

    def configure(self):
        if self.settings.os == "Windows":
            self.options["boost*"].layout = "tagged"

    def build(self):
        meson = Meson(self)
        meson.configure()
        meson.build()

    def package(self):
        meson = Meson(self)
        meson.install()
