rem @echo off
set xrc=c:\qadguicli
set xcode=D:\xcode
del %xrc%\xrc\*.* /s/q/f/a
rd %xrc%\xrc
mkdir %xrc%\xrc

rem Xcode.
set src=D:\ss2k10\trunk\common
cd /d %src%
%xcode%\xcode -d %xrc%\xrc *.p
%xcode%\xcode -d %xrc%\xrc *.i

rem convert.
mkdir %xrc%\gui
echo srcSearchPath=%src%> %xrc%\convert.tmp
echo targetDir=%xrc%\gui>> %xrc%\convert.tmp
echo workDir=%xrc%>> %xrc%\convert.tmp
%xcode%\convert.exe -r -ini=%xrc%\convert.tmp %src%\xx*.p>outlog.log
del %xrc%\convert.tmp /s/q/f/a