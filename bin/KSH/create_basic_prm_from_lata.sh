#!/bin/bash

echo "Usage: $0 [ -gen_pdf ] file"
genonly=1
[ "$1" = "-gen_pdf" ] && genonly=0 && shift
# Existence of .lata file or .son file?
lata=$1.lata
son=$1.sons
if [ "$1" = "" ] || [ ! -f $lata ]
then
   if [ "$1" = "" ] || [ ! -f $son ]
   then
      echo "=========================="
      echo "$lata not exist."
      echo "$son not exist."
      #exit -1
   else
      echo "=========================="
      echo "$son exist."
   fi
else
   echo "=========================="
   echo "$lata exist."
   if [ -f $son ]
   then
      echo "$son exist."
   fi
fi
prm=$1.prm
data=$1.data
dtev=$1.dt_ev
if [ "$genonly" = "1" ]
    then 
    REP=""
else
    REP="../"
    mkdir -p src
    cd src
fi
case=${REP}$1

# Generation of prm file
# ----------------------
dim=`grep -i dimension ${REP}$data | cut -d " " -f 2`
echo $ECHO_OPTS "Parameters {
	Title \"$1 $dim"D" calculation\"
	Author \"`whoami`\"
	VersionTrio_U $TRUST_VERSION
 	TestCase . $data
}\n" > $prm
if [ -f ${REP}$dtev ]
then
   echo $ECHO_OPTS "Chapter {
	Title \"Convergence\" " >> $prm
   residu=`head -n1 ${REP}$dtev | cut -d " " -f 5 | sed -r "s/\t//ig"`
   echo $ECHO_OPTS "\n\tfigure { \n\t\ttitle \"Evolution of residue\"\n\t\tinclude_description_curves 0\n\t\tlabelX \"TIME\"\n\t\tlabelY \"$residu\"\n\t\tlogX\n\t\tlogY" >> $prm
   echo $ECHO_OPTS "\t\tcurve {\n\t\t\tfile ${REP}$dtev\n\t\t\tcolumns (\$1) (\$4)" >> $prm
   #echo $ECHO_OPTS "\t\t\tlegend \"column 4\"" >> $prm
   echo $ECHO_OPTS "\t\t\tstyle linespoints\n\t\t}\n\t}\n}\n" >> $prm
fi

# Paragraph with visu block
if [ -f ${REP}$lata ]
then
   echo $ECHO_OPTS "Chapter {
	Title \"Visualization via VisIt\" " >> $prm
   lata2dx ${REP}$lata writeprm 2>&1 | grep -v "] " | grep -v _boundaries | awk '{print "\n\tvisu {\n\t\twidth 8cm\n\t\ttitle \""$4" "$5"\"\n\t\t"$0"\n\t}"}'>> $prm
   echo $ECHO_OPTS "}\n" >> $prm
   sed -i "1,$ s?scalar?pseudocolor?g" $prm
   sed -i "1,$ s?title \" \"?title \"MESH\"?g" $prm
fi

