set BUILDDIR=%~dp0\plugin\build
if not exist %BUILDDIR% (
   mkdir %BUILDDIR%
)
pushd %BUILDDIR%
dir
cmake -G "Visual Studio 14 2015 Win64" ..
cmake --build . --target ALL_BUILD --config Release
popd