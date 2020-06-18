@echo off
setlocal
set SDKInc="C:\devtools\WindowskSDK\Include"
set SDKLib="C:\devtools\WindowskSDK\Lib"

set DDKInc="C:\devtools\WindowsDDK\inc\ddk"

set APIInc="C:\devtools\WindowsDDK\inc\api"
set APILib="C:\devtools\WindowsDDK\lib\win7\i386"

set CRTInc="C:\devtools\WindowsDDK\inc\crt"
set CRTLib=C:\devtools\WindowsDDK\lib\Crt\i386

set LuaPInc="C:\Program Files (x86)\Lua\5.1\include"
set LuaPLib="C:\Program Files (x86)\Lua\5.1\lib"

set LuaWInc="C:\Projects\luaW"

set DirectXInc="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include"
set DirectXLib="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86"

set CL86="C:\devtools\WindowsDDK\bin\x86\x86\cl.exe"
set LINK86="C:\devtools\WindowsDDK\bin\x86\x86\link.exe"

%CL86% /c /MD /I%DirectXInc% /I%APIInc% /I%DDKInc% /I%SDKInc% /I%CRTInc% /I%LuaPInc% /I%LuaWInc% /TcLuaXInput.c /Tc%LuaWInc%\luaW.c
if not errorlevel 1 (
%LINK86% /DLL /EXPORT:luaopen_LuaXInput /LIBPATH:%DirectXLib% /LIBPATH:%APILib% /LIBPATH:%SDKLib% /LIBPATH:%CRTLib% /LIBPATH:%LuaPLib% /DEFAULTLIB:lua51.lib /DEFAULTLIB:XInput.lib LuaXInput.obj luaW.obj /OUT:LuaXInput51.dll
del LuaXInput.obj
del luaW.obj
del LuaXInput51.exp
del LuaXInput51.lib
)

endlocal