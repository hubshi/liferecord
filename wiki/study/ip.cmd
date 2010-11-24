echo off
rem 导出IP配置
rem netsh -c interface ip dump > c:\tmpip.txt
rem 写入临时文件
rem 自c:\~ip.txt导入IP
rem netsh -f c:\~ip.txt

if /i "%1" == "" goto office
if /i "%1" == "/o" goto office
if /i "%1" == "/?" goto helpme
if /i "%1" == "/h" goto helpme
if /i "%1" == "/x" goto roger
if /i "%1" == "/r" goto roger
if /i "%1" == "/c" goto clear

:helpme
echo ip [/h] [/?] [/o] [/x] [/c]
echo set ip address
echo [/h]=[/?] show help page
echo [""]=[o] set to office
echo [/x] set to roger's home
echo [/c] set to dhcp auto format

goto done

:office
echo pushd interface ip > c:\~ip.txt
echo # "WIRELESS" 的接口 IP  配置 >> c:\~ip.txt
echo set address name="WIRELESS" source=dhcp >> c:\~ip.txt
echo set dns name="WIRELESS" source=static addr=211.155.17.188 register=PRIMARY >> c:\~ip.txt
echo add dns name="WIRELESS" addr=202.96.128.68 index=2 >> c:\~ip.txt
echo set wins name="WIRELESS" source=dhcp >> c:\~ip.txt
echo # "LAN" 的接口 IP  配置 >> c:\~ip.txt
echo set address name="LAN" source=dhcp >> c:\~ip.txt
echo set dns name="LAN" source=static addr=211.155.17.188 register=PRIMARY >> c:\~ip.txt
echo add dns name="LAN" addr=202.96.128.68 index=2 >> c:\~ip.txt
echo set wins name="LAN" source=dhcp >> c:\~ip.txt
echo popd >> c:\~ip.txt
goto setip

:roger
echo pushd interface ip > c:\~ip.txt
echo # roger's home >> c:\~ip.txt
echo set address name="WIRELESS" source=static addr=192.168.0.52 mask=255.255.255.0 >> c:\~ip.txt
echo set address name="WIRELESS" gateway=192.168.0.1 gwmetric=0 >> c:\~ip.txt
echo set dns name="WIRELESS" source=static addr=192.168.0.1 register=PRIMARY >> c:\~ip.txt
echo set wins name="WIRELESS" source=static addr=none >> c:\~ip.txt
echo popd >> c:\~ip.txt
goto setip

:clear
echo pushd interface ip > c:\~ip.txt
echo # "WIRELESS" 的接口 IP  配置 >> c:\~ip.txt
echo set address name="WIRELESS" source=dhcp >> c:\~ip.txt
echo set dns name="WIRELESS" source=dhcp register=PRIMARY >> c:\~ip.txt
echo set wins name="WIRELESS" source=dhcp >> c:\~ip.txt
echo # "LAN" 的接口 IP  配置 >> c:\~ip.txt
echo set address name="LAN" source=dhcp >> c:\~ip.txt
echo set dns name="LAN" source=dhcp register=PRIMARY >> c:\~ip.txt
echo set wins name="LAN" source=dhcp >> c:\~ip.txt
echo popd >> c:\~ip.txt

:setip
echo off
rem 自c:\~ip.txt导入IP
netsh -f c:\~ip.txt
ipconfig /renew LAN
ipconfig /renew WIRELESS
del /s/q/f/a c:\~ip.txt

:done
echo on
