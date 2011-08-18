set fold="%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%%time:~9,2%"
cd /d c:\qadtuicli
mkdir .\garbage\%fold%\
move *.log .\garbage\%fold%\
move *. .\garbage\%fold%\
move *.p .\garbage\%fold%\
move *.i .\garbage\%fold%\
move protrace.* .\garbage\%fold%\
move *.rpt .\garbage\%fold%\
move *.prn .\garbage\%fold%\
move *.tmp .\garbage\%fold%\
move *.bak .\garbage\%fold%\
move *.xls? .\garbage\%fold%\
move *.csv .\garbage\%fold%\
move *.t .\garbage\%fold%\
move *.txt .\garbage\%fold%\
move *.cim .\garbage\%fold%\
move .\garbage\src\*.* .\garbage\%fold%\
move xrc\*.* .\garbage\%fold%\
move *. .\garbage\%fold%\
