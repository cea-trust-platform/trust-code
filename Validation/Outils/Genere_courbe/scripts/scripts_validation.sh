#!/bin/bash 


echo "def Extract_Times [-revert] DIR [<output_dir>]"
Extract_Times()
{
    revert=""
    [ "$1" = "-revert" ] && revert="--reverse" && shift
    files_dir=$1 && shift
    output_dir=`pwd`
    [ "x$1" != "x" ] && output_dir=`realpath $1`

    org=`pwd`
    cd $files_dir
    ref=`pwd`
    archives=`ls *.tgz`
    archives_ok=`for arc in $archives ; do [ -f ${arc%.tgz}.pdf ] && echo ${arc}; done`
    echo $archives_ok

    mkdir -p $output_dir/build_prov
    cd $output_dir/build_prov
    stt=0
    for arc in $archives_ok
    do
      cas=${arc%.tgz}
      mkdir -p $cas
      cd $cas
      tar zxf $ref/$arc preserve/temps_total
      echo  `head -1 preserve/temps_total`  $cas
      t=`head -1 preserve/temps_total | awk '{print $1}'`
      stt=`echo $stt+$t | bc -l`
      cd $output_dir/build_prov 
    done    > $output_dir/build_prov/Times
#    echo $stt Total  >> $output_dir/build_prov/Times
    echo $stt Total 
    sort -n $revert $output_dir/build_prov/Times > $output_dir/Times
    cd ..
    rm -rf $output_dir/build_prov
    cd $org
}

echo def verif_BUILD
verif_BUILD()
{
    if [ "$Rapports_auto_root" = "" ]
        then
        Rapports_auto_root=$TRUST_ROOT
        export Rapports_auto_root
    fi
    for f in `ls BUILD`
      do
      [ ! -d BUILD/$f ]  && continue
      echo $f
      f2=`find  $Rapports_auto_root/Validation/Rapports_automatiques -follow  -name $f -type d | grep -v $f/src`
      diff -r BUILD/$f/src $f2/src
    done
}


echo def verif_archives
verif_archives()
{
    PB=0
    org=`pwd`

    if [ "$Rapports_auto_root" = "" ]
        then
        Rapports_auto_root=$TRUST_ROOT
        export Rapports_auto_root
    else
        echo "Using $Rapports_auto_root directory for validation instead of \$TRUST_ROOT"
    fi
    DIR=`dirname -- $0`
    DIR=`(cd $DIR;pwd)`
    cat /dev/null > nettoie
    cd archives
    for f in `ls *.pdf`
      do
      if [ ! -f `basename $f .pdf`.tgz ] 
          then
          echo "$f pb ? (no `basename $f .pdf`.tgz  file)"
          PB=2
      else
          [ -d ../BUILD/`basename $f .pdf`/build ] && echo "rm -rf BUILD/`basename $f .pdf`/build" >> ../nettoie
      fi
      [ "`find $Rapports_auto_root/Validation/Rapports_automatiques/ -follow -type d -name  $(basename $f .pdf)`" = "" ] && [ "`find $Rapports_auto_root/Validation/Rapports_automatiques/ -follow -type l -name  $(basename $f .pdf)`" = "" ] &&echo $f not in Validation/Rapports_automatiques ?  && PB=3
    done

    for file in `find  $Rapports_auto_root/Validation/Rapports_automatiques/$1 -follow -name '*'.prm`
      do
      f=`dirname $file`
      ze_dir=`dirname $f`
      f=`basename $ze_dir`
      if [ -f $ze_dir/skip_prm ] || [ -f $ze_dir/src/skip_prm ]
    then
    if [ -f $f.pdf ]
        then
        echo $f is skipped. Deleting corresponding PDF.
        rm $f.pdf  
    fi 
      else
    if [ ! -f $f.pdf ]  
        then
        echo $f not run ? 
        PB=1
    fi
      fi
    done
    # echo `wc -l ../nettoie` repertoires a effacer
    chmod +x ../nettoie
    cd $org
    return $PB
}


