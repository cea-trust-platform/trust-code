#/usr/bin/wish
# the next line restarts using wish \ 
#
#    exec /usr/bin/wish8.0 "$0" "$@"
# hello --
# Simple Tk script to create a button that prints "Hello, world".
# Click on the button to terminate the program.
#
# RCS: @(#) $Id: hello,v 1.2 1998/09/14 18:23:28 stanton Exp $
# 
# The first line below creates the button, and the second line
# asks the packer to shrink-wrap the application's main window
# around the button.
proc bidon { } {
    set nx 0
    set w .essa
    #catch {destroy $w}
    #toplevel $w   
    set f [ frame $w ]
    pack $w
    set ny 0
    set nz 0
    button .hello -text "Hello, world" -command {
	puts stdout "Hello, world"; destroy .
    }
    pack .hello
    button .listeview -text "List" -command {viewlist $w}
    pack .listeview
    button .essai -text "essai" -command { $DIR/info.cl  $NOM }
    button .exit -text "Exit" -command { destroy . }
    pack .essai .exit

    label .txt -text "meshes number nx $nx ny $ny nz $nz"
    button .modnb -text "Modify " -command { set  e [ lire_nb_maille $nx $ny $nz ] ; set e [ split $e ]; puts "$e"; set nx [ lindex  $e 0 ];puts "$nx";  .txt configure -text "meshes number nx $nx ny $ny nz $nz"}
    #pack .modnb .txt -side right


    label $f.tnx -text "nx"
    entry $f.nx
    $f.nx insert 0 "10" 
    button $f.but -text "Modify 2" -command "lire_nb_maille_2 $f.nx"  
    pack $f.but $f.nx $f.tnx -side right
    bind all <Control-c> {destroy .}  
}

proc lire_nb_maille { nx ny nz } {
    # puts "$nx $ny $nz";
    frame .frame
    label .lnx -text "nx"
    entry .nx 
    pack .nx .lnx -side right
    .nx insert 0 "$nx"
    set nx [ .nx get ]
    return "$nx $ny $nz";
}

