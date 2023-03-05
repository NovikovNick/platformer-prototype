# Platformer prototype
#### How to build:
```.env
git submodule init
git submodule update
./external/vcpkg/bootstrap-vcpkg.bat -disableMetrics
./external/vcpkg/vcpkg.exe install protobuf:x64-windows-static
./external/vcpkg/vcpkg.exe install sfml:x64-windows-static
```
