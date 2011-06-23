rem @echo off
set tpe=%1
set xrc=c:\qadguicli
set xcode=C:\qadguicli\proedit
set src=C:\qadguicli\garbage\src
set cim=C:\qadguicli\garbage\cim
del %src%\*.* /s/q/f/a
del %xrc%\xrc\*.* /s/q/f/a
rem rd %xrc%\xrc
mkdir %xrc%\garbage\src
mkdir /d %xrc%\xrc

rem copy src to srcdir
mkdir %src%
rem All program

if %tpe:~-2,1%==. (copy /y %1 %src%\) else (if %tpe:~-3,1%==. (copy /y %1 %src%\) else (if %tpe:~-4,1%==. (copy /y %1 %src%\) else (copy /y %1\*.* %src%\)))

rem Xcode.
cd /d %src%
%xcode%\xcode -d %xrc%\xrc xx*.p
%xcode%\xcode -d %xrc%\xrc xx*.i
%xcode%\xcode -d %xrc%\xrc xx*.v

goto exitme

rem convert.
mkdir %xrc%\gui
if %1=="A" goto exitme
echo srcSearchPath=%src%> %xrc%\convert.tmp
echo targetDir=%xrc%\gui>> %xrc%\convert.tmp
echo workDir=%xrc%>> %xrc%\convert.tmp
%xcode%\convert.exe -r -ini=%xrc%\convert.tmp %src%\xx*.p>%xrc%\convert.log
%xcode%\convert.exe -r -ini=%xrc%\convert.tmp %src%\xx*.i>>%xrc%\convert.log
rem del %xrc%\convert.tmp /s/q/f/a

:exitme
