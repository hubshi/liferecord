#!/bin/sh
# file: stopenv.tpl
# Script to stop environment.
 
# tokens:
# &ENV = Environment name
/app/tomcat/bin/shutdown.sh
 
/app/2011.1ee/qdt/envs/prod/scripts/stopqadfinprod.ksh
 
/app/2011.1ee/qdt/envs/prod/scripts/stopqadui_WSprod.ksh
/app/2011.1ee/qdt/envs/prod/scripts/stopqadui_ASprod.ksh
 
/app/2011.1ee/qdt/envs/prod/scripts/stopqadsi_ASprod.ksh
/app/2011.1ee/qdt/envs/prod/scripts/stopqxosi_ASprod.ksh
/app/2011.1ee/qdt/envs/prod/scripts/stopqxoui_ASprod.ksh
 
/app/2011.1ee/qdt/envs/prod/scripts/stop.prod

/app/progress/10.2B/bin/nsman -i NS1 -stop

/app/progress/10.2B/bin/proadsv -stop
  
