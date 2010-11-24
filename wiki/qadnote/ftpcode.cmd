set xrc=C:\qadguicli
cd /d %xrc%\xrc
echo open 192.168.0.1> %xrc%\~ftp.txt
rem echo demo>> %xrc%\~ftp.txt
rem echo demo>> %xrc%\~ftp.txt
echo mfg>> %xrc%\~ftp.txt
echo mfgmfg>> %xrc%\~ftp.txt
echo lcd %xrc%\xrc>> %xrc%\~ftp.txt
rem  echo cd /home/demo/xrc>> %xrc%\~ftp.txt
echo cd /home/mfg/xrc>> %xrc%\~ftp.txt
echo bin>> %xrc%\~ftp.txt
echo prompt off>> %xrc%\~ftp.txt
echo mput *.*>> %xrc%\~ftp.txt
echo y>>  %xrc%\~ftp.txtt
echo bye>> %xrc%\~ftp.txt

ftp -i -s:%xrc%\~ftp.txt
del /s/q/f/a %xrc%\~ftp.txt