echo def compare_pdf_batch 
compare_pdf_batch()
{
    batch_mode=""
    [ "$1" = "-batch" ] && batch_mode="-batch" && shift
    new=$1
    ref=$2

    base_ref=`basename $ref .pdf`
    base_new=`basename $new .pdf` 

    ppm_dir_ref1="ppms/ppm_ref_"
    ppm_dir_new1="ppms/ppm_new_"
    ppm_dir_diff1="ppms/ppm_diff_"
 
    ppm_dir_ref="ppms/ppm_ref_${base_ref}"
    ppm_dir_new="ppms/ppm_new_${base_new}"
    ppm_dir_diff="ppms/ppm_diff_${base_ref}"

    rm -rf $ppm_dir_new $ppm_dir_ref $ppm_dir_diff
    mkdir -p $ppm_dir_new
    mkdir -p $ppm_dir_ref
    mkdir -p $ppm_dir_diff
    pdftoppm -r 100 ${ref}  $ppm_dir_ref/${base_ref}_oo
    pdftoppm -r 100 ${new}  $ppm_dir_new/${base_new}_oo
    ret_code=0

    for file_ref in `ls $ppm_dir_ref/${base_ref}_oo*ppm`
    do
      file_new=`echo $file_ref | sed "s?$ppm_dir_ref1?$ppm_dir_new1?; s?$base_ref?$base_new?g"`
      diff $file_ref $file_new 1> /dev/null 2>&1
      if [ $? == 0 ]
      then
          echo Compare $file_ref OK
          rm -f $file_ref $file_new
      else
          ret_code=1
          difffile=`echo $file_ref | sed "s?$ppm_dir_ref?$ppm_dir_diff?"`
          echo "Compare $file_ref K0: see $difffile"
          compare $file_ref $file_new $difffile

          if [ "$batch_mode" != "-batch" ]
          then
              montfile=`echo $file_ref | sed "s?_oo?_mont?"`
              montage -geometry 630x900  -tile 3x1  $file_ref  $difffile  $file_new $montfile
              echo display $montfile >> avoir
          fi
      fi
    done
    #
    # Clean up and convert to PNG
    #
    if [ "$ret_code" = "0" ]; then
      rm -rf $ppm_dir_new $ppm_dir_ref $ppm_dir_diff
    else
      for f in `ls $ppm_dir_new/*.ppm`; do convert $f $f.png; done
      for f in `ls $ppm_dir_ref/*.ppm`; do convert $f $f.png; done
      for f in `ls $ppm_dir_diff/*.ppm`; do convert $f $f.png; done      
    fi

    return $ret_code
}

echo def compare_pdf
compare_pdf()
{
    rm -f avoir
    compare_pdf_batch $*
    [ -f avoir ] && sh ./avoir 
}


#
# For a single PRM, compare old and new report.
# If -batch is specified, do not stop to ask whether differences are ok or not and just produce difference files.
#
echo "def comp_fiche [ -batch ]"
comp_fiche()
{
    batch_mode=""
    [ "$1" = "-batch" ] && batch_mode="-batch" && shift
    fiche=$1
    echo "Comparing $fiche"
    [ "$fiche" = "" ] && exit -1

    mkdir -p new_rap/OK
    mkdir -p new_rap/KO

    [ ! -f old_rap/$fiche ] && echo "Old PRM old_rap/$fiche missing - Dealing with a new PRM?" && mv new_rap/$fiche new_rap/OK && return 0  # not an error, this might be a new PRM

    [ ! -f new_rap/$fiche ] && [ -f new_rap/OK/$fiche ] && echo "Comparison already done, was OK" && return 0
    [ ! -f new_rap/$fiche ] && [ -f new_rap/KO/$fiche ] && echo "Comparison already done, was KO" && return 1
    [ ! -f new_rap/$fiche ] && echo "New PRM missing. Exiting." && return 1


    rm -rf avoir 

    compare_pdf_batch $batch_mode new_rap/$fiche old_rap/$fiche
    ret_code=$?

    OK="n"
    if [ "$batch_mode" = "-batch" ]; then   # Non-interactive mdoe
      if  [ "$ret_code" != "0" ]; then
         mv new_rap/$fiche  new_rap/KO
      else
         mv new_rap/$fiche  new_rap/OK
      fi     
    else                                     # Interactive mode
        sh ./avoir

        echo "OK ? (o/n)"
        read OK
        if [ "$OK" = "o" ]; then
           mv new_rap/$fiche  new_rap/OK
        else
           mv new_rap/$fiche  new_rap/KO
        fi
    fi
    
    ## Clean up if no difference
    if [ "$ret_code" = "0" ] || [ "$OK" = "o" ]
    then
      ppm_dir_ref="ppms/ppm_ref_${fiche}"
      ppm_dir_new="ppms/ppm_new_${fiche}"
      ppm_dir_diff="ppms/ppm_diff_${fiche}"
      rm -f $ppm_dir_ref $ppm_dir_new $ppm_dir_diff
    fi
    return $ret_code
}


