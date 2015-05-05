#############################################################################
# Visual Tcl v1.10 Project
#

#################################
# GLOBAL VARIABLES
#
global widget; 
    set widget(cells) {.top17.fra27.cpd28.03}
    set widget(colNames) {.top17.fra27.fra21.can22}
    set widget(colNamesFrame) {.top17.fra27.fra21}
    set widget(infor) {.top17.fra27.fra18.fra24.lab17}
    set widget(lineNames) {.top17.fra27.fra18.can25}
    set widget(lineNamesFrame) {.top17.fra27.fra18}
    set widget(menuAide) {.top17.fra22.men17}
    set widget(menuFaces) {.top17.fra22.men24}
    set widget(menuFacteurs) {.top17.fra22.men25}
    set widget(menuFichiers) {.top17.fra22.men23}
    set widget(rev,.top17) {toplevel}
    set widget(rev,.top17.fra22.men17) {menuAide}
    set widget(rev,.top17.fra22.men23) {menuFichiers}
    set widget(rev,.top17.fra22.men24) {menuFaces}
    set widget(rev,.top17.fra22.men25) {menuFacteurs}
    set widget(rev,.top17.fra27.cpd28.01) {xScrollbar}
    set widget(rev,.top17.fra27.cpd28.02) {yScrollbar}
    set widget(rev,.top17.fra27.cpd28.03) {cells}
    set widget(rev,.top17.fra27.fra18) {lineNamesFrame}
    set widget(rev,.top17.fra27.fra18.can22) {colNames}
    set widget(rev,.top17.fra27.fra18.can25) {lineNames}
    set widget(rev,.top17.fra27.fra18.fra24.lab17) {infor}
    set widget(rev,.top17.fra27.fra21) {colNamesFrame}
    set widget(rev,.top17.fra27.fra21.can22) {colNames}
    set widget(toplevel) {.top17}
    set widget(xScrollbar) {.top17.fra27.cpd28.01}
    set widget(yScrollbar) {.top17.fra27.cpd28.02}

#################################
# USER DEFINED PROCEDURES
#
proc init {argc argv} {

}

init $argc $argv


proc main {argc argv} {

}

proc Window {args} {
global vTcl
    set cmd [lindex $args 0]
    set name [lindex $args 1]
    set newname [lindex $args 2]
    set rest [lrange $args 3 end]
    if {$name == "" || $cmd == ""} {return}
    if {$newname == ""} {
        set newname $name
    }
    set exists [winfo exists $newname]
    switch $cmd {
        show {
            if {$exists == "1" && $name != "."} {wm deiconify $name; return}
            if {[info procs vTclWindow(pre)$name] != ""} {
                eval "vTclWindow(pre)$name $newname $rest"
            }
            if {[info procs vTclWindow$name] != ""} {
                eval "vTclWindow$name $newname $rest"
            }
            if {[info procs vTclWindow(post)$name] != ""} {
                eval "vTclWindow(post)$name $newname $rest"
            }
        }
        hide    { if $exists {wm withdraw $newname; return} }
        iconify { if $exists {wm iconify $newname; return} }
        destroy { if $exists {destroy $newname; return} }
    }
}

#################################
# VTCL GENERATED GUI PROCEDURES
#

proc vTclWindow. {base} {
    if {$base == ""} {
        set base .
    }
    ###################
    # CREATING WIDGETS
    ###################
    wm focusmodel $base passive
    wm geometry $base 1x1+0+0
    wm maxsize $base 1265 994
    wm minsize $base 1 1
    wm overrideredirect $base 1
    wm resizable $base 1 1
    wm withdraw $base
    wm title $base "vt.tcl"
    ###################
    # SETTING GEOMETRY
    ###################
}

