rem @echo off
set fold="%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%%time:~9,2%"
set vfile=%1
rem set vfile=D:\ss\trunk\sinpo\xxarrp01
set xrc=c:\qadguicli\xrc
rem set xrc=\\192.168.0.92\admin\cim\xrc
set src=C:\qadguicli\garbage\src
set cim=C:\qadguicli\garbage\cim
set gui=c:\qadguicli\garbage\gui
set wkdir=c:\qadguicli
set xcode=C:\qadguicli\proedit
cd /d c:\qadtuicli
mkdir C:\qadguicli\garbage\%fold%\
move %src%\* c:\qadguicli\garbage\%fold%\
move %xrc%\* c:\qadguicli\garbage\%fold%\
move %wkdir%\gui\* c:\qadguicli\garbage\%fold%\

rem del %src%\*.* /s/q/f/a
rem del %xrc%\*.* /s/q/f/a
rem del %cim%\*.* /s/q/f/a

mkdir %src%
mkdir %xrc%

rem copy src to srcdir

rem All program

if %vfile:~-2,1%==. (copy /y %vfile% %src%\) else (if %vfile:~-3,1%==. (copy /y %vfile% %src%\) else (if %vfile:~-4,1%==. (copy /y %vfile% %src%\) else (copy /y %vfile%\*.* %src%\)))

rem Xcode.
cd /d %src%
%xcode%\xcode -d %xrc% *.p
%xcode%\xcode -d %xrc% *.w
%xcode%\xcode -d %xrc% *.i
%xcode%\xcode -d %xrc% *.v
%xcode%\xcode -d %xrc% *.t
rem if %2=="AA" goto exitme
rem goto exitme
rem convexitme
mkdir %gui%
echo srcSearchPath=%src%> %wkdir%\convert.tmp
echo targetDir=%wkdir%\gui>> %wkdir%\convert.tmp
echo workDir=%wkdir%>> %wkdir%\convert.tmp
%xcode%\convx64.exe -r -ini=%wkdir%\convert.tmp %src%\*.p>%wkdir%\convert.log
%xcode%\convx64.exe -r -ini=%wkdir%\convert.tmp %src%\*.i>>%wkdir%\convert.log
rem del %wkdir%\convert.tmp /s/q/f/a

:exitme
