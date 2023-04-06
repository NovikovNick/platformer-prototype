pushd out\build\x64-Debug
del *.log
start platformer-ggpo-0.0.5.exe local 7000 127.0.0.1:7001 
start platformer-ggpo-0.0.5.exe remot 7001 127.0.0.1:7000 
popd