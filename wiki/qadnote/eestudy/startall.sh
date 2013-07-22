#!/bin/sh
# file: startenv.tpl
# Script to start environment.
 
# tokens:
# &ENV = Environment name
/app/progress/10.2B/bin/proadsv -start
sleep 8
/app/progress/10.2B/bin/nsman -i NS1 -start
sleep 8
 
/app/2011.1ee/qdt/envs/prod/scripts/start.prod
/app/2011.1ee/qdt/envs/prod/scripts/startqadui_ASprod.ksh
/app/2011.1ee/qdt/envs/prod/scripts/startqadui_WSprod.ksh
/app/2011.1ee/qdt/envs/prod/scripts/startqadfinprod.ksh
 
/app/2011.1ee/qdt/envs/prod/scripts/startqadsi_ASprod.ksh
/app/2011.1ee/qdt/envs/prod/scripts/startqxosi_ASprod.ksh
/app/2011.1ee/qdt/envs/prod/scripts/startqxoui_ASprod.ksh

/app/tomcat/bin/startup.sh

#/app/progress/10.2B/bin/proserve /database/prod/mfgprod -L 8000  -B 1000  -c 350 -S 54004 -N TCP -m3 -Ma 4 -Mpb 30 -NL -R0 -ServerType SQL
 
