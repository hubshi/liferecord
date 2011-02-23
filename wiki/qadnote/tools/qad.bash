#!/bin/sh
function ShowMenu(){
	clear
	echo "            MFG/PRO STARTUP MENU          "
	echo "==================================================================="
	echo " 1 - StartUp	MFG/PRO EB21 SP6 Demonstration Server "
	echo " 2 - ShutDown	MFG/PRO EB21 SP6 Demonstration Server "
	echo " 3 - Run	MFG/PRO EB21 SP6 Demonstration Client "
	echo " 0 - ShutDown The System "
	echo "==================================================================="
	echo ""
	echo -n " Enter your choice : "
}
while true ; do
	ShowMenu
	read sel

	case $sel in 
	    [1] )
		 while true ; do
			/app/scripts/startDemo
			echo " "
			echo " Procedure completed ... "
			echo " "
			echo  -n " Press r to return to the main menu: "
			read key
			if [ "$key" = "r" ]; then
				break
			fi
			echo $key
		done ;;
	    [2] )
		 while true ; do
			/app/scripts/stopDemo
			echo " "
			echo " Procedure completed ... "
			echo " "
			echo  -n " Press r to return to the main menu: "
			read key
			if [ "$key" = "r" ]; then
				break
			fi
			echo $key
		done ;;	  
	    [3] ) 
		 while true ; do
			/app/mfgpro/eb21sp6/client.Demonstration
			echo " "
			echo " Procedure completed ... "
			echo " "
			echo  -n " Press r to return to the main menu: "
			read key
			if [ "$key" = "r" ]; then
				break
			fi
			echo $key
		done ;;
	    [0] )
		 while true ; do
			echo "System will be shut down, Yes or No ?"
			read key
			if [ "$key" = "y" ]; then
				poweroff
				break
			else 
				break
			fi
			echo $key
		done ;;
	esac 
done
