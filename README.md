# WinIMerge #

WinIMerge is a simple image diff/merge tool like WinMerge.

## Screenshot

![winimerge.png](https://bitbucket.org/repo/RoKbrr/images/3384177401-winimerge.png)

## Dependencies

This software uses the FreeImage open source image library.

See http://freeimage.sourceforge.net for details.

FreeImage is used under the the GNU GPL version.

## How to build (Visual Studio 2019)
~~~
git clone https://github.com/winmerge/winimerge
git clone https://github.com/winmerge/freeimage
cd winimerge
buildbin.vs2019.cmd
~~~

## How to build (conan + meson)
Install dependencies:
- git - windows installer
- python - windows installer
- visual studio C++ - windows installer
- meson, ninja, conan - pip

Create conan profile for VS-C++, e.g.
~~~
conan profile detect --force
conan profile path default
~~~
Open the displayed path and make sure C++17 is enabled:
~~~
[settings]
arch=x86_64
build_type=Release
compiler=msvc
compiler.cppstd=17
compiler.runtime=dynamic
compiler.version=193
os=Windows
~~~
Get sources and build (this was tested in git-bash):
~~~
git clone https://github.com/winmerge/winimerge
cd winimerge
conan build . --output-folder=builddir --build=missing --settings=build_type=Debug
~~~

## Run
~~~
PATH=./builddir/src/WinIMergeLib:$PATH ./builddir/src/WinIMerge/WinIMerge.exe
~~~

## License

GPL2