proc vTclWindow.top17 {base} {
    if {$base == ""} {
        set base .top17
    }
    if {[winfo exists $base]} {
        wm deiconify $base; return
    }
    ###################
    # CREATING WIDGETS
    ###################
    toplevel $base -class Toplevel
    wm focusmodel $base passive
    wm geometry $base 450x350+200+200
    wm maxsize $base 1024 860
    wm minsize $base 1 1
    wm overrideredirect $base 0
    wm resizable $base 1 1
    wm sizefrom $base user
    wm deiconify $base
    wm title $base "Define view factors"
    frame $base.fra22 \
        -borderwidth 2 -height 37 -relief raised -width 677 
    menubutton $base.fra22.men23 \
        -menu .top17.fra22.men23.m -padx 5 -pady 4 -text Fichiers 
    menu $base.fra22.men23.m \
        -cursor {} 
    $base.fra22.men23.m add command \
        -accelerator Ctrl+o -command Pb:Ouvrir -label Ouvrir 
    $base.fra22.men23.m add command \
        -accelerator Ctrl+e -command {Pb:Enregistrer 0} -label Enregistrer 
    $base.fra22.men23.m add command \
        -command Pb:Enregistrer_sous -label {Enregistrer sous ...} 
    $base.fra22.men23.m add separator
    $base.fra22.men23.m add command \
        -accelerator Ctrl+c -command Pb:Quitter -label Quitter 
    menubutton $base.fra22.men24 \
        -menu .top17.fra22.men24.m -padx 5 -pady 4 -text {Faces rayonnantes} 
    menu $base.fra22.men24.m \
        -cursor {} 
    $base.fra22.men24.m add command \
        -accelerator Ctrl+V -command {Pb:Modifier_Surfaces 0} \
        -label {Voir Surfaces & Emissivites} 
    $base.fra22.men24.m add separator
    $base.fra22.men24.m add command \
        -accelerator Ctrl+S -command {Pb:Modifier_Surfaces 1} \
        -label {Modifier Surfaces} 
    $base.fra22.men24.m add command \
        -accelerator Ctrl+E -command {Pb:Modifier_Surfaces 2} \
        -label {Modifier Emissivites} 
    menubutton $base.fra22.men25 \
        -menu .top17.fra22.men25.m -padx 5 -pady 4 -text {Facteurs de forme} 
    menu $base.fra22.men25.m \
        -cursor {} 
    $base.fra22.men25.m add command \
        -accelerator F2 -command {
	    InitWarnings
	    Pb:Verifier
	} -label Verifier 
    $base.fra22.men25.m add command \
        -accelerator F3 \
        -command {
	    InitWarnings
	    if {[Pb:Verifier_Donnees]} {
		Pb:Symetriser
	    }
	} \
        -label Symetriser 
    $base.fra22.men25.m add command \
        -accelerator F4 \
        -command {
	    InitWarnings
	    if {[Pb:Verifier_Donnees]} {
		Pb:Resoudre
	    }
	} \
        -label {Resoudre xxx} 
    $base.fra22.men25.m add command \
        -accelerator F5 \
        -command {
	    puts "tot"
	   
	    Pb:Corriger
	   
	    
	} \
        -label {Corriger xxx} 
    $base.fra22.men25.m add separator
    $base.fra22.men25.m add command \
        -accelerator F6 \
        -command {   
	    InitWarnings
	    Pb:Calc_ANSYS
	} \
        -label {Calculer (ANSYS)} 
    $base.fra22.men25.m add command \
        -accelerator F7 \
        -command {	   
	    InitWarnings
	    Pb:Opt_ANSYS
	} \
        -label {Options (ANSYS)} 
    $base.fra22.men25.m add separator
    $base.fra22.men25.m add command \
        -accelerator Ctrl+i -command {
	
	    Pb:initialiser
	} -label Initialiser 
    menubutton $base.fra22.men17 \
        -menu .top17.fra22.men17.m -padx 5 -pady 4 -text Aide 
    menu $base.fra22.men17.m \
        -cursor {} 
    $base.fra22.men17.m add command \
        -accelerator F1 -command Aide:Index -label Index 
    $base.fra22.men17.m add command \
        -accelerator Ctrl+h -command Aide:Rechercher -label Rechercher 
    $base.fra22.men17.m add command \
        -accelerator Ctrl+x -command Aide:Fermer_Aides -label {Fermer Aides} 
    $base.fra22.men17.m add separator
    $base.fra22.men17.m add command \
        -command {Aide:Aff_Fich -1} -label {A propos} 
    frame $base.fra27 \
        -borderwidth 2 -height 75 -width 125 
    frame $base.fra27.fra18 \
        -borderwidth 2 -height 140 -width 8 
    frame $base.fra27.fra18.fra24 \
        -borderwidth 2 -height 31 -width 63 
    canvas $base.fra27.fra18.can25 \
        -borderwidth 2 -height 241 -relief ridge -width 60 
    frame $base.fra27.fra18.fra26 \
        -borderwidth 2 -height 13 -width 37 
    frame $base.fra27.fra21 \
        -borderwidth 2 -height 24 -width 125 
    canvas $base.fra27.fra21.can22 \
        -borderwidth 2 -height 22 -relief ridge -width 352 
    frame $base.fra27.fra21.fra23 \
        -borderwidth 2 -height 22 -width 13 
    frame $base.fra27.cpd28 \
        -borderwidth 1 -height 30 -width 30 
    scrollbar $base.fra27.cpd28.01 \
        -borderwidth 1 -command {$base.fra27.cpd28.03 xview} -orient horiz \
        -width 15
    scrollbar $base.fra27.cpd28.02 \
        -borderwidth 1 -command {$base.fra27.cpd28.03 yview} -orient vert \
        -width 15
    canvas $base.fra27.cpd28.03 \
        -borderwidth 2 -cursor fleur -height 100 -relief ridge -width 100 \
        -xscrollcommand {.top17.fra27.cpd28.01 set} \
        -yscrollcommand {.top17.fra27.cpd28.02 set} 
    ###################
    # SETTING GEOMETRY
    ###################
    pack $base.fra22 \
        -in .top17 -anchor center -expand 0 -fill x -side top 
    pack $base.fra22.men23 \
        -in .top17.fra22 -anchor center -expand 0 -fill none -side left 
    pack $base.fra22.men24 \
        -in .top17.fra22 -anchor center -expand 0 -fill none -side left 
    pack $base.fra22.men25 \
        -in .top17.fra22 -anchor center -expand 0 -fill none -side left 
    pack $base.fra22.men17 \
        -in .top17.fra22 -anchor center -expand 0 -fill none -side right 
    pack $base.fra27 \
        -in .top17 -anchor center -expand 1 -fill both -side top 
    pack $base.fra27.fra18 \
        -in .top17.fra27 -anchor ne -expand 0 -fill y -side left 
    pack $base.fra27.fra18.fra24 \
        -in .top17.fra27.fra18 -anchor center -expand 0 -fill none -side top 
    pack $base.fra27.fra18.can25 \
        -in .top17.fra27.fra18 -anchor center -expand 1 -fill both -side top 
    pack $base.fra27.fra18.fra26 \
        -in .top17.fra27.fra18 -anchor center -expand 0 -fill none -side top 
    pack $base.fra27.fra21 \
        -in .top17.fra27 -anchor center -expand 0 -fill x -side top 
    pack $base.fra27.fra21.can22 \
        -in .top17.fra27.fra21 -anchor center -expand 1 -fill x -side left 
    pack $base.fra27.fra21.fra23 \
        -in .top17.fra27.fra21 -anchor center -expand 0 -fill none -side left 
    pack $base.fra27.cpd28 \
        -in .top17.fra27 -anchor center -expand 1 -fill both -side top 
    grid columnconf $base.fra27.cpd28 0 -weight 1
    grid rowconf $base.fra27.cpd28 0 -weight 1
    grid $base.fra27.cpd28.01 \
        -in .top17.fra27.cpd28 -column 0 -row 1 -columnspan 1 -rowspan 1 \
        -sticky ew 
    grid $base.fra27.cpd28.02 \
        -in .top17.fra27.cpd28 -column 1 -row 0 -columnspan 1 -rowspan 1 \
        -sticky ns 
    grid $base.fra27.cpd28.03 \
        -in .top17.fra27.cpd28 -column 0 -row 0 -columnspan 1 -rowspan 1 \
        -sticky nesw 
}

Window show .
Window show .top17

main $argc $argv
