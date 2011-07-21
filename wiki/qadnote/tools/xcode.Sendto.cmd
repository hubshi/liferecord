rem @echo off
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

goto exitme

rem convert.
mkdir %gui%
if %1=="A" goto exitme
echo srcSearchPath=%src%> %xrc%\convert.tmp
echo targetDir=%xrc%\gui>> %xrc%\convert.tmp
echo workDir=%xrc%>> %xrc%\convert.tmp
%xcode%\convert.exe -r -ini=%xrc%\convert.tmp %src%\xx*.p>%xrc%\convert.log
%xcode%\convert.exe -r -ini=%xrc%\convert.tmp %src%\xx*.i>>%xrc%\convert.log
rem del %xrc%\convert.tmp /s/q/f/a

:exitme
