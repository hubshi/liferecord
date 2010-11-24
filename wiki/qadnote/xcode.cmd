rem @echo off
set xrc=c:\qadguicli
set xcode=D:\xcode
del %xrc%\xrc\*.* /s/q/f/a
rd %xrc%\xrc /s/q
mkdir %xrc%\xrc

set src=D:\Trunk\sincere\sh005\xxworcmt
cd /d %src%
%xcode%\xcode -d %xrc%\xrc xxworc*.p
%xcode%\xcode -d %xrc%\xrc xx*300.p
%xcode%\xcode -d %xrc%\xrc *.i
copy /y lbl.cim + msg.cim %xrc%\xrc\lbl.cim

