[![windows-latest](https://github.com/NovikovNick/platformer-prototype/actions/workflows/github-ci.yml/badge.svg)](https://github.com/NovikovNick/platformer-prototype/actions/workflows/github-ci.yml)

# Platformer prototype
#### How to build:
```.env
git submodule update --init
./external/vcpkg/bootstrap-vcpkg.bat -disableMetrics
cmake -S ./external/ggpo -D BUILD_SHARED_LIBS=off -G "Visual Studio 17 2022" -A x64 -B ./external/ggpo/build
cmake --build ./external/ggpo/build --config Debug
cmake --build ./external/ggpo/build --config Release
```
Then open root in Visual Studio as local folder. IDE will detect CMake and run `vcpkg inslall` that will download the rest of the dependencies.