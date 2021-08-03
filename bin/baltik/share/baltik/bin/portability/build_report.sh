#!/bin/bash
project=$1
verbose=0
[ "$2" = "-v" ] && verbose=1
list_machine=`ls ${project}_%_*_%_time.log |sed "s/_%_time.log//; s/${project}_%_//"`
[ $verbose -eq 1 ] && echo machines ${list_machine}
list_info_res=`grep Info_global ${project}_%_*.log | awk '{print $2}' | sort -u `
list_info_base="date_debut date_fin Os model CC cible prepare configure make make_check make_install"

list_info=${list_info_base}

for mm in ${list_info_res}
do
  testu=0
  for m2 in ${list_info_base}
  do
    [ "$mm" = "$m2" ] && testu=1
  done
  [ $testu -eq 0 ] && list_info=$list_info" "$mm	
done

# set -xv 
echo $list_info

echo '<HTML>
<HEAD>
   <META NAME=ROBOTS CONTENT="NOINDEX, NOFOLLOW">
   <META NAME=ROBOTS CONTENT="NOARCHIVE">
<TITLE>'$project' Results compilation</TITLE>
</HEAD>
<BODY>
<CENTER><B><FONT SIZE=+1>Compilation de '$project le `date `'</FONT></B></CENTER>
<TABLE BORDER WIDTH=100% NOSAVE > 
<TR>
<TD>Machine</TD>' > nuit_${project}.html
for info in $list_info
do
echo '<TD>'$info'</TD>'
done >>nuit_${project}.html
echo '</TR>' >> nuit_${project}.html
nb_machines=0
nb_ok=0
nb_ok_cible=0
nb_cibles=0
for machine in $list_machine
  do
   let nb_machines=nb_machines+1
   tot_ok=0
   cible=0
	   file=`ls -tr  ${project}_%_${machine}_%_*.log| grep -vi time|tail -1`;
	   [ $verbose -eq 1 ] && echo last_file $machine ${file}
	   echo "<TR> <TD><A HREF=${file} >$machine</A></TD>" >>nuit_${project}.html
  for info in $list_info
    do
    
    for f in  `ls  ${project}_%_${machine}_%_*.log`
      do	
      res=`grep "Info_global $info " $f|sed "s/Info_global $info //"`
      file=$f
      [ "$res" != "" ] && break
    done
    
    [ "$res" = "OK" ] &&  [ ${info} = "make_install" ] && tot_ok=1 
    [ "$res" = "" ] && res="&nbsp;"
    color=""
    [ "$res" = "OK" ] && color=BGCOLOR=3D"#E47833" &&  res="<A HREF=$file> $res </A>"
    
    [ "$res" = "KO" ] && color=BGCOLOR="#C42111" && res="<A HREF=$file> $res </A>" 
    
 #   [ "`echo $res|awk '{print $1}`" = "file" ] && res="<A HREF=$file> $res </A>"
    echo "<TD $color>$res</TD>" >> nuit_${project}.html 
   [ "$info" = "cible" ] && cible=$res
  done
  if [ "$cible" = "1" ]
  then
   let nb_cibles=nb_cibles+1 
  [ $tot_ok -eq 1 ] && let nb_ok_cible=nb_ok_cible+1
  fi
  [ $tot_ok -eq 1 ] && let nb_ok=nb_ok+1
  echo "</TR>" >> nuit_${project}.html
done
echo "</TABLE>
<p>total_OK: $nb_ok / $nb_machines &nbsp;</p>
<p>cible_OK: $nb_ok_cible / $nb_cibles </p>
</BODY>
</HTML>" >> nuit_${project}.html