echo "def compare_new_rap_old_rap [-batch]"
compare_new_rap_old_rap()
{
    cd new_rap/; fiches=`ls *.pdf`; cd -
    mkdir -p new_rap/OK new_rap/KO
    batch_mode=$1

    for fiche in $fiches
      do
      comp_fiche $batch_mode $fiche
    done

}


echo def genere_new_rap_old_rap  [ -perf ] [-data] [-dt_ev]  newarchives oldarchives
gen_fiche()
{
    cd preserve
    python $TRUST_ROOT/Validation/Outils/Genere_courbe/src/genererCourbes.py -p `ls *prm| grep -v test_lu.prm` --no_prereq
    if [ "$?" != "0" ]; then return 1; fi
    cd ..
    return 0
}

#
# Generate old and new 'fiche', potentially removing performance data and other stuff.
# This uses the raw information found in the .tgz file.
# Arguments 
#   gen_rap_fiche [-perf] [-data] [-dt_ev] fiche new_archive_dir reference_dir
#
gen_rap_fiche()
{
    perf=""
    data=""    
    dt_ev=""
    [ "$1" = "-perf" ] && perf=$1 && shift
    [ "$1" = "-data" ] && data=$1 && shift
    [ "$1" = "-dt_ev" ] && dt_ev=$1 && shift
    fiche=$1
    new=`realpath $2`
    ref=`realpath $3`
    
    mkdir -p new_rap old_rap
    
    [ "$ref" = "" ] || [ "$new" = "" ] || [ "$fiche" = "" ] && return 1
    [ ! -f $ref/$fiche ] && echo Fiche de reference "$ref/$fiche" manquante #  &&  return
    pdf=`basename $fiche .tgz`.pdf
    pdf_new=`find new_rap -name $pdf`
    
    #
    # Check if report already generated
    #
    if [ "$pdf_new" != "" ]
    then
      echo Fiche deja generee: $pdf
      [ $pdf_new -ot $new/$fiche ] && echo $pdf_new a refaire ?
      return 0
    fi

    echo ""
    echo @@ Handling report $pdf
    working_dir="rap_${fiche}"
    rm -rf $working_dir
    mkdir $working_dir
    cd $working_dir || exit -1
    
    #
    # Building new report
    #
    echo ""    
    echo @@ Building NEW report

    tar zxf $new/$fiche
    gen_fiche
    if [ "$?" != "0" ]; then cd ..; echo "gen_fiche FAILED!!!"; exit 1; fi

    mv preserve new
    mv new/rapport.pdf ../new_rap/$pdf

    #
    # Building old report
    #
    echo ""    
    echo @@ Building OLD report

    [ ! -f $ref/$fiche ] && echo $ref/$fiche absent  && cd .. && rm -rf $working_dir && return
    tar zxf $ref/$fiche
    \cp new/version_utilisee preserve

    if [ "$perf" != "" ]
    then
      cd new
      tar cvf perf.tar `find . -name '*'.perf` temps_total
      cd ../preserve
      tar cvf perf.tar `find . -name '*'.perf` temps_total
      tar xvf ../new/perf.tar
      cd ..
    fi

    if [ "$data" != "" ]
    then
      cd new
      tar cvf data.tar `find . -name '*'.data` 
      cd ../preserve
      tar cvf data.tar `find . -name '*'.data` 
      tar xvf ../new/data.tar
      cd ..
    fi

    if [ "$dt_ev" != "" ]
    then
      cd new
      tar cvf dt_ev.tar `find . -name '*'.dt_ev` temps_total
      cd ../preserve
      tar cvf dt_ev.tar `find . -name '*'.dt_ev` temps_total
      tar xvf ../new/dt_ev.tar
      cd ..
    fi

    gen_fiche
    if [ "$?" != "0" ]; then cd ..; echo "gen_fiche FAILED!!!"; exit 1; fi
    mv -f preserve/rapport.pdf ../old_rap/$pdf
    
    cd ..
    #
    # Clean-up
    #
    rm -rf $working_dir
}

