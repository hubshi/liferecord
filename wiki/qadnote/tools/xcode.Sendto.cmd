rem @echo off
rem 说明：vFile要加密的路径或文件可以是%1,%2如果为空则不转换为GUI程序其他任何值都要转换
set vfile=%1
set xrc=c:\qadguicli\xrc
set src=C:\qadguicli\garbage\src
set cim=C:\qadguicli\garbage\cim
set gui=c:\qadguicli\garbage\gui
set wkdir=c:\qadguicli
set xcode=C:\qadguicli\proedit

del %src%\*.* /s/q/f/a
del %xrc%\*.* /s/q/f/a
del %cim%\*.* /s/q/f/a

mkdir %src%
mkdir %xrc%
mkdir %cim%

rem copy src to srcdir

rem All program

if %vfile:~-2,1%==. (copy /y %vfile% %src%\) else (if %vfile:~-3,1%==. (copy /y %vfile% %src%\) else (if %vfile:~-4,1%==. (copy /y %vfile% %src%\) else (copy /y %vfile%\*.* %src%\)))

rem Xcode.
cd /d %src%
copy %src%\*.cim %xrc%\p.cim
%xcode%\xcode -d %xrc% *.p
%xcode%\xcode -d %xrc% *.i
%xcode%\xcode -d %xrc% *.v
%xcode%\xcode -d %xrc% *.t

if %2=="" goto exitme

rem convert if %2<>"".

mkdir %gui%
echo srcSearchPath=%src%> %wkdir%\convert.tmp
echo targetDir=%wkdir%\gui>> %wkdir%\convert.tmp
echo workDir=%wkdir%>> %wkdir%\convert.tmp
%xcode%\convert.exe -r -ini=%wkdir%\convert.tmp %src%\xx*.p>%wkdir%\convert.log
%xcode%\convert.exe -r -ini=%wkdir%\convert.tmp %src%\xx*.i>>%wkdir%\convert.log
rem del %wkdir%\convert.tmp /s/q/f/a

:exitme
