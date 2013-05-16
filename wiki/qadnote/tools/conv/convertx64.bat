set wkdir=C:\qadguicli\conv
echo > %wkdir%\convert.log
copy %1 %wkdir%\src
cd /d %wkdir%
for /r %%i in (.\src\*.p .\src\*.i) do %wkdir%\convx64.exe "%%i" >>%wkdir%\convert.log
del /f/s/q %wkdir%\src\*.* 
pause 