genere_new_rap_old_rap()
{
    perf=""
    data=""
    dt_ev=""
    [ "$1" = "-perf" ] && perf=$1 && shift
    [ "$1" = "-data" ] && data=$1 && shift
    [ "$1" = "-dt_ev" ] && dt_ev=$1 && shift
    NEW=$1
    REF=$2
    [ "$NEW" = "" ] && exit
    [ "$REF" = "" ] && exit
    cd $NEW/; fiches=`ls *.tgz`; cd -

    for fiche in $fiches
      do
      gen_rap_fiche $perf $data $dt_ev $fiche $NEW $REF
    done
}


#
# Generate the validation makefile, potentially using Salloc. 
# Also generate a CTest file containing all the logic for the run, report generation and comparison of all PRMS.
#
echo "def Generate_makefile_validation [ -without_deps_exe ] [ -parallel_sjob ] [<time_file>]"
Generate_makefile_validation()
{
    deps="\$(exec)"
    [ "$1" = "-without_deps_exe" ] && deps="" && shift
    [ "$1" = "-parallel_sjob" ] && parallel="1" && shift
    SCRIPT_ROOT=$TRUST_ROOT/Validation/Outils/Genere_courbe/scripts
    LANCE=$SCRIPT_ROOT/Lance_gen_fiche
    source $LANCE
    if [ "$1" = "" ]     # Time file provided
    then
      prm=`get_list_prm`
    else
      prm=`get_list_prm_from_Times $1`
    fi

    res=`for cas in $prm; do basename $cas; done`
    resb=""
    res2=""
    for cas in $res
    do
      res2=$res2" "archives/$cas.pdf
      resb=$resb" "$cas
    done

    echo $ECHO_OPTS "all_tgz: $resb" >makefile
    echo $ECHO_OPTS "all:\t $res2" >> makefile

    function get_lance_cmd()
    {
        p=$1
        if [ "$parallel" = "1" ]; then
            full_file=`find $p/src -name "*.prm"`
            max_nb_proc=1
            if [ -f "$full_file" ]; then
               max_nb_proc=`python $SCRIPT_ROOT/get_nb_procs.py $full_file | sort -n | tail -1`
               # A PRM might have no test case at all:
               if [ "x$max_nb_proc" = "x" ]; then max_nb_proc=1; fi
            fi
            lance="$TRUST_ROOT/bin/Sjob/Salloc -n $max_nb_proc $LANCE"
            echo $lance
        else
            echo $LANCE
        fi
    }

    for p in $prm
    do
      lance=`get_lance_cmd $p`
      cas=$(basename $p)
      pdf="archives/$(basename $p).pdf"
      echo $ECHO_OPTS "BUILD/deps_$cas: $deps \$(shell find $p/src -newer $pdf 2>/dev/null)" 
      echo $ECHO_OPTS "\t @mkdir -p BUILD;touch BUILD/deps_$cas"
      echo "${pdf}: BUILD/deps_$cas"
      echo $ECHO_OPTS "\t@$lance \$(option_fast) -pdf_only  $p"
    done >> makefile

    for p in $prm
    do
      lance=`get_lance_cmd $p`
      cas=$(basename $p)
      echo $cas: archives/$cas.tgz
      echo archives/$cas.tgz: BUILD/deps_$cas
      echo $ECHO_OPTS "\t@$lance \$(option_fast) $p"
    done >> makefile
}
