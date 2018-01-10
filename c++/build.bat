set __DIR=%~dp0

REM build plugin
pushd %__DIR%\addon

call node-gyp configure --msvs_version=2015
call node-gyp build
copy build\Release\node_plugin.node ..\..\bin\windows-x64.node
popd

pushd %__DIR%\plugin
cmake -G "Visual Studio 14 2015 Win64" .
cmake --build . --target ALL_BUILD --config Release
copy  Release\example.dll ..\..\bin\example.dll
popd