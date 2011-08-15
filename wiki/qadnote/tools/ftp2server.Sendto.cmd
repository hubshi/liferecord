rem 把这个档案加入右键菜单可以方便上传到ftpserver.
rem "注：vfile=含目录的文件名,目录名,%1(send to)"

rem showa set vlcd=c:\qadguicli\xrc
rem showa set vcd=/var/tmp/xrc
rem showa set vhostip=192.168.0.92
rem showa set vusr=admin
rem showa set vpasswd=plan2011
rem showa set vfile=c:\qadguicli\xrc
rem showa set vworkdir=c:\qadguicli

rem shinpo ftp server.
set vlcd=c:\qadguicli\xrc
set vcd=/var/tmp/xrc
set vhostip=222.85.150.27
set vusr=mfg
set vpasswd=mfgmfg
set vfile=c:\qadguicli\xrc
set vworkdir=c:\qadguicli

cd /d %vlcd%
echo open %vhostip%> %vworkdir%\~ftp.txt 
echo %vusr%>> %vworkdir%\~ftp.txt
echo %vpasswd%>> %vworkdir%\~ftp.txt
echo cd %vcd%>> %vworkdir%\~ftp.txt
echo bin>> %vworkdir%\~ftp.txt
rem echo prompt off>> %vworkdir%\~ftp.txt
if %vfile:~-2,1%==. (echo put %vfile% >> %vworkdir%\~ftp.txt) else (if %vfile:~-3,1%==. (echo put %vfile% >> %vworkdir%\~ftp.txt) else (if %vfile:~-4,1%==. (echo put %vfile% >> %vworkdir%\~ftp.txt) else (echo mput %vfile%\*.*>> %vworkdir%\~ftp.txt)))
echo bye>> %vworkdir%\~ftp.txt

ftp -i -s:%vworkdir%\~ftp.txt
del /s/q/f/a %vworkdir%\~ftp.txt