# Paragraph with curve block
if [ -f ${REP}$son ]
then
   width="Width 12cm" # Essayer de mettre 2 courbes par page
   nbson=`awk '{print $NR}' ${REP}$son`
   echo $ECHO_OPTS "Chapter {
	Title \"Visualization via Gnuplot\" " >> $prm
   cat ${REP}$son | grep -v .plan | while read file
   do 
      probe=`echo $ECHO_OPTS $file | sed "s/$1_//g" | sed "s/\.son//g" | awk '{print ""$0""}'`
      field=`grep Champ ${REP}$file | cut -d " " -f 3`
      type=`grep Type ${REP}$file | cut -d " " -f 3`
      if [[ "$type" == "SEGMENT" ]]
      then
          echo $ECHO_OPTS "\n\tfigure { \n\t\ttitle \"$field profile\"">> $prm
	  echo $ECHO_OPTS "\n\t\t$width" >> $prm
          echo $ECHO_OPTS "\t\tinclude_description_curves 0\n\t\tlabelX \"AXIS\"\n\t\tlabelY \"$field\"" >> $prm
          echo $ECHO_OPTS "\t\tcurve {\n\t\t\tsegment $case $probe" >> $prm
	  echo $ECHO_OPTS "\t\t\tlegende \"$probe\"" >> $prm
          echo $ECHO_OPTS "\t\t\tstyle linespoints\n\t\t}" >> $prm
          echo $ECHO_OPTS "\t}" >> $prm
      #elif [[ "$type" == "POINTS" ]]
      #then
      #    echo $ECHO_OPTS "\n\tfigure { \n\t\ttitle \"$field profile\"">> $prm
      #    echo $ECHO_OPTS "\n\t\t$width" >> $prm
      #    echo $ECHO_OPTS "\t\tinclude_description_curves 0\n\t\tlabelX \"AXIS\"\n\t\tlabelY \"$field\"" >> $prm
      #    echo $ECHO_OPTS "\t\tcurve {\n\t\t\tpoints $case $probe" >> $prm
      #    echo $ECHO_OPTS "\t\t\tlegende \"$probe\"" >> $prm
      #    echo $ECHO_OPTS "\t\t\tstyle linespoints\n\t\t}" >> $prm
      #    echo $ECHO_OPTS "\t}" >> $prm
      else
        nbscal=1
        lab[1]=""
        lab[2]=""
        lab[3]=""
        if [[ "$field" == "VITESSE" ]] || [[ "$field" == "VELOCITY" ]]
        then
          nbscal=$dim
	  lab[1]="X_"
	  lab[2]="Y_"
	  lab[3]="Z_"
        fi
        line=`head -n5 ${REP}$file | tail -n1`
        nbcol=`echo $line | wc -w`
        nbcol=`expr $nbcol - 1`
        for ((j=1 ; $nbscal - $j + 1 ; j++))
        do
          echo $ECHO_OPTS "\n\tfigure { \n\t\ttitle \"$field evolution\"">> $prm
	  echo $ECHO_OPTS "\n\t\t$width" >> $prm
 	  echo $ECHO_OPTS "\t\tinclude_description_curves 0\n\t\tlabelX \"TIME\"\n\t\tlabelY \"${lab[$j]}"$field"\"" >> $prm
	  k=`expr $j + 1`
          for ((i=$k ; $nbcol - $i + $k ; i=$i+$nbscal))
          do
            echo $ECHO_OPTS $file | awk -v REP=${REP} '{print "\t\tcurve {\n\t\t\tfile "REP$0""}' >> $prm
            echo $ECHO_OPTS "\t\t\tcolumns (\$1) ($"$i")" >> $prm
	    echo $ECHO_OPTS "\t\t\tlegende \"$probe $i\"" >> $prm
            #echo $ECHO_OPTS "\t\t\tlegend \"column $i\"" >> $prm
            echo $ECHO_OPTS "\t\t\tstyle linespoints\n\t\t}" >> $prm
          done
          echo $ECHO_OPTS "\t}" >> $prm
        done
      fi
   done
   cat ${REP}$son | grep .plan | while read file
   do 
      probe=`echo $ECHO_OPTS $file | sed "s/$1_//g" | sed "s/\.plan//g" | awk '{print ""$0""}'`
      field=`grep GRILLE ${REP}$file | cut -d " " -f 3`
      type=`grep GRILLE ${REP}$file | cut -d " " -f 2`
      echo $ECHO_OPTS "\n\tfigure { \n\t\ttitle \"$field 2D map\"">> $prm
      echo $ECHO_OPTS "\n\t\t$width" >> $prm
      echo $ECHO_OPTS "\t\tinclude_description_curves 0\n\t\tlabelX \"AXIS\"\n\t\tlabelY \"AXIS\"" >> $prm
      echo $ECHO_OPTS "\t\tcurve {\n\t\t\tplan $case $probe" >> $prm
      echo $ECHO_OPTS "\t\t\tlegende \"$probe\"" >> $prm
      echo $ECHO_OPTS "\t\t\tstyle points\n\t\t}" >> $prm
      echo $ECHO_OPTS "\t}" >> $prm
   done
   echo $ECHO_OPTS "}\n" >> $prm
fi
echo $ECHO_OPTS "Chapter {
\tTitle \"Computer performance\"
\ttableau_performance {
\t}
}" >> $prm

echo "-> Generate prm file... OK"
[ "$genonly" = "1" ] && exit

echo "$prm created in src directory."
echo "=========================="

# Creation of pdf file
# --------------------
Run_fiche -not_run -o $1.pdf -xpdf
echo "=========================="
if [ -f ${REP}build/$1.pdf ]
then
  echo "-> Generate pdf file${REP}.OK"
  echo "$1.pdf created in build directory."
else
  echo "-> Generate pdf file${REP}.KO"
fi
echo "=========================="