proc exe_example { w name } {
    puts "$name"
    global DIR
    if [ file exists $name.prep ] {
	Dialog3 "$name.prep already exists It is neither copy nor read" ex 
    } else {
	set res "[ exec cat $DIR/examples/$name.prep > $name.prep ]"
	readmodel $name.prep $w
    }
}
proc Examples w {
   
    global DIR
    set win .listexemple
    if ![winfo exists $win.taille ] {
	toplevel $win
    } 
    wm deiconify $win
    raise $win
    if [winfo exists $win] {
	#puts "it is destroyed "
	#puts " [ winfo children $win ]"
	foreach i [winfo children $win ] {destroy $i }
	#destroy $win
    } 

    if ![winfo exists $win.taille ] { 
	#	  toplevel $win
	frame $win.taille
	
	frame $win.list
	listbox $win.list.list -xscrollcommand { .listexemple.listx set } -yscrollcommand { .listexemple.list.y set }
	scrollbar $win.listx  -command {  .listexemple.list.list xview } -orient horiz
	scrollbar $win.list.y  -command {  .listexemple.list.list yview } -orient vert
	#puts "[ls]"
	set li [ glob $DIR/examples/*.prep ]
	puts "$li"

	foreach j $li {
	    $win.list.list insert end [ exec  basename $j .prep ]
	}
	#pack  $win.taille $win.mn -side right
	pack $win.list.list  -expand yes -side left  -in $win.list  -fill both
 	pack  $win.list.y  -side left  -expand no -in $win.list -fill y
	
	pack $win.list -expand yes -side top -fill both 
	pack  $win.listx -expand 0 -side top   -in $win -fill x
	button $win.img -text "Picture" -command { Load_image_demo $w  [ lindex [ selection get ] 0 ]}
	button $win.edit -text "edit" -command {  Edit_f $w [ lindex [ selection get ] 0 ]}
	button $win.exe -text "copy and read" -command { exe_example $w [ lindex [ selection get ] 0 ]       }
	button $win.exit2  -text "exit" -command { destroy .listexemple }
	pack  $win.exe $win.img      $win.exit2 -side left -expand 1
#	bind $win.list.list <Double-Button-1> {exe_example $w [ lindex [ selection get ] 0 ]  ]    }
#	bind $win.list <Return> { exe_example $w [ lindex [ selection get ] 0 ]  ] }
    } else { puts "pb viewlist" }  
}
proc exe_trio name {
    global DIR
    Dialog3  "[exec $DIR/iTriou $name [pwd] ]" trio
}
proc Exec_trio w {
   
    
    set win .listdata
    if ![winfo exists $win.taille ] {
	toplevel $win
    } 
    wm deiconify $win
    raise $win
    if [winfo exists $win] {
	#puts "it is destroyed "
	#puts " [ winfo children $win ]"
	foreach i [winfo children $win ] {destroy $i }
	#destroy $win
    } 

    if ![winfo exists $win.taille ] { 
	#	  toplevel $win
	frame $win.taille
	
	frame $win.list
	listbox $win.list.list -xscrollcommand { .listdata.listx set } -yscrollcommand { .listdata.list.y set }
	scrollbar $win.listx  -command {  .listdata.list.list xview } -orient horiz
	scrollbar $win.list.y  -command {  .listdata.list.list yview } -orient vert
	#puts "[ls]"
	set li [ glob ../prepro/trans_geom/*.data ]
	puts "$li"

	foreach j $li {
	    $win.list.list insert end [ exec  basename $j ]
	}
	#pack  $win.taille $win.mn -side right
	pack $win.list.list  -expand yes -side left  -in $win.list  -fill both
 	pack  $win.list.y  -side left  -expand no -in $win.list -fill y
	
	pack $win.list -expand yes -side top -fill both 
	pack  $win.listx -expand 0 -side top   -in $win -fill x

	button $win.edit -text "edit" -command {  Edit_f $w ../prepro/trans_geom/[ lindex [ selection get ] 0 ] }
	button $win.exe -text "execute" -command { exe_trio [ lindex [ selection get ] 0 ]       }
	button $win.exit2  -text "exit" -command { destroy .listdata }
	pack  $win.exe $win.edit     $win.exit2 -side left -expand 1
	bind $win.list.list <Double-Button-1> {exe_trio [ lindex [ selection get ] 0 ]  ]    }
	bind $win.list <Return> { exe_trio  [ lindex [ selection get ] 0 ]  ] }
    } else { puts "pb viewlist" }  
}

proc viewlist w {
    set win .viewlist
    if ![winfo exists $win.taille ] {
	toplevel $win
    } 
    wm deiconify $win
    raise $win

    if [winfo exists $win] {
	#puts "it is destroyed "
	#puts " [ winfo children $win ]"
	foreach i [winfo children $win ] {destroy $i }
	#destroy $win
    } 

    if ![winfo exists $win.taille ] { 
	#	  toplevel $win
	frame $win.taille
	label $win.taille.txt -text [ ligne_text $w -1 ]
	button $win.taille.mn -text "Modify" -command { Edit $w -1 }
	pack $win.taille.txt $win.taille.mn -in $win.taille -side left
	pack $win.taille -in $win -side top
	frame $win.list
	listbox $win.list.list -xscrollcommand { .viewlist.listx set } -yscrollcommand { .viewlist.list.y set }
	scrollbar $win.listx  -command {  .viewlist.list.list xview } -orient horiz
	scrollbar $win.list.y  -command {  .viewlist.list.list yview } -orient vert
	for { set i  0 } { $i < [ $w.liste.nbre cget -text ] } {incr i } {
	    $win.list.list insert end [ ligne_text $w $i ] }
	#pack  $win.taille $win.mn -side right
	pack $win.list.list  -expand yes -side left  -in $win.list  -fill both
 	pack  $win.list.y  -side left  -expand no -in $win.list -fill y
	
	pack $win.list -expand yes -side top -fill both 
	pack  $win.listx -expand 0 -side top   -in $win -fill x
	button $win.add -text "add" -command { Add $w  }
	button $win.edit -text "edit" -command {  Edit $w [ lindex [ selection get ] 0 ]}
	button $win.delete -text "delete" -command { if { [ Dialog2 "you are close to destroy [ lindex [ selection  get ] 0 ] " {Yes No} ] != "No"   } {Delete $w [ lindex [ selection  get ] 0 ] } }
	button $win.copy -text "copy" -command { Add $w [ lindex [ selection  get ] 0 ] }
	button $win.move -text "move" -command { Move $w [ lindex [ selection  get ] 0 ] }
	button $win.exit2  -text "exit" -command { destroy .viewlist }
	pack $win.add $win.edit $win.copy $win.move  $win.delete $win.exit2 -side left -expand 1
	bind $win.list.list <Double-Button-1> {set sel [ lindex [ selection get] 0 ];foreach ii $sel { set es [ Edit $w $ii;] } ;  }
	bind $win.list <Return> { [ Edit $w [ lindex [ selection get ] 0 ] ] }
    } else { puts "pb viewlist" }
}



proc lire_nb_maille_2 { nx  } {
    if ![winfo exists .es4] {
	toplevel .es4
	
	#frame .es4.lire_nb_maille_2
	#pack .es4.lire_nb_maille_2
	label .es4.es -text "nx"
	pack .es4.es
	set nxl "10"
	$nx delete 0 end
	$nx insert 0 $nxl
	button .es4.exit -text "exit" -command { destroy .es4 }
	pack .es4.exit
    } else {
	wm deiconify .es4
	raise .es4
    }
}

proc search_list { name0 list } {
    set name [ string toupper $name0 ]
    set i 0
    set n [ $list size ]
    set res [ string compare $name [ $list get $i ] ]
    while {  [  string compare $name [ $list get $i ] ] != 0 &&  $i < $n   } { set i [ expr $i + 1 ] }
    if { $i >= $n } { if { [string compare $name "TAILLES"] != 0 } { set i -2 } else { set i -1 } }
    return $i
}

proc copy_list { list1 list2 } {
    $list1 delete 0 end
    set n [ $list2 size ] 
    #puts "$n"
    for {set i 0} { $i < $n } {incr i ; } {
	$list1 insert end [ $list2 get $i]
    }
}

proc readinfo3 w {
    global DIR
    global NOM
    global testinfo
    puts "reading of informations"
    
    set w2 $w.info
    if [ winfo exist $w2 ] {
	#puts "[winfo children $w2]"
	foreach i [winfo children $w2] {destroy $i }
    } else {
    frame $w2
    }
    listbox  $w2.lfort 
    listbox $w2.lutil
    #on rentre le champ taille
    set no "-1"
    frame $w2.$no
    label $w2.$no.rout_fort -text "TAILLES"
    label $w2.$no.rout_util -text "nodes"
    label $w2.$no.nargs -text  "3"
    label $w2.$no.v0 -text "nx"
    label $w2.$no.v1 -text "ny"
    label $w2.$no.v2 -text "nz"
    label $w2.$no.c0 -text "nx"
    label $w2.$no.c1 -text "ny"
    label $w2.$no.c2 -text "nz"
    label $w2.$no.comm -text "sizes"
    #
    set no "0"
    set CAS [lindex [split $NOM . ] 0]
    set nsup "infosupp$CAS"
    #puts "$nsup"
    if [ file exists $nsup ] {
	puts "reading of $nsup"
	set listef [list "$DIR/info.prepro" "$nsup" ]
	set testinfo 3
    } else {
	set listef [list "$DIR/info.prepro"]
    }
    foreach namef $listef {
    set fileid [open $namef r]
    set ok 1
    while  { ! [ eof $fileid ]  }  {
	gets $fileid line
	#puts $line
	set es [ split $line "," ]
	if  { [string compare "[lindex $es 0 ]"  "" ] != 0 }  {
	    #puts "nom [lindex $es 0 ]" 
	    frame $w2.$no
	    # nouveau on verifie qu'il n'y a pas conflit entre les routines utilisateurs et celles dans info.prepro
	    set newfort [lindex $es 0]
	    set newutil [lindex $es 1]
	    for { set ii 0 } { $ii < [  $w2.lutil size] } { incr ii } {
		if { ![ string compare $newfort [ $w2.lfort get $ii ] ] } {
		puts "the subroutine $newfort  already exists, remove the subroutine in infosupp$CAS and in routinesupp$CAS"
		set ok 0
		}
		if { ![ string compare $newutil [ $w2.lutil get $ii ] ] } {
		puts "the user subroutine $newutil  already exists, remove the subroutine in infosupp$CAS"
		set ok 0
		}  
		}
	    if { $ok == 0 } { exit }
	    label $w2.$no.rout_fort -text "$newfort"
	    $w2.lfort insert end "$newfort"
	    label $w2.$no.rout_util -text "$newutil"
	    $w2.lutil insert end "$newutil"
	    label $w2.$no.nargs -text  "[lindex $es 2]"
	    set j 3
	    set nargs "[lindex $es 2]"
	    for {set i 0} {$i < $nargs } {incr i ;} {
		label $w2.$no.v$i -text [lindex $es $j]
		incr j
		label $w2.$no.c$i -text [lindex $es $j]
		incr j
		#puts "$j"
	    }
	    #puts "[llength $es ]"
	    label $w2.$no.comm -text "[ lindex $es [expr [ llength $es ] - 1]]"
	    set no [ expr $no + 1 ]
	}
    }
    }
    label $w2.nbre -text "$no"
    #puts "[$w2.lfort configure]"
    #puts "es [$w2.lutil get 0 ]"
    
    #puts "nbre  [ $w2.nbre cget -text ]"
    puts "reading ended"
}




proc readmodel { name  w { fort 0 } } {
    global DIR
    global PWD
    global NOM
    global testinfo
    set testinfo2 0
    puts " model reading"
    if { $fort != 0 }   {
	exec sh $DIR/recup_call $name 2>/dev/null  
	puts "end of $name reading"
	set  fileid [open "/tmp/file1" r]
	$w.modif configure -text "1" 
	set NOM "nouveau.prep"
    }    else  {
	
	set PWD [ exec dirname $name ]
        puts "[  cd $PWD ]"
       puts  "[ exec $DIR/initprepro [pwd]  ]" 
	set NOM [ exec basename $name ]
	set  fileid [open $name r] }
    set no 0
    set w2 $w.liste
    # destroy $w.liste.n0
    set nm1  $w2.n-1 
    set nm2 $w2.cn-1
    set liw  [ winfo children $w2 ]
#    puts  " $liw";
    set i1 [ lsearch $liw $nm1]
    set liw [ lreplace $liw $i1 $i1 ]
    set i2 [ lsearch $liw $nm2 ]
    set liw [ lreplace $liw $i2 $i2 ]
 #   puts "ici $i1 $i2 $liw"
    foreach i  $liw { destroy $i  }
    

    if [ info exist $w2 ] { puts " one reads .." ; destroy $w2}
    
    
    while  { ! [ eof $fileid ]  }  {
	gets $fileid line
	set es [ split $line  ]

	set rout [ lindex $es 0 ]

	if { ![ string compare "$rout" "file" ] } { puts "one exits";break }
	set  type [ search_list $rout $w.info.lfort ]
	
	if { $type > -1} {
	    frame $w2.n$no 
	    set w2no $w2.n$no 
	#    puts "ici $rout $type"
	    label $w2no.type -text "$type"
	    set nargs [ lindex $es 1 ]
	    #puts "es2 $nargs [ $w.info.$type.nargs cget -text ]"
	    if { $nargs != [ $w.info.$type.nargs cget -text ] } { puts "error!!!! with $line number of arguments is not ok" }
	    for { set i 0 } { $i <  $nargs   } { incr i } {
		label $w2no.arg$i -text "[ lindex $es [ expr $i + 2 ] ]" }
	    #puts "$i"
	    set i [ expr $i + 2 ]
	    set tt  [ llength $es ] 
	    for { set kk $tt } { $kk >= $i } { incr kk -1  } {set lc [ string  length "[lindex $es $kk]"];if { $lc != 0 } break;}
	    #puts "$kk"
	    label $w2no.comm -text "[ lrange $es $i $kk ]"
	    label $w2.cn$no -text "n$no"
	    label $w2.n$no.nmax -text "$nargs"
	    #puts "commentaire [ $w2no.comm cget -text ]  [ $w2.cn$no cget -text ] "
	    
	    set no [ expr $no + 1 ]
	} else { if { $type > -2 } {
	    set nargs [ lindex $es 1 ]
	    set w2no $w2.n-1
	    for { set i 0 } { $i < $nargs   } { incr i } {
		$w2no.arg$i configure -text "[ lindex $es [ expr $i + 2 ] ]" }
	    #puts "$i"
	} else { if { $testinfo != 3 } { set testinfo2 -1 } ;puts "$rout is not understood.... not taken into account" }
	}
    }
    puts "lines number found $no "
    label $w2.nbre -text "$no"
    label $w2.nnbre -text "$no"
    if ![ eof $fileid ] {
	while {![ eof $fileid ]} {
	#puts "on a trouve file"
	    #puts "ici [ eof $fileid ]"
	#gets $fileid $line
	#puts $line
	    set ligne [ split $line ]
	set file [ lindex $ligne 1 ]
	    set CAS [lindex [split $NOM . ] 0]
	    set file2 [ lindex [ split $file $CAS ] 0]
	    regsub $CAS $file  "" file2
	    set file "$file2$CAS"
	    puts "one looks if it needs to generate $file"
	#puts $file
	    set status_fe 0
	if [ file exists $file] {
	    puts "WARNING one does not modify $file"
	    set file0 $file
	    set file $file.d
	    
	    set status_fe 1
	    #set test [string compare [ lindex [split [gets $fileid line]] 0 ] "file"]
	    #while { $test && ![ eof $fileid ]} {set ligne [split [gets $fileid line]]; set test [string compare "[lindex $line 0 ]" "file" ] }
	} 
	    
	    if ![ eof $fileid ] {
		puts "creation of $file"
		#gets $fileid line
		set sortie [  open $file w ]
		set test [string compare [ lindex [split [gets $fileid line]] 0 ] "file" ]
		while { $test && ![ eof $fileid ]} { puts $sortie "$line" ; set ligne [split [gets $fileid line]]; set test [string compare "[lindex $line 0 ]" "file" ] }
		 #puts "la [ eof $fileid ]"
		close $sortie
	    
		if { $status_fe != 0 } {
		    set resdiff "[ exec $DIR/test_diff $file $file0  ]"
		    puts "$resdiff"
		}
	}
	}
	
    }
    puts "End of model reading"
    readinfo3 $w
    if {$testinfo2!=0 && $testinfo==3 }    { puts "restart since fileinfo exists";readmodel $name $w $fort }
    viewlist $w
}

proc Save { w } {
    global NOM
    global PWD
    global listefichier
    set types { {{Prepro Files} {*.prep}} {{All}       {*}} }
    set name [tk_getSaveFile -defaultextension .prep -initialfile $NOM -initialdir [pwd] -filetypes $types]
    
    if { $name != "" } {
	puts "Saving in $name"
	$w.modif configure -text "0" 
	set fileid [ open $name w ]
	for { set ii -1 } { $ii < [ $w.liste.nbre cget -text ] } { incr ii } {
	    set ligne $w.liste.[$w.liste.cn$ii cget -text]
	    set ligneinfo $w.info.[$ligne.type cget -text]
	    set sortie "[ $ligneinfo.rout_fort cget -text ] [ $ligneinfo.nargs cget  -text ]"
	    set nargs  [ $ligneinfo.nargs cget  -text ]
	    for { set jj 0 } { $jj <  $nargs  } { incr jj } {
		set sortie "$sortie [$ligne.arg$jj cget -text]"
	    }
	    set sortie "$sortie [$ligne.comm cget -text ]"
	    puts $fileid "$sortie"
	 #   puts "$sortie"
	}
	# on ecrit les files...
	set CAS [lindex [split $NOM . ] 0]

	foreach no $listefichier {
	    set nomm "$no$CAS"
	    if [ file exists $nomm ] {
		set entree [ open $nomm r ]
		puts $fileid "file $no"
		puts $fileid "[ read $entree ]"
	    }
	}
	close $fileid
	set NOM [ exec basename $name ]
	readmodel $name $w
    }
}
proc ligne_text { w i } {
    # puts "$w $i"
    set ik $i
    set ligneliste $w.liste.[$w.liste.cn$i cget -text]
    set type [ $ligneliste.type cget -text ]
    set ligneinfo $w.info.$type
    set ligne "[ $ligneinfo.rout_util cget -text ]"
    for { set i 0 } { $i < [ $ligneinfo.nargs cget -text ] } {incr i } {
	set ligne  "$ligne [ $ligneinfo.v$i cget -text ] = [ $ligneliste.arg$i cget -text ] "
	#set ligne  [ $ligneinfo.v$i cget -text ] 
	#puts "[ $ligneinfo.v$i cget -text ] "
    }
    set ligne "$ik \"[ $ligneliste.comm cget -text ]\" $ligne"
    return $ligne
}



proc Edit_f { w f } {
    #Dialog2 "On ne peut pas pour l'instant utiliser l'interface pour editer les fichiers... desole" {Ok}
   #return
    #Cela ne marche pas !!!
    puts "$f edit"
    global NOM
    global DIR
    set name $w.$f
    # ATTENTION plus de controle de l'edition pour l'instant
    if [ winfo exist $name ] {
	Dialog2 "$f edition is running Please close the window" {OK}
    } else {
	#frame $name
	#toplevel $name
	exec $DIR/iediteur $f &
    }
}
proc Edit { w i } {
    global DIR
    set name .edit$i
    
    if ![winfo exists $name] {
	toplevel $name
	label $name.label -text "One edit $i "
	pack $name.label
	# TYPE
	set type [ $w.liste.[$w.liste.cn$i cget -text ].type cget -text ]
	#label $name.type -text "[ $w.info.$type.rout_util cget -text ]"
	frame $name.t
	pack $name.t
	label $name.t.rcom -text "[ $w.info.$type.comm cget -text ]"     
	#entry $name.etype
	#$name.etype insert 0 "[ $name.type cget -text ]"
	#pack $name.type $name.etype -side top
	#set mnubuttonoptions [ $w.info.lutil get $type ]
	set m [tk_optionMenu $name.t.mtype mnubuttonoptions$i [ $w.info.lutil get $type  ] ]
	$m entryconfigure 0 -command {puts "ok"}
	
	for { set ii 0 } { $ii < [  $w.info.lutil size] } { incr ii } {
	    $m add radiobutton -variable mnubuttonoptions$i -label  [ $w.info.lutil get $ii ] -command "modify $w $i $type $ii"
	}
	
	
	#label $name.t.image -image image$i
	set fileimg "$DIR/[$w.info.lfort get $type ].gif"
	puts "ou $fileimg"
	if { [ file exists $fileimg ] } {
	    button $name.t.img -text "picture.." -command "Load_image $w $type" 
	} else { button $name.t.img }
	
	pack $name.t.mtype $name.t.rcom $name.t.img -side left -in $name.t
 	#entry $name.i
	#$name.i insert 0 $i
	var_edit $name $w $i
	button $name.exit2  -text "cancel" -command " destroy .edit$i "
	set .name $name
	button $name.ok -text "ok" -command " $w.modif configure -text 1; affecte_var $name $w $i; destroy .edit$i ;viewlist $w "
	pack $name.exit2 $name.ok -expand 1 -side left 
    } else {
	wm deiconify $name
	raise $name
    }
}

proc Load_image { w type } { 
    global DIR
    set base .img$type
    if ![winfo exists $base] {
	toplevel $base
	set titre [$w.info.lutil get $type ]
	set fileimg  "$DIR/[$w.info.lfort get $type ].gif"
	wm title $base "$titre"
	wm iconname $base "$titre"
	image create photo image$type 
	image$type configure -file $fileimg
	label $base.img -image image$type
	pack $base.img 
	button $base.ok -text "Ok" -command " destroy $base"
	pack $base.ok 
    } else {
	wm deiconify $base
	raise $base
    }
}
proc Load_image_demo { w type } { 
    global DIR
    set base .img$type
    if ![winfo exists $base] {
	toplevel $base
	set titre $type
	set fileimg  "$DIR/examples/$type.gif"
	if [ file exist $fileimg ]	{
	    wm title $base "$titre"
	    wm iconname $base "$titre"
	    image create photo image$type 
	    image$type configure -file $fileimg
	    label $base.img -image image$type
	    pack $base.img 
	    button $base.ok -text "Ok" -command " destroy $base"
	    pack $base.ok 
	} else {
	    Dialog3 "$fileimg does not exist" loadimg
	}
    } else {
	wm deiconify $base
	raise $base
    }
}
proc modify { w i type newtype } {
    set ligne  $w.liste.[$w.liste.cn$i cget -text]
    #puts "$w $i $type $newtype" 
    if { $type != $newtype } {
	$ligne.type config -text "$newtype"
	#puts "[ info vars .essa.liste.n0.arg0 ] "
	set nmax [ $ligne.nmax cget -text ]
	set nargs [ $w.info.$newtype.nargs cget  -text ]
	puts "namx $nmax $nargs"
	for { set jj $nmax } { $jj < $nargs } {incr jj } {
	    label $ligne.arg$jj -text "?"   
	}
	if { $nargs > $nmax } {
	    puts "here" 

	    $ligne.nmax configure -text "$nargs" 
	     puts "[ $ligne.nmax cget -text]" }
	destroy .edit$i
	Edit $w $i
    } else {
	puts "ok"
    }
}
proc var_edit { w1 w j } {
    set ligne  $w.liste.[$w.liste.cn$j cget -text]
    set type  [ $ligne.type cget -text ]
    #puts "$type"
    frame $w1.comm
    label $w1.comm.txt -text "Comment:"
    entry $w1.comm.etxt -xscrollcommand "$w1.comm.s1 set" 
    bind w1.comm <Double-Button-1> { puts "Okkkkk"}
    scrollbar $w1.comm.s1 -relief sunken -orient horiz -command "$w1.comm.etxt xview"
    $w1.comm.etxt insert 0 "[ $ligne.comm cget -text ]"
    #pack  $w1.comm.txt $w1.comm.etxt $w1.comm.s1 -in $w1.comm  -side left
    pack   $w1.comm.etxt $w1.comm.s1 -in $w1.comm  -side top  -fill x
    pack configure $w1.comm.s1 -side top
    #pack $w1.comm.s1 -side top
    pack $w1.comm -fill x
    for { set ii 0 } { $ii < [ $w.info.$type.nargs cget -text ] } { incr ii } {
	set wii "$w1.$ii"
	frame $wii
	label $wii.var -text "[ $w.info.$type.c$ii cget -text ] ([ $w.info.$type.v$ii cget -text ])"
        entry $wii.evar
	$wii.evar insert 0 "[ $ligne.arg$ii cget -text ]" 
	pack $wii.var $wii.evar -in $wii -side left
	
	pack $wii -side top -in $w1
    } 
}

proc affecte_var { w1 w j } {
    set ligne $w.liste.[$w.liste.cn$j cget -text]
    set type  [ $ligne.type cget -text ]
     puts "here $type"
    
    #entry $w1.etxt
    
    #$w1.etxt insert 0 "[ $w.liste.$j.comm cget -text ]"
    set routt [ $w.info.$type.rout_fort cget -text ]
    if ![ string compare $routt COUPE2D ] {
	$ligne.comm configure -text "coupe_2D"
    } else {
	if ![ string compare $routt COUPEAXI2D ] {
	    $ligne.comm configure -text "coupe_Axi2D"
	} else {
	    $ligne.comm configure -text [ $w1.comm.etxt get ]
	}
    }
    for { set ii 0 } { $ii < [ $w.info.$type.nargs cget -text ] } { incr ii } {
	#label $w1.var$ii -text "[ $w.info.$type.v$ii cget -text ]"
        #entry $w1.evar$ii
	$ligne.arg$ii configure -text "[ $w1.$ii.evar get]"  
	#pack $w1.var$ii $w1.evar$ii
    } 
}


proc Delete { w i } {
    # pour detruire i on modifie les cn$j pour i>j et on diminue le nbre de champ valide
    for { set j $i } { $j < [expr [ $w.liste.nbre cget -text] -1 ]} { incr j } {
	$w.liste.cn$j configure -text "[ $w.liste.cn[expr $j + 1] cget -text ]"
    }
    destroy $w.liste.cn$j 
    $w.liste.nbre configure -text [ expr [ $w.liste.nbre  cget -text ] -1 ] 
    viewlist $w
    $w.modif configure -text "1"
}

proc Add { w {icopy -1}} {
    $w.modif configure -text "1"
    set nbre [ $w.liste.nbre cget -text ] 
    set nnbre   [ $w.liste.nnbre cget -text ] 
    set nbrem [ expr [ $w.liste.nbre cget -text ] + 1 ]
    set nnbrem  [expr [ $w.liste.nnbre cget -text ] + 1 ]
    
    
    label $w.liste.cn$nbre -text "n$nnbre"
    
    frame $w.liste.n$nnbre
    $w.liste.nnbre configure -text "$nnbrem"
    $w.liste.nbre configure -text "$nbrem"
    #label $w.liste.cn$nbre -text "n$nnbre"
    if { $icopy != -1 } {
	set ligne  $w.liste.[$w.liste.cn$icopy cget -text]
	label $w.liste.n$nnbre.type -text "[ $ligne.type cget -text]"
	set nmax [ $ligne.nmax cget -text]
	label $w.liste.n$nnbre.nmax -text "$nmax"
	for { set jj 0 } { $jj < $nmax } {incr jj } {
	    label $w.liste.n$nnbre.arg$jj -text "[$ligne.arg$jj cget -text ]"
	    
	}
	label $w.liste.n$nnbre.comm -text "[ $ligne.comm cget -text]"
    } else {
	label $w.liste.n$nnbre.type -text "0"
	label $w.liste.n$nnbre.nmax -text "0"
	label $w.liste.n$nnbre.comm -text "?"
    }
    Edit $w $nbre
    
}



proc Move {w i } {
    puts "$w $i"
    toplevel .move
    label .move.label -text "Move at number "
    entry .move.etype
    .move.etype insert 0 ""
    entry .move.i 
    .move.i insert 0 "$i"
    pack .move.label .move.etype -side left
    button .move.exit2  -text "cancel" -command { destroy .move }
    button .move.ok -text "ok" -command { move $w [.move.i get ]  [.move.etype get  ] }
    bind .move <Return> {   move $w [.move.i get ] [.move.etype get  ] }
    pack .move.exit2 .move.ok
}
proc move { w i in } {
    puts "$w $i $in"
     set nbre [ $w.liste.nbre cget -text ]
    set test "0"
    set liste " "
    for { set jj 0 } { $jj < $nbre } {incr jj } {
	if [ winfo exists .edit$jj ] { set test "1"
	                               set liste "$liste $jj" }
        }
    if { $test != 0 } {
	Dialog2 "It needs to quit all editing windows ($liste)" {Ok}
	} else {
    $w.modif configure -text "1"
    if { $in != "" } {
	if { $in == "end" }	{
	    puts "no code"
	}
	if { $i < $in } {
	    set cni [ $w.liste.cn$i cget -text ]
	    for { set jj $i } { $jj < $in } { incr jj } {
		$w.liste.cn$jj configure -text [ $w.liste.cn[expr $jj + 1 ] cget -text ]
	    }
	    $w.liste.cn$in configure -text "$cni"
	} else {
	    set cni [ $w.liste.cn$i cget -text ]
	    for { set jj $i } { $jj > $in } { incr jj -1 } {
		$w.liste.cn$jj configure -text [ $w.liste.cn[expr $jj - 1 ] cget -text ]
		puts " ici $jj"
	    }
	    $w.liste.cn$in configure -text "$cni"
	}
	
	destroy .move
	viewlist $w
    }
   } 
}

proc gen_code { w } {
    global DIR
    set nx [ $w.liste.n-1.arg0 cget -text ]
    set ny [ $w.liste.n-1.arg1 cget -text ]
    set nz [ $w.liste.n-1.arg2 cget -text ]
    puts "nx: $nx, ny: $ny , nz: $nz"
    
    if { $nx != 0  &&  $ny != 0  &&  $nz != 0  } {
	set fileid0 [ open "defineh_inc" w ] 
	puts  $fileid0 "\#ifndef NIMAX \n\#define NIMAX $nx \n\#define nx $nx\n\#endif"
	puts  $fileid0 "\#ifndef NJMAX \n\#define NJMAX $ny \n\#define ny $ny\n\#endif"
	puts  $fileid0 "\#ifndef NKMAX \n\#define NKMAX $nz\n\#define nz $nz \n\#endif"
	
	close $fileid0
	set r "appelrac.h"
	set fileid [ open $r w ]
	put_ligne $fileid "print *,'pas de raccord particulier'"
	put_ligne $fileid "racpres=0"
	close $fileid
	set rd "decl_appelrac.h"
	set fileidr [ open $rd w ]
	put_ligne $fileidr " "
	close $fileidr
	set r "appeldec.h"
	set fileid [ open $r w ]
	put_ligne $fileid "print *,'pas de decoupage'"
	put_ligne $fileid "return"
	close $fileid
	set rd "decl_appeldec.h"
	set fileidr [ open $rd w ]
	put_ligne $fileidr "       integer NPROCDEC"
	close $fileidr
	set r "appel.h"
	puts "code created in $r"
	set fileid [ open $r w ]
	set rd "decl_appel.h"
	puts "incl created in $rd"
	set fileidr [ open $rd w ]
	for { set ii 0 } { $ii < [ $w.liste.nbre cget  -text ] } { incr ii } {
	    set ligne  $w.liste.[$w.liste.cn$ii cget -text]
	   
	    set ligneinfo $w.info.[$ligne.type cget -text]
	    set routefort [ $ligneinfo.rout_fort cget -text ]
	    set testi 0
	    if { ![ string compare $routefort SEPRAC ] } {
		close $fileid
		set r "appelrac.h"
		puts "code created in $r"
		set fileid [ open $r w ]
		close $fileidr
		set rd "decl_appelrac.h"
		puts "incl created in $rd"
		set fileidr [ open $rd w ]
		set testi 1 
	    } else {
		if { ![ string compare $routefort SEPDEC ] } {
		    close $fileid
		    set r "appeldec.h"
		    puts "code created in $r"
		    set fileid [ open $r w ]
		    close $fileidr
		    set rd "decl_appeldec.h"
		    puts "incl created in $rd"
		    set fileidr [ open $rd w ]
		    set testi 1
		}
	    }
	    # on ecrit les commentaires
	    put_ligne $fileid [ $ligne.comm  cget -text ] 1
    	    if { [ string compare $routefort FORT ] } {

    	    set titi "[ $ligne.comm  cget -text ]"
    	    set instri "write(14,*) 'comment $titi end_comment'"
            put_ligne $fileid "$instri" 2
	    }
	    # on ecrit la valeur des variables
	    set var2 ""
	    set nargs  [ $ligneinfo.nargs cget  -text ] 
	    for { set jj 0 } { $jj <  $nargs  } { incr jj } {
		set val [$ligne.arg$jj cget -text ]
	        set ze [ string compare $val "0" ]

		set varp [ string toupper [$ligneinfo.v$jj cget -text]]
		set c [ string index $varp 0 ]
		set type "real"
	
		set t2 [ string compare $c "I" ]
		set tf2 [ string compare $c "O" ]
		set ischar [ string compare $varp "NAME" ]
		if {  $t2 >=0   &&   $tf2<0  } {
		    set type "integer"
		} else {
			if { $ze == 0 } {
				puts "one modify the line $ii, variable var [$ligneinfo.c$jj cget -text] ([$ligneinfo.v$jj cget -text]) of [ $ligne.arg$jj cget -text ] at 0. "
			       set zero 0.
			       $ligne.arg$jj configure -text $zero
	                       $w.modif configure -text "1" 
			}
		}	
		if { $ischar == 0 } {
			set type "character*80"
		        set c2 [ string index $val 0 ]
		        set not_ok [ string compare $c2 "'" ]

			if { $not_ok != 0 } {
				set toto "'$val'"
				puts "one modify the line $ii, variable var [$ligneinfo.c$jj cget -text] ([$ligneinfo.v$jj cget -text]) of [ $ligne.arg$jj cget -text ] at $toto "
			       $ligne.arg$jj configure -text $toto
	                       $w.modif configure -text "1" 
			   }	   
		}
		
		put_ligne $fileidr "       $type $varp"
		if { $jj != 0 } {
		    set var2 "$var2, [$ligneinfo.v$jj cget -text]"
		} else {
		    set var2 "$var2 [$ligneinfo.v$jj cget -text]"
		}
		set var "[$ligneinfo.v$jj cget -text]=[$ligne.arg$jj cget -text ]"
		put_ligne $fileid $var
	    }
	    
	    
	    if { ![ string compare $routefort FORT ] } {
		put_ligne $fileid "CALL FORT"
		set var [ $ligne.comm  cget -text ]
	    } else {
		if { $testi != 1 } {
		    set var "CALL $routefort ($var2 )"
		} else {
		    set testi 0
		    set var  " "
		}
	    }
	    put_ligne $fileid $var
	    put_ligne $fileid "call FLUSH_cond"
	}
	close $fileid
	close $fileidr
	set res "[ exec $DIR/modif_decl [pwd] ]"
    } else {  
	Dialog2 "nx or ny or nz has value 0, code not created" {Ok}
	Edit $w -1
    }
}

proc put_ligne { fileid ligne0 { comm  0 } } {
    #puts "$ligne0"
    set ligne "$ligne0"
    if { $comm != 1 } {
	set ligne "      $ligne"
    } else { set ligne "C $ligne" }
    if { [ string length $ligne ] < 50 } {
	puts $fileid "$ligne"
    } else { 
	#puts "$comm il faut decouper la ligne $ligne"
	set es [ split $ligne ]
	set l [ llength $es ]
	set i 0
	#puts "ici" 
	#puts [ lindex $es $i ]
	set prem 0
	while { $i < $l }	{
	    if { $prem != 0 } {
		if { $comm == 1 } { set li "C  "
		} else { set li "     1 " 
		    if { $comm == 2 } { set li "$li , '" }
		}  } else {
		    set li "[ lindex $es $i ]"
		    set i [ expr $i + 1 ] 
		}
	    
	    set j $i
	    while { [ string length $li ] < 50 } { set li "$li [ lindex $es $j ]" ; set j [ expr $j + 1 ] } 
	    if  { $comm == 2 && $j < $l } {
		set li "$li '"
	    }
	    puts $fileid "$li"
	    set i $j
	    set prem 1
	}
    }
}


proc Dialog2 { mesg { options  Ok} } {
    puts "$mesg $options"
    global res
    set base .essa.message
    set sw [winfo screenwidth .]
    set sh [winfo screenheight .]
    if {![winfo exists $base]} {
        toplevel $base -class vTcl
        wm title $base "Visual Tcl Message"
        wm transient $base .essa
        frame $base.f -bd 2 -relief groove
        label $base.f.t -bd 0 -relief flat -text $mesg -justify left 
        frame $base.o -bd 1 -relief sunken
        foreach i $options {
            set n [string tolower $i]
            button $base.o.$n -text $i -width 5 \
		-command "
                 set res $i
                puts $i
                destroy $base
		"
            pack $base.o.$n -side left -expand 1 -fill x
        }
        pack $base.f.t -side top -expand 1 -fill both -ipadx 5 -ipady 5
        pack $base.f -side top -expand 1 -fill both -padx 2 -pady 2
        pack $base.o -side top -fill x -padx 2 -pady 2
    }
    wm withdraw $base
    update idletasks
    set w [winfo reqwidth $base]
    set h [winfo reqheight $base]
    set x [expr ($sw - $w)/2]
    set y [expr ($sh - $h)/2]
    
    wm geometry $base +$x+$y
    
    wm deiconify $base
    grab $base
    tkwait window $base
    grab release $base
    return $res
    #return "Oui"
}
proc test { comm w  } {
 if { [$w.modif cget -text ] != 0 } {
     if { [ Dialog2 "some modifications not saved  Save it" {Yes No} ] != "No"} {
	 Save $w;
	 if { [$w.modif cget -text ] == 0 }  {$comm $w } 
     } else {
	 if { [ Dialog2 "Are you sure taht you do not want to save the modifications ?" {Yes No } ] == "Yes" } {
	     $comm $w }
     }
 } else { $comm $w }
}

proc test2 { comm w  } {
 if { [$w.modif cget -text ] != 0 } {
     if { [ Dialog2 "some modifications not saved Save it" {Yes No} ] != "No"} {
	 Save $w;
	 if { [$w.modif cget -text ] == 0 }  {$comm $w } 
     } else {
	 if { [ Dialog2 "It is necessary to save modifications to ensure to have good boundaries conditions. Run whatever?" {Yes No } ] == "Yes" } {
	     $comm $w }
     }
 } else { $comm $w }
}


proc New { w  } {
   exe_example $w new	
}
proc Dialog3 { mesg num { options  Ok} } {
    #puts "$mesg $options"
    
    set base .essa.message$num
    set sw [winfo screenwidth .]
    set sh [winfo screenheight .]
    if [winfo exists $base] { destroy $base }
    if {![winfo exists $base]} {
        toplevel $base -class vTcl
        wm title $base "Visual Tcl Message"
        wm transient $base .essa
        frame $base.f -bd 2 -relief groove
        text $base.f.t -bd 0 -relief flat   -yscrollcommand " $base.f.ys set "
	$base.f.t insert 1.0 "$mesg"
	scrollbar $base.f.ys -command   "  $base.f.t yview " -orient vert
        frame $base.o -bd 1 -relief sunken
        foreach i $options {
            set n [string tolower $i]
            button $base.o.$n -text $i -width 5 \
		-command "
                destroy $base
		"
            pack $base.o.$n -side left -expand 1 -fill x
        }
        pack $base.f.t $base.f.ys -side left -expand 1 -fill both -ipadx 5 -ipady 5
        pack $base.f -side top -expand 1 -fill both -padx 2 -pady 2
        pack $base.o -side top -fill x -padx 2 -pady 2
    }
    wm withdraw $base
    update idletasks
    set w [winfo reqwidth $base]
    set h [winfo reqheight $base]
    set x [expr ($sw - $w)/2]
    set y [expr ($sh - $h)/2]
    
    wm geometry $base +$x+$y
    
    wm deiconify $base
    grab $base
    # tkwait window $base
    grab release $base
    #return $res
    #return "Oui"
}

proc info_bord { w } {
    global NOM
    global PWD
    global DIR
    gen_code $w 
    Dialog3 "[ exec $DIR/info.cl [pwd] $NOM  2>/dev/null]" 1
}

proc exe_prepro {  w i  } {
    global NOM
    global DIR 
    global PWD 
     if { [$w.modif cget -text ] != 0 } {
	 if { [ Dialog2 "some modifications not saved Save it" {Yes No} ] != "No"} {
	     Save $w;
	     if { [$w.modif cget -text ] == 0 }  {Dialog3 [ exec $DIR/exec_prepro2 [pwd] $NOM $i 2>/dev/null ] 2} 
	 } else {
	     if { [ Dialog2 "It is necessary to save modifications to ensure to have good boundaries conditions. Run whatever?" {Yes No } ] == "Yes" } {
		 Dialog3 [ exec $DIR/exec_prepro2 [pwd] $NOM $i 2>/dev/null ] 2 }
	 }
     } else { Dialog3 [ exec $DIR/exec_prepro2 [pwd] $NOM $i 2>/dev/null ]  2 }
}

proc exe_recup_geom { w } {
    global NOM
    global DIR 
    global PWD 
    Dialog2 [ exec $DIR/recupe_geom.ksh [pwd] $NOM  2>/dev/null ] 
}

proc Defaut {w } {
    global NOM
    global DIR
	global PWD
    exec cp $DIR/defaut.prep .
    readmodel [pwd]/defaut.prep $w
    Dialog2 "Select OK then:\n1)Modify maillagedefaut file to specify your geometry dimensions \n2)Specify all indexes matter in the viewlist for each block\n3)Modify nodes number nx,ny,nz at the top of the viewlist" {OK}
    exec $DIR/iediteur maillagedefaut &
}
proc Fichiers_supp { w } {
    global NOM
    global DIR
    set CAS  [lindex [split $NOM . ] 0]
    global listefichier
    set name $w.liste_fichiers_supplementaires
    if [ winfo exists $name ] { destroy $name }
    toplevel $name
    foreach file0  $listefichier {
	set file $file0$CAS
	frame $name.$file
	if [ file exists $file ] {
	    label $name.$file.t -text "File $file (exist)"
	} else {
	     label $name.$file.t -text "File $file (does not exist)"
	}
	
	button $name.$file.b -text "Edit $file" -command "Edit_f $w $file"
	pack $name.$file.t $name.$file.b -side left
	pack $name.$file
    }
    label $name.rem -text "In the case of modification dealing with infosup$CAS, it is necessary to save an read once the model"
    pack $name.rem 
}
set w .essa
set DIR [ exec dirname  [ info script ]]
puts "$DIR"
set PWD [ pwd ]
set listefichier [ list "maillage" "routinesupp" "infosupp" "noms_des_bords" ]
# initialisation (copie de fichiers)
puts  "[ exec $DIR/initprepro [pwd]  ]"
set f [ frame $w ]
pack $w
set testinfo 0

button .new -text "New" -command { test  New $w } 
pack .new
button .defaut -text "Default" -command {  test Defaut $w  }
pack .defaut
button .exem -text "Examples..." -command "Examples $w"
pack .exem
button .listeview -text "List" -command {viewlist $w}
pack .listeview

button .essai -text "Boundaries informations" -command { test2 info_bord $w }
button .gen_code -text "Code generation" -command { gen_code $w  }
button .exec -text "Modeling run..." -command { gen_code $w ; Dialog3 " [ exec $DIR/exec_model_linux2 0 [lindex [split $NOM . ] 0] 2>err  ]" mod0 }
button .execbis -text "Modeling run (FIELD)..." -command { gen_code $w ; Dialog3 " [ exec $DIR/exec_model_linux2 1  [lindex [split $NOM . ] 0] 2>err  ]" mod0 }
button .execsal -text "Translation OCC run..." -command { gen_code $w ; Dialog3 " [ exec $DIR/exec_model_linux2 2  [lindex [split $NOM . ] 0] 2>err  ]" mod0 }
button .meshtv -text "Pre-mesh visualization" -command { exec  $DIR/imeshtv model.lata 2>err & } 
button .exec2 -text "Pre-processing run..." -command { exe_prepro  $w 1 }
button .exec2b -text "Pre-processing run...\n(distant connectors)" -command { exe_prepro  $w 2 }

set types { {{Prepro Files} {*.prep}} {{All}       {*}} }
set typesf  { {{Fortran header} {*.h}} {{All}       {*}} }
button .save -text "Save file prepro" -command { Save $w  }

button .read -text "Read file prepro" -command {   if { [$w.modif cget -text ] != 0 } {if { [ Dialog2 "some modifications are not saved Save it" {Yes No} ] != "No"   } {Save $w } } ; \
    readmodel [tk_getOpenFile -defaultextension .prep   -initialdir [pwd] -filetypes $types]  $w }
button .readf -text "read file fortran " -command {  if { [$w.modif cget -text ] != 0 } {if { [ Dialog2 "some modifications are not saved Save it" {Yes No} ] != "No"   } {Save $w } } ; \
     readmodel  [tk_getOpenFile   -initialdir [pwd] -filetypes $typesf] $w  1}
button .exit -text "exit" -command { if { [$w.modif cget -text ] != 0 } {if { [ Dialog2 "some modifications are not saved Save it" {Yes No} ] != "No"   } {Save $w; if { [$w.modif cget -text ] == 0 }  {destroy . } } else { if { [ Dialog2 "Do you really want to quit ?" {Yes No } ] == "Yes" } {destroy . } } } else { destroy . } }


button .te -text "Added files" -command "Fichiers_supp $w"
button .triou -text "TRUST run ..." -command "Exec_trio $w"
button .nett -text "Make clean..." -command {if { [ Dialog2 "will remove all the files in model/avs in preproand all the files in model except files with extension .prep or starting by maillage" {Ok Cancel} ] != "Cancel"   } {  puts "[exec $DIR/nettoie [pwd]   2>/dev/null]" } } ;
button .recup_geom -text "Get geom" -command {  exe_recup_geom $w }  ;
#pack .save .read .readf .essai .gen_code .exec .execbis .exec2 .exec2b .te .exit
pack .save .read  .essai  .exec .execbis .execsal .meshtv .exec2 .exec2b .te .triou .nett .recup_geom .exit

bind all <Control-c> {destroy .}  

label $w.modif -text "0" 
global NOM
set NOM "nouveau.prep"
readinfo3 $w

#initilisation
frame $w.liste
label $w.liste.nbre -text "0"
label $w.liste.nnbre -text "0"
set listetaille $w.liste.n-1
frame $listetaille 
label $w.liste.cn-1 -text "n-1"
#puts "[$w.liste.cn-1 cget -text]"
label $listetaille.type -text "-1"
label  $listetaille.comm -text ""
label $listetaille.arg0 -text "0"
label $listetaille.arg1 -text "0"
label $listetaille.arg2 -text "0"

wm minsize . 1 1
#puts "nbre  [ $w.info.nbre cget -text ]"
#puts "essai [ $w.info.lutil get 0]"
if { $argc >= 1 } { readmodel [ lindex [ split $argv ] 0 ]  $w }
if { $argc >= 2 } { set arg  [ lindex [ split $argv ] 1 ] ;gen_code $w ;if {  $arg != "code" } { set mode 0; if {  $arg == "sal" } { set mode 2 };exec $DIR/exec_model_linux2 -no_xterm $mode [lindex [split $NOM . ] 0 ] 2>err; if {  $arg == "2" } {   exec $DIR/exec_prepro2 -no_xterm [pwd] $NOM 1 2>err    };   } }
#puts "ici $argc [ lindex [ split $argv ] 1 ]"
if { $argc == 2 } { exit }
#readmodel es.prep $w
#gen_code $w
viewlist $w

#puts "$NOM [split $NOM "." ]"

