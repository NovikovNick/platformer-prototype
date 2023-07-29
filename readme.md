[![windows-latest](https://github.com/NovikovNick/platformer-prototype/actions/workflows/github-ci.yml/badge.svg)](https://github.com/NovikovNick/platformer-prototype/actions/workflows/github-ci.yml)

# Platformer prototype
The library was created as a Proof of Concept for using the Rollback Network to compensate lag in action games. Network part is implemented by [GGPO](https://www.ggpo.net/). To support a deterministic state used [fixed-point math library](https://github.com/MikeLankamp/fpm). Serialization/Deserialization is implemented by Protobuf. Also for debugging added a simple GUI on SFML+ImGui.

The project uses [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

Package manager - some of dependencies is loaded by vcpkg.

Cmake builds the library in three variations:
* sync - only core functionality without network support. The tickrate is set by calling the update method. It means the tickrate is bound to the framerate.
* async - also core functionality without a network, but the tickrate is fixed during session. Library calls update method in separate thread themself.
* ggpo - full functionality with p2p connection and NAT traversal.
Each variation is built as a dll and additional exe file with an example of usage. 6 files in total.
CI/CD - unit tests executed by github actions

#### How to build:
Execute:
```.env
git submodule update --init
./external/vcpkg/bootstrap-vcpkg.bat -disableMetrics
cmake -S ./external/ggpo -D BUILD_SHARED_LIBS=off -G "Visual Studio 17 2022" -A x64 -B ./external/ggpo/build
```
Then open ./external/ggpo/build/GGPO.sln in Visual Studio to build.
Then open root in Visual Studio as local folder. IDE will detect CMake and run `vcpkg inslall` that will download the rest of the dependencies.