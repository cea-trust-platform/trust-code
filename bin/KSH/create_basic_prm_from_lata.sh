#!/bin/bash

echo "Usage: $0 file"

# Existence of .lata file or .son file?
lata=$1.lata
son=$1.son
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
mkdir -p src
cd src
prm=$1.prm
data=$1.data
dtev=$1.dt_ev
 
# Generation of prm file
# ----------------------
dim=`grep -i dimension ../$data | cut -d " " -f 2`
echo -e "Parameters {
	Title \"$1 $dim"D" calculation\"
	Author \"`whoami`\"
	VersionTrio_U $TRUST_VERSION
 	TestCase . $data
}\n" > $prm
if [ -f ../$dtev ]
then
   echo -e "Chapter {
	Title \"Convergence\" " >> $prm
   residu=`head -n1 ../$dtev | cut -d " " -f 5 | sed -r "s/\t//ig"`
   echo -e "\n\tfigure { \n\t\ttitle \"Evolution of residue\"\n\t\tinclude_description_curves 0\n\t\tlabelX \"TIME\"\n\t\tlabelY \"$residu\"\n\t\tlogX\n\t\tlogY" >> $prm
   echo -e "\t\tcurve {\n\t\t\tfile ../$dtev\n\t\t\tcolumns (\$1) (\$4)\n\t\t\tlegend \"column 4\"\n\t\t\tstyle linespoints\n\t\t}\n\t}\n}\n" >> $prm
fi

# Paragraph with visu block
if [ -f ../$lata ]
then
   echo -e "Chapter {
	Title \"Visualization via VisIt\" " >> $prm
   lata2dx ../$lata writeprm 2>&1 | grep -v "] " | grep -v _boundaries | awk '{print "\n\tvisu {\n\t\ttitle \""$4" "$5"\"\n\t\t"$0"\n\t}"}'>> $prm
   echo -e "}\n" >> $prm
   sed -i "1,$ s?scalar?pseudocolor?g" $prm
   sed -i "1,$ s?title \" \"?title \"MESH\"?g" $prm
fi

# Paragraph with curve block
if [ -f ../$son ]
then
   nbson=`awk '{print $NR}' ../$son`
   echo -e "Chapter {
	Title \"Visualization via Gnuplot\" " >> $prm
   cat ../$son | grep -v .plan | while read file
   do 
      field=`grep Champ ../$file | cut -d " " -f 3`
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
      line=`head -n5 ../$file | tail -n1`
      nbcol=`echo $line | wc -w`
      nbcol=`expr $nbcol - 1`
      for ((j=1 ; $nbscal -$j + 1 ; j++))
      do
        echo -e $file | sed "s/$1_//g" | sed "s/\.son//g" | awk '{print "\n\tfigure { \n\t\ttitle \""$0"\""}' >> $prm
	echo -e "\t\tinclude_description_curves 0\n\t\tlabelX \"TIME\"\n\t\tlabelY \"${lab[$j]}"$field"\"" >> $prm
	k=`expr $j + 1`
        for ((i=$k ; $nbcol - $i + $k ; i=i+$nbscal))
        do
           echo -e $file | awk '{print "\t\tcurve {\n\t\t\tfile ../"$0""}' >> $prm
           echo -e "\t\t\tcolumns (\$1) ($"$i")\n\t\t\tlegend \"column $i\"\n\t\t\tstyle linespoints\n\t\t}" >> $prm
        done
        echo -e "\t}" >> $prm
      done
   done
   echo -e "}\n" >> $prm
fi

echo "-> Generate prm file... OK"
echo "$prm created in src directory."
echo "=========================="

# Creation of pdf file
# --------------------
Run_fiche -not_run -o $1.pdf
echo "=========================="
if [ -f ../build/$1.pdf ]
then
  echo "-> Generate pdf file...OK"
  echo "$1.pdf created in build directory."
else
  echo "-> Generate pdf file...KO"
fi
echo "=========================="

