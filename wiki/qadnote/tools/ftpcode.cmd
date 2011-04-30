rem call c:\qadguicli\xcode.cmd

set xrc=C:\qadguicli
cd /d %xrc%\xrc
rem echo open 222.85.150.27> %xrc%\~ftp.txt
echo open 192.168.0.92> %xrc%\~ftp.txt
rem echo demo>> %xrc%\~ftp.txt
rem echo demo>> %xrc%\~ftp.txt
echo gsa>> %xrc%\~ftp.txt
echo gsa>> %xrc%\~ftp.txt
echo lcd %xrc%\xrc>> %xrc%\~ftp.txt
rem  echo cd /home/demo/xrc>> %xrc%\~ftp.txt
echo cd xrc>> %xrc%\~ftp.txt
echo bin>> %xrc%\~ftp.txt
rem echo prompt off>> %xrc%\~ftp.txt
echo mput *.*>> %xrc%\~ftp.txt
echo bye>> %xrc%\~ftp.txt

ftp -i -s:%xrc%\~ftp.txt
del /s/q/f/a %xrc%\~ftp.txt