rem 吧这个档案加入右键菜单可以方便上传到ftpserver.
set xrc=C:\qadguicli
cd /d %xrc%\xrc
rem echo open 222.85.150.27> %xrc%\~ftp.txt
echo open 192.168.0.92> %xrc%\~ftp.txt
rem echo demo>> %xrc%\~ftp.txt
rem echo demo>> %xrc%\~ftp.txt
echo admin>> %xrc%\~ftp.txt
echo admin>> %xrc%\~ftp.txt
rem echo lcd %xrc%\xrc>> %xrc%\~ftp.txt
rem  echo cd /home/demo/xrc>> %xrc%\~ftp.txt
echo cd /var/tmp/xrc>> %xrc%\~ftp.txt
echo bin>> %xrc%\~ftp.txt
rem echo prompt off>> %xrc%\~ftp.txt
echo put %1>> %xrc%\~ftp.txt
echo put %2>> %xrc%\~ftp.txt
echo put %3>> %xrc%\~ftp.txt
echo put %4>> %xrc%\~ftp.txt
echo put %5>> %xrc%\~ftp.txt
echo put %6>> %xrc%\~ftp.txt
echo put %7>> %xrc%\~ftp.txt
echo put %8>> %xrc%\~ftp.txt
echo put %9>> %xrc%\~ftp.txt
echo put %10>> %xrc%\~ftp.txt
echo put %11>> %xrc%\~ftp.txt
echo put %12>> %xrc%\~ftp.txt
echo put %13>> %xrc%\~ftp.txt
echo put %14>> %xrc%\~ftp.txt
echo put %15>> %xrc%\~ftp.txt
echo put %16>> %xrc%\~ftp.txt
echo put %17>> %xrc%\~ftp.txt
echo put %18>> %xrc%\~ftp.txt
echo put %19>> %xrc%\~ftp.txt
echo bye>> %xrc%\~ftp.txt

ftp -i -s:%xrc%\~ftp.txt
del /s/q/f/a %xrc%\~ftp.txt
pause
