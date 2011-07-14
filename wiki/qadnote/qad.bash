#!/bin/sh
function ShowMenu(){
	clear
	echo "            MFG/PRO STARTUP MENU          "
	echo "==================================================================="
	echo " 1 - Run	 QAD2008 Production Client "
	echo " 2 - Run	 QAD2008 Demonstration Client "
	echo " 3 - Run	 QAD2008 Training Client "
	echo " 4 - ShutDown The System "
	echo " 0 - Exit "
	echo "==================================================================="
	echo ""
	echo -n " Enter your choice : "
}
while true ; do
	ShowMenu
	trap   " ShowMenu "   1   2   3   9   15
	read sel

	case $sel in 
	    [1] )
		 while true ; do
			/app/mfgpro/qad2008/client.Production
			echo ""
			echo " Procedure completed ... "
			sleep 3
			break
		done ;;
	    [2] )
		 while true ; do
			/app/mfgpro/qad2008/client.Demonstration
			echo " "
			echo " Procedure completed ... "
			sleep 3
			break
		done ;;	  
	    [3] ) 
		while true ; do
			/app/mfgpro/qad2008/client.Training
			echo " "
			echo " Procedure completed ... "
			sleep 3
			break
		done ;;
	    [4] )
		 while true ; do
			echo " System will be shut down ..."
			echo " Yes or No ? "
			echo " If you choose yes , you need to enter the current user's password again "
			echo -n " Entey your choice [ y / n ] :"
			read key
			if [ "$key" = "y" ]; then
				sudo /app/scripts/shutdown
				echo " System is going down ..."
				echo " Now , you can close the window safely . "
				sleep 10
				break
			else 
				break
			fi
		done ;;
	    [0] )exit ;;
	esac 
done
