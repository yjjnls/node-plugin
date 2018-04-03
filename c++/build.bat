@echo OFF

rem build arch (x86 | x64) generator (2015|2017)

set msvs=%1
set arch=%2

set __CWD__=%CD%
pushd %~dp0\..
set __ROOT__=%CD%
popd


if not defined arch ( set arch=x64 )
if "[%arch%]" neq "[x86]" (
   if "[%arch%]" neq "[x64]" (
       set arch=x64
	   echo force arch to %arch%
   )
)

if not defined msvs set msvs=2015

set __bin__=%__ROOT__%\bin\win\%arch%
if exist %__bin__% del /Q /S %__bin__%

REM --------------------------
REM     plugin.addon
REM --------------------------
set project=plugin.addon

pushd %~dp0\addon


set command="node-gyp configure "
set options=--arch=x64
if %arch% == x86 (
   set options=--arch=win32
)


if defined msvs ( set options=%options% --msvs_version %msvs%)


call node-gyp configure %options% 
if %errorlevel% neq 0 goto _ERROR

set options=
if defined _DEBUG (set options=--debug)
call node-gyp build %options%
if %errorlevel% neq 0 goto _ERROR

if not exist %__bin__% mkdir %__bin__%
if defined _DEBUG (
    copy build\Debug\plugin.node %__bin__%\.
) else (
    copy build\Release\plugin.node %__bin__%\.
)
popd
cls

REM -----------------------------
REM     converter example
REM -----------------------------

pushd %~dp0\plugin
set project=converter
if not exist build mkdir build
cd build
set options=-G "Visual Studio

if %msvs% == 2015 set options=%options% 14 2015
if %msvs% == 2017 set options=%options% 15 2017
if %arch% == x64  set options=%options% Win64
set options=%options%"
cmake %options% ..
set options=--config Release
if defined _DEBUG set options=--config Debug
cmake --build . --target ALL_BUILD %options%
if %errorlevel% != 0 goto _ERROR

if defined _DEBUG (
    copy Debug\converter.dll %__bin__%\.
) else (
    copy Release\converter.dll %__bin__%\.
)

popd
cls
:success
echo.
echo.

set config=Release
if defined _DEBUG ( set config=Debug )

echo         node-plugin addon and example build success !
echo         Arch   : %arch%
echo         Build  : %config%
echo         Dir    : %__bin__%
echo.
echo.

dir /b/s %__bin__%


GOTO _END

:_ERROR
echo "build %project% failed !"
popd

:_END
cd %__CWD__%
