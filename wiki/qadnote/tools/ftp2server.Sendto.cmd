rem 把这个档案加入右键菜单可以方便上传到ftpserver.
set xrc=C:\qadguicli
set tpe=%1
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
if %tpe:~-2,1%==. (echo put %1 >> %xrc%\~ftp.txt) else (if %tpe:~-3,1%==. (echo put %1 >> %xrc%\~ftp.txt) else (if %tpe:~-4,1%==. (echo put %1 >> %xrc%\~ftp.txt) else (echo mput %1\*.*>> %xrc%\~ftp.txt)))
echo bye>> %xrc%\~ftp.txt
ftp -i -s:%xrc%\~ftp.txt
del /s/q/f/a %xrc%\~ftp.txt
