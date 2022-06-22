@echo off

if not exist build mkdir build

xcopy /y /q ts_libs build

pushd build
cl /Zi /nologo /FC /I..\ts_headers\ ..\app\scratch_main.c /link ../engine_core.lib /out:scratch.exe
popd
