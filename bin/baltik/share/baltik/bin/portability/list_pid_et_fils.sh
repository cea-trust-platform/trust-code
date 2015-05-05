#!/bin/bash
USAGE="Usage: $0 pid"
NOTANINTEGER="$1 is not an integer"
NOTAPROCESS="$1: no such process"
NOCHILDREN="$1 has no children"

if [ $# = 0 ]
then
echo $USAGE
exit
fi

MYPID=$1
NUMBER=$1
ISNUMBER=0
ISPID=0
ISPARENT=0

PS=/bin/ps

isNumber ()
{
if [ $MYPID -gt 0 ]; then
ISNUMBER=1
else
echo $NOTANINTEGER
exit -1
fi
}

isProcess ()
{
while read pid
do
if [ $pid -eq $MYPID ]; then
ISPID=1
break;
fi
done

if [ $ISPID -eq 0 ]; then
exit -1
fi
}

hasChildren ()
{
while read ppid
do
if [ $ppid -eq $MYPID ]; then
ISPARENT=1
break;
fi
done

if [ $ISPARENT -eq 0 ]; then
exit -1
fi
}

printChildren ()
{
#echo "UID\tPID\tPPID\tTTY\tTIME\t\tCOMMAND"
while read owner process parent tty time command
do
if [ "$command" ]
then
if [ $parent -eq $MYPID ]; then
#echo "$owner\t$process\t$parent\t$tty\t$time\t$command"
echo $process
fi
fi
done
}

formatPIDS ()

{
$PS -eo "%U %p %P %y %x %a" | awk 'NR>2 && $1!="" {print}'
}

printPIDS ()

{
$PS -eo "%p" | grep -v PID
}

printPPIDS ()

{
$PS -eo "%P" | grep -v PID
}

if [ ! isNumber ]; then
echo $NOTANUMBER
exit
fi
proc=`printPIDS | isProcess`
if [ ! proc ]; then
echo $NOTAPROCESS
exit
fi
child=`printPPIDS | hasChildren`
if [ ! child ]; then
echo $NOCHILDREN
exit
fi
listepid=$MYPID

liste=`formatPIDS | printChildren `
while [ "$liste" != "" ]
do
  ln=""
  for pid in $liste
  do
    MYPID=$pid
    listepid=""$listepid" "$pid
    # echo $listepid
    ln=$ln" "`formatPIDS | printChildren `
    done
  liste=$ln
done
echo $listepid
