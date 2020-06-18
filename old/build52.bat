@echo off
setlocal
::If you are not Matthew Sessions, or you are but compiling on Lionel instead of Martin, you'll need to adjust these environment variables:
set SDK="C:\devtools\WindowskSDK"
set DDK="C:\devtools\WindowsDDK"
set DirectX="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)"
set LuaP="C:\Program Files (x86)\Lua\5.2"

set CL86="C:\devtools\WindowsDDK\bin\x86\x86\cl.exe"
set LINK86="C:\devtools\WindowsDDK\bin\x86\x86\link.exe"

%CL86% /c /I%SDK%\Include /I%DDK%\inc\crt /I%DDK%\inc\api /I%LuaP%\include /I%DirectX%\Include /arch:SSE2 /Ox /TcLuaXInput.c /FoLuaXInput.obj
if not errorlevel 1 (
%LINK86% /DLL /EXPORT:luaopen_LuaXInput /LIBPATH:%DDK%\lib\win7\i386 /LIBPATH:%DDK%\lib\Crt\i386 /LIBPATH:%DirectX%\Lib\x86 /LIBPATH:%LuaP%\lib /DEFAULTLIB:lua52.lib /DEFAULTLIB:XINPUT.lib LuaXInput.obj /OUT:LuaXInput52.dll
)

del LuaXInput.obj
del LuaXInput.exp
del LuaXInput.lib
endlocal