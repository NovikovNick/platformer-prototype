# Platformer prototype
#### How to build:
```.env
git submodule init
git submodule update
./external/vcpkg/bootstrap-vcpkg.bat -disableMetrics
```
Build GGPO.

Then open it in Visual Studio as local folder. IDE will detect CMake and run `vcpkg inslall` that will download the rest of the dependencies.