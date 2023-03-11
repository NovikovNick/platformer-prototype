pushd out\build\x64-Debug\showcases
del *.log
start app_showcase.exe local 7000 127.0.0.1:7001 
start app_showcase.exe remot 7001 127.0.0.1:7000 
popd