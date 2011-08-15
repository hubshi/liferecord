rem @echo off
set wkdir=c:\qadguicli
set xcode=C:\qadguicli\proedit
set xrc=c:\qadguicli\xrc
set src=C:\qadguicli\garbage\src
set cim=C:\qadguicli\garbage\cim
set gui=C:\qadguicli\garbage\gui

del %src%\*.* /s/q/f/a
del %xrc%\*.* /s/q/f/a
del %cim%\*.* /s/q/f/a
mkdir %src%
mkdir %xrc%
mkdir %cim%

call %wkdir%\cpfile.cmd

rem Xcode.
cd /d %src%
%xcode%\xcode -d %xrc% *.p
%xcode%\xcode -d %xrc% *.i

copy %cim%\*.cim %xrc%\p.cim

rem convert.
mkdir %gui%
if %1=="A" goto exitme
echo srcSearchPath=%src%> %xrc%\convert.tmp
echo targetDir=%xrc%\..\gui>> %xrc%\convert.tmp
echo workDir=%xrc%>> %xrc%\convert.tmp
%xcode%\convert.exe -r -ini=%xrc%\convert.tmp %src%\*.p>%xrc%\convert.log
%xcode%\convert.exe -r -ini=%xrc%\convert.tmp %src%\*.i>>%xrc%\convert.log
rem del %xrc%\convert.tmp /s/q/f/a

:exitme
