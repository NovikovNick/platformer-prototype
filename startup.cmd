pushd out\build\x64-Debug\showcases
del *.log
start ggpo_showcase.exe local 7000 127.0.0.1:7001 
start ggpo_showcase.exe remot 7001 127.0.0.1:7000 
popd