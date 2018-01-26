
pushd %~dp0\..
set ROOTD=%CD%
popd

if not exist %ROOTD%\bin (
   mkdir %ROOTD%\bin
)

pushd %ROOTD%\c++\addon
call node-gyp configure --msvs_version=2015
call node-gyp build
copy build\Release\node_plugin.node %ROOTD%\bin\windows-x64.node
popd

set PLUGIN_BUILD=%ROOTD%\c++\plugin\build
if exist %PLUGIN_BUILD% (
   rmdir /S /Q %PLUGIN_BUILD%
)
mkdir %PLUGIN_BUILD%
pushd %PLUGIN_BUILD%
cmake -G "Visual Studio 14 2015 Win64" ..
cmake --build . --target ALL_BUILD --config Release
copy  Release\calc.dll %ROOTD%\bin\calc.dll
popd