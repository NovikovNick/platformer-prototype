# Platformer prototype
#### How to build:
```.env
git submodule init
git submodule update
./external/vcpkg/bootstrap-vcpkg.bat -disableMetrics
pushd ./external/ggpo
cmake -G "Visual Studio 17 2022" -A x64 -B build -DBUILD_SHARED_LIBS=on
popd
```
Open ./external/ggpo/build/GGPO.sln in Visual Studio to build.
Then open root in Visual Studio as local folder. IDE will detect CMake and run `vcpkg inslall` that will download the rest of the dependencies.