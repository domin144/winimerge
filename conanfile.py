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
        self.requires("openimageio/[>=2.2]")

        # Boost.Nowide
        self.requires("boost/[>=1.73]")
        self.requires("gtest/[>=1.12]")
        
        # Hacks:
        # Resolve conflict
        self.requires("openexr/[>=3.2.3]", override=True)
        # Resolve conflict
        self.requires("libpng/[>=1.6.42]", override=True)
        # Jasper 4.0.0 does not build with Visual Studio C++ 2022
        self.requires("jasper/[>=4.2.0]", override=True)

    def build_requirements(self):
        self.tool_requires("meson/[>=1.1.0]")
        self.tool_requires("pkgconf/[>=1.9]")

    def configure(self):
        if self.settings.os == "Windows":
            self.options["boost*"].layout = "tagged"
        
        # Hacks:
        self.options["freeimage/*"].with_tiff = False
        self.options["freeimage/*"].with_openexr = False
        self.options["openimageio/*"].with_ffmpeg = False

    def build(self):
        meson = Meson(self)
        meson.configure()
        meson.build()

    def package(self):
        meson = Meson(self)
        meson.install()
