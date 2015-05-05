#!/usr/bin/wish -f
##########################################################################
#
# Éditeur graphique de facteurs de forme pour le module rayonnement de
# TRUST.
#
# T. Cornu - G. Etievent, 
# 1998 SIMULOG
# $Id: flowcharteditor.aux.tcl,v 1.49 1998/09/24 11:36:50 cesame Exp $
###

##########################################################################
# 
# Initialisation de la vue graphique 
#
##########################################################################

# Chargement de la definition de l'IHM

source [file join [file dirname [info script]] trio_rayo_ihm.tcl]
source [file join [file dirname [info script]] balloon.tcl]
source [file join [file dirname [info script]] opts.tcl]
init_balloons -delay 500 -color cyan

bind .top17 <Control-e> {$widget(menuFichiers).m invoke 2}
bind .top17 <Control-o> {$widget(menuFichiers).m invoke 1}
bind .top17 <Control-c> {$widget(menuFichiers).m invoke 5}

bind .top17 <Control-V> {$widget(menuFaces).m invoke 1}
bind .top17 <Control-S> {$widget(menuFaces).m invoke 3}
bind .top17 <Control-E> {$widget(menuFaces).m invoke 4}

bind .top17 <F2> {$widget(menuFacteurs).m invoke 1}
bind .top17 <F3> {$widget(menuFacteurs).m invoke 2}
bind .top17 <F4> {$widget(menuFacteurs).m invoke 3}
bind .top17 <F5> {$widget(menuFacteurs).m invoke 4}
bind .top17 <F5> {$widget(menuFacteurs).m invoke 4}
bind .top17 <F6> {$widget(menuFacteurs).m invoke 6}
bind .top17 <F7> {$widget(menuFacteurs).m invoke 7}
bind .top17 <Control-i> {$widget(menuFacteurs).m invoke 8}

bind .top17 <F1> {$widget(menuAide).m invoke 1}
bind .top17 <Control-h> {$widget(menuAide).m invoke 2}
bind .top17 <Control-x> {$widget(menuAide).m invoke 3}

# parametres principaux

global cellWidth cellHeight dimension


#     tailles en pixels :
set cellWidth 50
set cellHeight 20

#------------------------------------------------------------------------
# xScroll --
#
# Scrolle les deux fenetres cellules et noms de colonnes du meme
# deplacement en X

proc xScroll {args} {
    global widget cellWidth  cellHeight

    eval [concat $widget(cells)    xview $args]
    eval [concat $widget(colNames) xview $args]
}

#------------------------------------------------------------------------
# yScroll --
#
# Scrolle les deux fenetres cellules et noms de lignes du meme 
# depalcement en Y

proc yScroll {args} {
    global widget cellWidth  cellHeight

    eval [concat  $widget(cells)    yview $args]
    eval [concat $widget(lineNames) yview $args] 
}

#------------------------------------------------------------------------
# InitWarnings --
#
# Initialise la fenetre d'affichage des warnings

proc InitWarnings {} {
    global Pbinit
    if {$Pbinit==0} {
    } else {
	if {[winfo exists .warnings]} {
	    .warnings.corps.texte configure -state normal
	    .warnings.corps.texte delete 1.0 end
	    .warnings.corps.texte configure -state disabled
	    wm deiconify .warnings
	} else {
	    toplevel .warnings
	    wm resizable .warnings 1 1
	    wm minsize .warnings 310 200
	    wm title .warnings "Messages"

	    label .warnings.titre -text "Messages lors de la verification/Resolution"

	    frame .warnings.corps
	    scrollbar .warnings.corps.scroll -command ".warnings.corps.texte yview" \
		-orient vert -width 15
	    text .warnings.corps.texte -borderwidth 2 -relief sunken -wrap word\
		-yscrollcommand ".warnings.corps.scroll set" -width 40 -height 10 -bg white
	    .warnings.corps.texte tag configure bug -foreground red

	    pack .warnings.corps.texte  -side left -expand 1 -fill both
	    pack .warnings.corps.scroll -side left -fill y

	    button .warnings.fermer -text Fermer -command {destroy .warnings}

	    .warnings.corps.texte configure -state normal
	    .warnings.corps.texte delete 1.0 end
	    .warnings.corps.texte configure -state disabled

	    pack .warnings.titre -side top
	    pack .warnings.corps -side top -expand 1 -fill both
	    pack .warnings.fermer -side top
	}
    }
}

#------------------------------------------------------------------------
# InitWarnings --
#
# Insert le message msg dans la fenetre d'affichage des warnings

proc InsertWarning {msg drap} {
    if {[winfo exists .warnings]} {
	.warnings.corps.texte configure -state normal
	.warnings.corps.texte insert end "$msg" $drap
	.warnings.corps.texte insert end "\n---------------------------\n"
	.warnings.corps.texte configure -state disabled
    } else {
	InitWarnings
	.warnings.corps.texte configure -state normal
	.warnings.corps.texte insert end "$msg" $drap
	.warnings.corps.texte insert end "\n---------------------------\n"
	.warnings.corps.texte configure -state disabled
    }
}

#------------------------------------------------------------------------
# PbInit --
#
# Affiche message : pas de cas en memoire

proc PbInit {} {
    toplevel .pbinit
    wm resizable .pbinit 0 0
    wm title .pbinit "Attention"
     wm geometry .pbinit [wm geometry .top17]
    grab .pbinit
    focus .pbinit

    label .pbinit.text -text "ATTENTION\n\nAucun cas n'a ete mis en memoire.\nCommencez par ouvrir un fichier dans Fichiers/Ouvrir." -anchor center
    button .pbinit.ok -text Ok -command {destroy .pbinit}
    pack .pbinit.text .pbinit.ok -side top -expand 1
}


#------------------------------------------------------------------------
# initCells --
#
# Initialise le tableau de cellules

proc initCells {} {
    global widget cellWidth cellHeight PbnomProbleme \
	PbnomFace Pbsurface Pbfactform Pbnb_faces PbmodifieP help_tips

    # creation des cellules

    for {set i 0} {$i < $Pbnb_faces} {incr i} {
	for {set j 0} {$j < $Pbnb_faces} {incr j} {
	    set theCellWidget $widget(cells).$i-$j
	    entry $theCellWidget -bg white
	    $theCellWidget insert 0 \
		$Pbfactform([lindex $PbnomFace $i],[lindex $PbnomFace $j])
	    $widget(cells) create window \
		[expr $i * $cellWidth] [expr $j * $cellHeight] \
		-anchor nw \
		-height $cellHeight \
		-width $cellWidth \
		-window $theCellWidget \
		-tags cellule
	    bind $theCellWidget <Any-Key> {
		set PbmodifieP 1
	    }
	    bind $theCellWidget <Return> {
		set indj [string range %W 22 end]
		set indi [string range $indj 0 [expr [string first "-" $indj]-1] ]
		set indj [string range $indj [expr [string first "-" $indj]+1] end]
		
		InitWarnings
		if {[Pb:Verifier_Donnees]} {
		    Pb:Symetriser_ij $indi $indj
		}
	    }

# 	    if {$i<=$j} {
# 		$theCellWidget configure -state disabled -bg grey
# 	    } else {
# 		$theCellWidget configure -bg white
# 	    }
	}
    }

    # creation des noms de colonnes

    for {set i 0} {$i < $Pbnb_faces} {incr i} {
	set theColNameWidget $widget(colNames).$i
	button $theColNameWidget -text [lindex $PbnomFace $i]\
	    -borderwidth 2 -relief raised -state disabled -disabledforeground black
	$widget(colNames) create window \
	    [expr $i * $cellWidth] 0 \
	    -anchor nw \
	    -height [$widget(colNames) cget -height] \
	    -width $cellWidth \
	    -window $theColNameWidget \
	    -tags colonne
	set nom [lindex $PbnomFace [string range [file extension $theColNameWidget] 1 end]]
 	set help_tips($theColNameWidget) "$nom $Pbsurface($nom)"
    }
    
    # creation des noms de lignes

    for {set j 0} {$j < $Pbnb_faces} {incr j} {
	set theLineNameWidget $widget(lineNames).$j
	button $theLineNameWidget -text [lindex $PbnomFace $j]\
	    -borderwidth 2 -relief raised -state disabled -disabledforeground black
	$widget(lineNames) create window \
	    0 [expr $j * $cellHeight] \
	    -anchor nw \
	    -width [$widget(lineNames) cget -width] \
	    -height $cellHeight \
	    -window $theLineNameWidget \
	    -tags ligne
	set nom "[lindex $PbnomFace [string range [file extension $theLineNameWidget] 1 end]]"
 	set help_tips($theLineNameWidget) "$nom $Pbsurface($nom)"
    }

    # reconfiguration des commandes de scrolling pour deplacer
    # simultanement le canvas des cellules et le canvas superieur
    # (resp. le canvas gauche).

    $widget(xScrollbar) configure -command xScroll
    $widget(yScrollbar) configure -command yScroll

    # configuration du canvas des cellules pour le scrolling
    $widget(cells) configure -scrollregion \
	[list \
	     0 0 \
	     [expr $Pbnb_faces * $cellWidth] [expr $Pbnb_faces * $cellHeight]]
    $widget(cells) configure -xscrollincrement $cellWidth
    $widget(cells) configure -yscrollincrement $cellHeight

    # configuration du canvas du bord superieur pour le scrolling

    $widget(colNames) configure -scrollregion \
	[list \
	     0 0 \
	     [expr $Pbnb_faces * $cellWidth] [$widget(colNames) cget -height]]
    $widget(colNames) configure -xscrollincrement $cellWidth

    # configuration du canvas du bord gauche pour le scrolling

    $widget(lineNames) configure -scrollregion \
	[list \
	     0 0 \
	     [$widget(lineNames) cget -width] [expr $Pbnb_faces * $cellHeight]]
    $widget(lineNames) configure -yscrollincrement $cellHeight
}


##########################################################################
#
# Structures de donnees pour un probleme avec du rayonnement
#
# Variables contenant les informations :
#
# PbnbFaces		nombres effectif de faces rayonnantes
# PbnomProbleme	          nom des fichiers contenant le jeu de donnees
# PbnomFace(N)		tableau contenant les noms des faces rayonnantes
# Pbsurface(N)		tableau contenant les surfaces des faces
# Pbemissivite(N)		tableau contenant l'emissivite des surfaces
# Pbfactform(N,N)	         tableau contenant les factforms
# PbmodifieP		booleen indiquant si les donnees sont en cours 
#			de modification (elles necessitent alors une 
#			sauvegarde sur disque).
##########################################################################


#------------------------------------------------------------------------
# Pb:initialiser --
#
# Reinitialise la memoire avec des donnees vides pour un probleme.

proc Pb:initialiser {} {
    global PbnbFaces PbmodifieP PbmodifieS PbnomProbleme PbnomFace \
	Pbsurface Pbfactform PbPath widget methode2D methode3D nb_zones_2D_ANSYS \
	dec_ang_ANSYS graph_ANSYS nb_zones_3D_ANSYS methode2D_defaut methode3D \
	nb_zones_2D_ANSYS_defaut nb_zones_3D_ANSYS_defaut dec_ang_ANSYS_defaut \
	graph_ANSYS_defaut

# initialisation des options par defaut ANSYS
    set methode2D_defaut 0
    set methode3D_defaut 2
    set nb_zones_2D_ANSYS_defaut 200
    set nb_zones_3D_ANSYS_defaut 20    
    set dec_ang_ANSYS_defaut 20
    set graph_ANSYS_defaut 0
# initialisation des options aux valeurs par defaut
    if { [catch {expr $methode2D} ]} {
	set methode2D $methode2D_defaut 
    }
    if { [catch {expr $mehtode3D} ]} {
	set methode3D $methode3D_defaut 
    }
    if { [catch {expr $nb_zones_2D_ANSYS} ]} {
	set nb_zones_2D_ANSYS $nb_zones_2D_ANSYS_defaut
    }
    if { [catch {expr $nb_zones_3D_ANSYS} ]} {
	set nb_zones_3D_ANSYS $nb_zones_3D_ANSYS_defaut
    }
    if { [catch {expr $dec_ang_ANSYS} ]} {
	set dec_ang_ANSYS $dec_ang_ANSYS_defaut
    }
    if { [catch {expr $graph_ANSYS} ]} {
	set graph_ANSYS $graph_ANSYS_defaut
    }

#effacement cellules si existent (pour nouveau fichier)
    set liste [$widget(cells) find withtag "cellule"]
    if {$liste!={}} {
	foreach el $liste {
	    set entree [$widget(cells) itemcget $el -window]
	    destroy $entree
	}
	$widget(cells) delete cellule
    }

#effacement colonnes si existent (pour nouveau fichier)
    set liste [$widget(colNames) find withtag "colonne"]
    if {$liste!={}} {
	foreach el $liste {
	    set bouton [$widget(colNames) itemcget $el -window]
	    destroy $bouton
	}
	$widget(colNames) delete colonne
    }

#effacement lignes si existent (pour nouveau fichier)
    set liste [$widget(lineNames) find withtag "ligne"]
    if {$liste!={}} {
	foreach el $liste {
	    set bouton [$widget(lineNames) itemcget $el -window]
	    destroy $bouton
	}
	$widget(lineNames) delete ligne
    }

    set PbnbFaces 0
    set PbmodifieP 0
    set PbmodifieS 0
    set PbnomProbleme "essai"
    wm title .top17 "Saisie des facteurs de forme : $PbnomProbleme"
    set PbnomFace {}
    if {[info exists Pbsurface]} {
	unset Pbsurface
    }
    if {[info exists Pbfactform]} {
	unset Pbfactform
    }
}

#------------------------------------------------------------------------
# Pb:ReInitFacteursr --
#
# Reinitialise les entrees a 0.0 et xxx.

proc Pb:ReInitFacteurs {} {
    global Pbnb_faces widget PbmodifieP Pbinit

    if {$Pbinit==0} {
	PbInit
    } else {
	toplevel .attention
	wm resizable .attention 0 0
	wm title .attention "Attention"
	grab .attention
	focus .attention
	label .attention.titre -text "ATTENTION :\n\nCela va effacer le contenu de la matrice en cours !"
	frame .attention.boutons
	pack .attention.titre .attention.boutons -side top -pady 10p

	button .attention.boutons.ok -text "Continuer" -command {
	    for {set i 0} {$i<$Pbnb_faces} {incr i} {
		for {set j 0} {$j<$Pbnb_faces} {incr j} {
		    $widget(cells).$i-$j delete 0 end
		    $widget(cells).$i-$j insert 0 "xxx"
		}
	    }
	    set PbmodifieP 1
	    destroy .attention
	}
	button .attention.boutons.annuler -text "Annuler" -command {destroy .attention}
	pack .attention.boutons.ok .attention.boutons.annuler -side left
    }
}

#------------------------------------------------------------------------
# Pb:Quitter --
#
# Quitte l'application.

proc Pb:Quitter {} {
    global PbmodifieP PbmodifieS
    if {$PbmodifieP==1 || $PbmodifieS==1} {
	toplevel .attention
	wm resizable .attention 0 0
	wm title .attention "Attention : cas non sauvegarde"
	grab .attention
	focus .attention
	
	label .attention.titre -text "ATTENTION\n\nLe cas en cours n'a pas ete sauvegarde !\n\nVoulez-vous"
	frame .attention.boutons
	pack .attention.titre .attention.boutons -side top -pady 10p

	button .attention.boutons.s_and_q -text "Sauvegarder\net Quitter" -command {
	    if {[Pb:Enregistrer 0]==1} {
		exit
	    }
	}
	button .attention.boutons.quit -text "Quitter\nsans sauvegarder" -command exit
	button .attention.boutons.annuler -text "Annuler" -command {destroy .attention}
	pack .attention.boutons.s_and_q .attention.boutons.quit .attention.boutons.annuler -side left -padx 5p
    } else {
	toplevel .quitter
	wm resizable .quitter 0 0
	wm title .quitter "Quitter ?"
	grab .quitter
	focus .quitter

	label .quitter.titre -text "Etes-vous sur de vouloir quitter l'application ?"
	frame .quitter.boutons
	pack .quitter.titre .quitter.boutons -side top -pady 10p
	button .quitter.boutons.quit -text "Oui" -command exit
	button .quitter.boutons.annuler -text "Annuler" -command {destroy .quitter}
	pack .quitter.boutons.quit .quitter.boutons.annuler -side left -padx 5p
    }
}

#------------------------------------------------------------------------
# Pb:Enregistrer --
#
# Sauvegarde des donnees en memoire dans un jeu de fichiers. 
# L'argument est le nom des fichiers correspondants.

proc Pb:Enregistrer {filename} {
    global PbPath PbmodifieP PbmodifieS PbnomProbleme Pbinit \
	PbnomFace Pbsurface Pbemissivite Pbfactform Pbnb_faces widget

    set enreg_ok 0

    if {$Pbinit==0} {
	PbInit
    } else {
	set enreg_ok [Pb:Mise_a_Jour]

#  	if {$enreg_ok==1} {
	    if {$filename==0} {
		set filename "$PbPath/$PbnomProbleme"
	    } else {
		set filename "$PbPath/$filename"
	    }
	    
	    if {$PbmodifieS==1} {
		set filenam "$filename"
		append filenam ".facesrayo"
		set f [open $filenam w]
		set NbFacesRayo 0
		# correction >0 ->=0.
		# car maintenat emissivite=0 correct
		foreach el $PbnomFace {
		    if {[expr $Pbemissivite($el)>=0.]} {
			incr NbFacesRayo
		    }
		}
		puts $f "$Pbnb_faces $NbFacesRayo"
		foreach el $PbnomFace {
		    puts $f "$el $Pbsurface($el) $Pbemissivite($el)"
		}
		close $f
		set PbmodifieS 0
	    }
	    if {$PbmodifieP==1} {
		set filenam "$filename"
		if {$enreg_ok==1} {
		    append filenam ".factforme"
		} else {
		    append filenam ".factforme_bak"
		}
		set f [open $filenam w]
		puts $f $Pbnb_faces
		if {$enreg_ok==1} {
		    foreach el1 $PbnomFace {
			set ligne {}
			foreach el2 $PbnomFace {
			    lappend ligne $Pbfactform($el2,$el1)
			}
			puts $f $ligne
		    }
		} else {
		    for {set i 0} {$i < $Pbnb_faces} {incr i} {
			set ligne {}
			for {set j 0} {$j < $Pbnb_faces} {incr j} {
			    lappend ligne [$widget(cells).$j-$i get]
			}
			puts $f $ligne
		    }
		}
		close $f
		set PbmodifieP 0
	    }
# 	}
    }
    return $enreg_ok
}

#------------------------------------------------------------------------
# Pb:Enregistrer_sous --
#
# Sauvegarde des donnees en memoire dans un jeu de fichiers. 
# L'argument est le nom des fichiers correspondants.

proc Pb:Enregistrer_sous {} {
    global PbnomProbleme Pbinit PbPath PbnomProbleme widget \
	PbmodifieP PbmodifieS

    if {$Pbinit==0} {
	PbInit
    } else {
	set types {{{Fichiers Faces Rayonnantes} {.facesrayo}} \
		       {{Tous} {*}}}
	set filename [tk_getSaveFile -defaultextension ".facesrayo" \
			  -initialdir $PbPath \
			  -initialfile $PbnomProbleme \
			  -parent $widget(cells) -filetypes $types]
	if {$filename != ""} {
	    set PbPath [file dirname $filename]
	    set PbnomProbleme [file tail [file rootname $filename]]
	    wm title .top17 \
		"Saisie des facteurs de forme : $PbnomProbleme"
	    set PbmodifieP 1
	    set PbmodifieS 1
	    Pb:Enregistrer $PbnomProbleme
	}
    }
}

#------------------------------------------------------------------------
# Pb:ouvrir --
#
# Lecture en memoire d'un jeu de donnees sur disque
# L'argument est le nom des fichiers correspondants.

proc Pb:Ouvrir {} {
    global widget PbnomProbleme PbnomFace Pbsurface \
	PbPath Pbnb_faces Pbfactform
   
    set types {{{Fichiers Faces Rayonnantes} {.facesrayo}} \
	{{Fichiers Donnees TRUST} {.data .geo .info}} {{Tous} {*}}}
    set filename [tk_getOpenFile -defaultextension ".facesrayo" \
                      -initialdir $PbPath -initialfile $PbnomProbleme \
                      -parent $widget(cells) -filetypes $types]
    if {$filename != ""} {
	set PbPath [file dir $filename]
	set filename [file tail $filename]
	Pb:Lit_Fichier "$filename"
    }
}

#------------------------------------------------------------------------
# Est_Reel --
#
# verifie si on a une liste de reels.

proc Est_Reel {liste} {

    set res 1
    foreach el $liste {
	if {[catch {expr $el}]} {
	    set res 0
	}
    }

    return $res
}

#------------------------------------------------------------------------
# Calculer_Surface --
#
# calcule la surface d'une ligne du fichier .data. pour cas 2D

proc Calculer_Surface2D {ligne} {

    set ok [expr [llength $ligne]==8]
    set surf 0.

    if {$ok} {
	set liste [list [lindex $ligne 2] [lindex $ligne 3] [lindex $ligne 7]]
	set ok [expr ![string compare [lindex $ligne 1] "="] &&\
		    ![string compare [lindex $ligne 4] "<="] && \
		    ![string compare [lindex $ligne 6] "<="]]
	set ok2 [Est_Reel $liste]
	if {$ok2 && $ok} {
	    set l [expr [lindex $ligne 7]-[lindex $ligne 3]]
	    if {[expr $l>0]} {
		set surf $l
	    } else {
		set surf "Coefficients non ordonnes (dimensions =$l)"
	    }
	} else {
	    if {$ok2} {
	    set surf "Signes non conformes"
	    } else {
		set surf "Elements non reels"
	    }
	}
    } else {
	set surf "Definition de bord non conforme\n(nb elements [llength $ligne], au lieu de 8)"
    }

    return $surf
}
# calcule la surface d'une ligne du fichier .data. pour cas 3D

proc Calculer_Surface3D {ligne} {

    set ok [expr [llength $ligne]==13]
    set surf 0.

    if {$ok} {
	set liste [list [lindex $ligne 2] [lindex $ligne 3] [lindex $ligne 7]\
		       [lindex $ligne 8] [lindex $ligne 12]]
	set ok [expr ![string compare [lindex $ligne 1] "="] &&\
		    ![string compare [lindex $ligne 4] "<="] && \
		    ![string compare [lindex $ligne 6] "<="] && \
		    ![string compare [lindex $ligne 9] "<="] && \
		    ![string compare [lindex $ligne 11] "<="]]
	set ok2 [Est_Reel $liste]
	if {$ok2 && $ok} {
	    set l1 [expr [lindex $ligne 7]-[lindex $ligne 3]]
	    set l2 [expr [lindex $ligne 12]-[lindex $ligne 8]]
	    if {[expr $l1>0] && [expr $l2>0]} {
		set surf [expr $l1*$l2]
	    } else {
		set surf "Coefficients non ordonnes (dimensions =$l1 x $l2)"
	    }
	} else {
	    if {$ok2} {
	    set surf "Signes non conformes"
	    } else {
		set surf "Elements non reels"
	    }
	}
    } else {
	set surf "Definition de bord non conforme\n(nb elements [llength $ligne], au lieu de 13)"
    }

    return $surf
}

#------------------------------------------------------------------------
# Pb:Lit_Fichier --
#
# lit le fichier filename sur le disque.

proc Pb:Lit_Fichier {filename} {
    global PbPath PbnomProbleme PbnomFace Pbsurface Pbemissivite \
	Pbnb_faces Pbfactform Pbinit PbmodifieP PbmodifieS dimension

    Pb:initialiser
    set Pbinit 1

    set PbmodifieP 0
    set PbnomProbleme [file tail [file rootname $filename]]
    wm title .top17 \
	"Saisie des facteurs de forme : $PbnomProbleme"

    if {[file extension $filename]==".data" || [file extension $filename]==".geo"} {
	set init 0
	set Pbnb_faces 0
	set f [open "$PbPath/$filename" r]
	while {![eof $f]} {
	    gets $f ligne
	    if {[string first "\{" $ligne]!=-1 || [string first "\}" $ligne]!=-1} {
		set ligne ""
	    }
	    if {[lsearch [string tolower $ligne] "dimension"]==0} {
		set dim [lindex $ligne 1]
		if {$dim!=3 && $dim!=2} {
		    toplevel .pbouv
		    wm resizable .pbouv 0 0
		    wm title .pbouv "Attention"
		    focus .pbouv
		    label .pbouv.text -text "ATTENTION\n\nCette interface travaille sur des cas pour le rayonnement\n\nen DEUX ou TROIS dimensions.\n"
		    button .pbouv.ok -text Ok -command {destroy .pbouv}
		    pack .pbouv.text .pbouv.ok -side top
		    set nom -1
		    break
		}
		set dimension $dim
	    }
	    if {[lsearch [string tolower $ligne] "bord"]==0} {
		set nom [lindex $ligne 1]
		if ($dimension==2) {
		    set surf [Calculer_Surface2D [lreplace $ligne 0 1]]
		} else {
		    set surf [Calculer_Surface3D [lreplace $ligne 0 1]]
		}
		if {[lsearch $PbnomFace $nom]==-1 && ![catch {expr $surf}]} {
		    lappend PbnomFace $nom
		    set Pbsurface($nom) $surf
		    set Pbemissivite($nom) 1.
		    incr Pbnb_faces
		} else {
		    toplevel .pbouv
		    wm resizable .pbouv 0 0
		    wm title .pbouv "Attention"
		    focus .pbouv
		    label .pbouv.text
		    if {[catch {expr $surf}]} {
			.pbouv.text configure -text \
			    "ATTENTION\n\nLe bord $nom est mal defini\n\n$surf\n$ligne"
		    } else {
			.pbouv.text configure -text \
			    "ATTENTION\n\nCas $PbnomProbleme non lu :\nLe fichier \".data\" contient au moins deux bords ayant le nom $nom."
		    }
		    button .pbouv.ok -text Ok -command {destroy .pbouv}
		    pack .pbouv.text .pbouv.ok -side top
		    set nom -1
		    break
		}
	    }
	    if {[lsearch [string tolower $ligne] "raccord"]==0 && [lsearch $PbnomFace [lindex $ligne 3]]==-1} {
		set nom [lindex $ligne 3]
		if ($dimension==2) {
		    set surf [Calculer_Surface2D [lreplace $ligne 0 3]]
		} else {
		    set surf [Calculer_Surface3D [lreplace $ligne 0 3]]
		}
		if {[lsearch $PbnomFace $nom]==-1 && ![catch {expr $surf}]} {
		    lappend PbnomFace $nom
		    set Pbsurface($nom) $surf
		    set Pbemissivite($nom) 1.
		    incr Pbnb_faces
		}
	    }
	set nom 1
	}
    } elseif {[file extension $filename]==".facesrayo"||[file extension $filename]==".info"} {
	set f [open "$PbPath/$filename" r]
	gets $f ligne
	set Pbnb_faces [lindex $ligne 0]

	for {set i 0} {$i<$Pbnb_faces} {incr i} {
	    gets $f ligne
	    set nom [lindex $ligne 0]
	    if {[lsearch $PbnomFace $nom]==-1} {
		lappend PbnomFace $nom
		set Pbsurface([lindex $ligne 0]) [lindex $ligne 1]
		if {[file extension $filename]==".facesrayo"} {
		    set Pbemissivite([lindex $ligne 0]) [lindex $ligne 2]
		} else {
		     set Pbemissivite([lindex $ligne 0]) 1.
		}
	    } else {
		toplevel .pbouv
		wm resizable .pbouv 0 0
		wm title .pbouv "Attention"
		focus .pbouv
		grab .pbouv
		label .pbouv.text -text "ATTENTION\n\nLe fichier \".facesrayo\" contient au moins deux faces ayant le nom $nom."
		button .pbouv.ok -text Ok -command {destroy .pbouv}
		pack .pbouv.text .pbouv.ok -side top
		set nom -1
		break
	    }
	}
    
	close $f

	if {$nom!=-1} {
	    set filename "$PbPath/$PbnomProbleme"
	    append filename ".factforme"
	    if {![file exists $filename]} {
		append filename "_bak"
	    }
	    set init 0
	    if {[file exists $filename]} {
		set f [open $filename r]
		gets $f ligne
		if {$ligne!=$Pbnb_faces} {
		    toplevel .pbouv
		    wm resizable .pbouv 0 0
		    wm title .pbouv "Attention"
		    focus .pbouv
		    label .pbouv.text -text "ATTENTION\n\nLes fichiers \".facesrayo\" et \".factforme\"\nne correspondent pas.\n\nInitialisation des facteurs de forme a xxx.\n(raison possible : nombres de faces differents)"
		    button .pbouv.ok -text Ok -command {destroy .pbouv}
		    pack .pbouv.text .pbouv.ok -side top
		} else {
		    set init 1
		    for {set i 0} {$i<$Pbnb_faces} {incr i} {
			gets $f ligne
			for {set j 0} {$j<$Pbnb_faces} {incr j} {
			    set x [lindex $ligne $j]
			    if {![catch {expr $x}] && [expr $x>=0] && [expr $x<=1]} {
				set Pbfactform([lindex $PbnomFace $j],[lindex $PbnomFace $i]) $x
			    } else {
# 				if {[expr $i!=$j]} {
				   set Pbfactform([lindex $PbnomFace $j],[lindex $PbnomFace $i])\
				       "xxx"
# 				} else {
# 				   set Pbfactform([lindex $PbnomFace $j],[lindex $PbnomFace $i])\
# 				       0.0
# 				}
			    }
#  			    if {[expr $i==$j]} {
#  				set Pbfactform([lindex $PbnomFace $j],[lindex $PbnomFace $i])\
#  				    0.0
#  			    }
			}
		    }
		}
	    }
	}
    } else {
	toplevel .pbouv
	wm resizable .pbouv 0 0
	wm title .pbouv "Attention"
	focus .pbouv
	label .pbouv.text -text "ATTENTION\n\nL'interface ne lit en entree que des fichiers ayant une extension\n\".facesrayo\", \".data\" ou \".geo\"."
	button .pbouv.ok -text Ok -command {destroy .pbouv}
	pack .pbouv.text .pbouv.ok -side top
	set nom -1
	set init 0
    }

    if {$init==0 && $nom!=-1} {
	for {set i 0} {$i<$Pbnb_faces} {incr i} {
	    for {set j 0} {$j<$Pbnb_faces} {incr j} {
# 		if {[expr $i!=$j]} {
		    set Pbfactform([lindex $PbnomFace $i],[lindex $PbnomFace $j]) "xxx"
# 		} else {
# 		    set Pbfactform([lindex $PbnomFace $j],[lindex $PbnomFace $i]) 0.0
# 		}
	    }
	}
	set PbmodifieP 1
	set PbmodifieS 1
    }

    if {$nom!=-1} {
	initCells
    } else {
	set Pbinit 0
	Pb:initialiser
    }
}

#------------------------------------------------------------------------
# Pb:Mise_a_Jour --
#
# Met a jour le tableau Pbfactform d'apres les entrees.

proc Pb:Mise_a_Jour {} {
    global PbmodifieP PbnomFace Pbnb_faces widget Pbfactform

    set ok [Pb:Verifier]

    if {$PbmodifieP==1 && $ok==1} {
	for {set i 0} {$i < $Pbnb_faces} {incr i} {
	    for {set j 0} {$j < $Pbnb_faces} {incr j} {
		set Pbfactform([lindex $PbnomFace $i],[lindex $PbnomFace $j]) [$widget(cells).$i-$j get]
	    }
	}
    }
    return $ok
}

#------------------------------------------------------------------------
# Pb:Verifier --
#
# Verifie les entrees.

proc Pb:Verifier {} {
    global Pbinit Pbnb_faces widget

    if {$Pbinit==0} {
	PbInit
	return 0
    } else {
	set ok [Pb:Verifier_Donnees]
	if {$ok==1} {
	    set ind [Pb:Compte_xxx]
	    if {$ind==0} {
		set ok2 [Pb:Verifier_Symetrie]
		set ok3 [Pb:Verifier_Completion]
		set ok [expr $ok2*$ok3]
		if {$ok==1} {
		    set msg "OK : Tous les elements sont conformes ; les relations de symetrie et de completion sont verifiees."
		    InsertWarning $msg ok
		}
		return $ok
	    } else {
		set ok [Pb:Resoudre]
		if {$ok==1} {
		    set ok2 [Pb:Verifier_Symetrie]
		    set ok3 [Pb:Verifier_Completion]
		    set ok [expr $ok2*$ok3]
		    if {$ok==1} {
			set msg "OK : Lancement de la resolution automatique, et verification :\nTous les elements sont conformes ; les relations de symetrie et de completion sont verifiees."
			InsertWarning $msg ok
		    }
		    return $ok
		} else {
		    set msg "Resolution des xxx restants impossible."
		    InsertWarning $msg bug
		    return 0
		}
	    }
	} else {
	    return 0
	}
    }
}


#------------------------------------------------------------------------
# Pb:VerifierX --
#
# Verifie que l'entree x est bien un reel entre 0 et 1, ou xxx.

proc Pb:VerifierX {x i j} {
    global PbnomFace
    set ok 1

    if {$x=="xxx"} {
    } elseif {[catch {expr $x}]} {
	set msg "*** L'element [lindex $PbnomFace $j] [lindex $PbnomFace $i] n'est pas un reel (ni xxx)."
	InsertWarning $msg bug
	set ok 0
    } elseif {$x>1.} {
	set msg "*** L'element [lindex $PbnomFace $j] [lindex $PbnomFace $i] est superieur a 1."
	InsertWarning $msg bug
	set ok 0
    } elseif {$x<0.} {
	set msg "*** L'element [lindex $PbnomFace $j] [lindex $PbnomFace $i] est inferieur a 0."
	InsertWarning $msg bug
	set ok 0
    }

    return $ok
}

#------------------------------------------------------------------------
# Pb:Verifier_Donnees --
#
# Verifie que les entrees sont bien des reels entre 0 et 1.

proc Pb:Verifier_Donnees {} {
    global widget Pbnb_faces PbnomFace i j Pbinit

    if {$Pbinit==0} {
	PbInit
	return 0
    } else {
	set ok 1
	for {set i 0} {$i < $Pbnb_faces} {incr i} {
	    for {set j 0} {$j < $Pbnb_faces} {incr j} {
		set x [$widget(cells).$i-$j get]
		set ok [expr $ok*[Pb:VerifierX $x $i $j]]
	    }
	}
	if {$ok==-1} {set ok 1}
	return $ok
    }
}

#------------------------------------------------------------------------
# Pb:Verifier_Symetrie --
#
# Verification des relations de symetrie entre facteurs de forme.

proc Pb:Verifier_Symetrie {} {
    global Pbnb_faces PbnomFace Pbsurface widget

    set ok 1
    for {set j 1} {$j<$Pbnb_faces} {incr j} {
	for {set i 0} {$i<$j} {incr i} {
	    set xij [$widget(cells).$j-$i get]
	    set xji [$widget(cells).$i-$j get]
	    if {![catch {expr $xij+$xji}]} {
		set x [expr $xij*$Pbsurface([lindex $PbnomFace $i]) - $xji*$Pbsurface([lindex $PbnomFace $j])]
		if {[expr $x!=0]} {
		    set msg "*** La symetrie [lindex $PbnomFace $j]-[lindex $PbnomFace $i] n'est pas verifiee (difference=$x)."
		    InsertWarning $msg bug
		    set ok 0
		}
	    }
	}
    }
    return $ok
}
proc Pb:Corriger {} {
    global Pbnb_faces PbnomFace Pbsurface widget \
	Pbinit
    puts "Pb:Corriger"
    if {$Pbinit==0} {
	PbInit
    } else {
	 puts "Pb:Corriger deb"
	for {set j 0} {$j<$Pbnb_faces} {incr j} {
	   for {set i 0} {$i<$j}  {incr i} {
		set xij [$widget(cells).$j-$i get]
		set xji  [$widget(cells).$i-$j get]
		set Si [ expr $Pbsurface([lindex $PbnomFace $i]) ]
		set Sj [ expr $Pbsurface([lindex $PbnomFace $j]) ]
		set err [ expr ($xij*$Si-$xji*$Sj)/2. ]
		puts "$i $j $err $Sj $xji $Si $xij "
		$widget(cells).$i-$j delete 0 end
		$widget(cells).$i-$j insert 0 [expr $xji+$err/$Sj ]
		$widget(cells).$j-$i delete 0 end
	       	set xji  [$widget(cells).$i-$j get]
		$widget(cells).$j-$i insert 0 [expr $xji*$Sj/$Si ]
	       	set xij  [$widget(cells).$j-$i get]
	       puts "$i $j $err $Sj $xji $Si $xij "
	       #$widget(cells).$j-$i insert 0 [expr ($xij-$err/$Si) ]
	       
	     #   puts " [expr $xij-$err/2./$Si ] [expr $xji-$err/2./$Sj ]"
	     #  puts " [expr $Si*($xij-$err/2./$Si) +$Sj*( $xji-$err/2./$Sj) ]"
	     #  puts "$i $j $err [expr $Sj*$xji] [ expr $Si*$xij] "
	    }
	
	}
	for {set j 0} {$j<$Pbnb_faces} {incr j} {
	    set ligne 0
	    for {set i 0} {$i<$Pbnb_faces} {incr i} {
		set x [$widget(cells).$i-$j get]
		set ligne [expr $ligne+$x]
	    }
	    for {set i 0} {$i<$Pbnb_faces} {incr i} {
		set xij  [$widget(cells).$i-$j get]
		$widget(cells).$i-$j delete 0 end
		$widget(cells).$i-$j insert 0 [expr $xij/$ligne ]
	    }
	}
    }
}

#------------------------------------------------------------------------
# Pb:Symetriser --
#
# Symetrisation de la matrice des facteurs de forme.

proc Pb:Symetriser {} {
    global Pbnb_faces PbnomFace Pbsurface widget \
	Pbinit

    if {$Pbinit==0} {
	PbInit
    } else {
	for {set j 0} {$j<$Pbnb_faces} {incr j} {
	    for {set i 0} {$i<$Pbnb_faces} {incr i} {
		set x [$widget(cells).$j-$i get]
		if {$x!="xxx" && [$widget(cells).$i-$j get]=="xxx"} {
		    $widget(cells).$i-$j delete 0 end
		    $widget(cells).$i-$j insert 0 [expr $x*$Pbsurface([lindex $PbnomFace $i])/$Pbsurface([lindex $PbnomFace $j])]
		}
	    }
	}
	Pb:Verifier_Symetrie
    }
}

#------------------------------------------------------------------------
# Pb:Symetriser_ij --
#
# Symetrisation du coefficient ij de la matrice des facteurs de forme.

proc Pb:Symetriser_ij {i j} {
    global Pbnb_faces PbnomFace Pbsurface widget \
	Pbinit

    if {$Pbinit==0} {
	PbInit
    } else {
	set x [$widget(cells).$i-$j get]
	if {[Pb:VerifierX $x $i $j] && $x!="xxx"} {
	    set y [expr $x*$Pbsurface([lindex $PbnomFace $j])/$Pbsurface([lindex $PbnomFace $i])]
	    $widget(cells).$j-$i delete 0 end
	    $widget(cells).$j-$i insert 0 $y
	}
    }
}

#------------------------------------------------------------------------
# Pb:Resoudre --
#
# Un passage de Resolution des xxx de la matrice des facteurs de forme.

proc Pb:Resoudre {} {
    global Pbnb_faces PbnomFace Pbsurface widget PbmodifieP \
	Pbinit

    if {$Pbinit==0} {
	PbInit
    } else {

	Pb:Symetriser

	set nb0 0
	set nb1 [Pb:Compte_xxx]
	set res_ok 1

	while {$nb1!=0 && $nb1!=$nb0} {
	    #Un passage de completion des lignes ou un seul xxx
	    for {set j 0} {$j<$Pbnb_faces} {incr j} {
		set ind {}
		set ligne0 0
		for {set i 0} {$i<$Pbnb_faces} {incr i} {
		    set x [$widget(cells).$i-$j get]
		    if {$x=="xxx"} {
			lappend ind $i
		    } else {
			set ligne0 [ expr $ligne0 + $x ]
		    }
		    
		}
		if {[llength $ind]==1 || $ligne0==1. } {
		    #un seul xxx dans la ligne : ok
		    set PbmodifieP 1
		    set ligne 0
		    set indxx [lindex $ind 0]
		   # for {set i 0} {$i<$Pbnb_faces} {incr i} {
		#	if {$i!=$indxx} {
			#    set ligne [expr $ligne+[$widget(cells).$i-$j get]]
			#}
		    #}
		    set ligne $ligne0
		    foreach {indxx}  $ind {
		    if {$ligne<=1.} {
			if {$indxx>$j} {
			    $widget(cells).$indxx-$j delete 0 end
			    $widget(cells).$indxx-$j insert 0 [expr 1.-$ligne]
			} else {
			    $widget(cells).$j-$indxx delete 0 end
			    $widget(cells).$j-$indxx insert 0 [expr (1.-$ligne)*$Pbsurface([lindex $PbnomFace $j])/$Pbsurface([lindex $PbnomFace $indxx])]
			}
		    } else {
			set res_ok 0
			break
		    }
		    }
		    Pb:Symetriser
		}
	    }

	    if {$res_ok==0} {
		break
	    }
	    set nb0 $nb1
	    set nb1 [Pb:Compte_xxx]
	}

	if {$res_ok==0} {
	    set msg "*** La ligne [lindex $PbnomFace $j]\na une somme superieure a 1 avant resolution !"
	    InsertWarning $msg bug
	    return 0
	} elseif {[expr $nb1==0]} {
	    set msg "La resolution des xxx est terminee."
	    InsertWarning $msg ok
	    return 1
	} else {
	    set msg "Le solveur ne peut resoudre les $nb1 xxx restants : le calcul necessite plus de donnees !"
	    InsertWarning $msg bug
	    return 0
	}
    }
}


#------------------------------------------------------------------------
# Pb:Compte_xxx --
#
# Denombre la quantite de xxx dans la matrice des facteurs de forme.

proc Pb:Compte_xxx {} {
    global Pbnb_faces PbnomFace widget

    set ind 0
    for {set j 0} {$j<$Pbnb_faces} {incr j} {
	for {set i 0} {$i<$Pbnb_faces} {incr i} {
	    set x [$widget(cells).$i-$j get]
	    if {$x=="xxx"} {
		incr ind
	    }
	}
    }

    return $ind
}

#------------------------------------------------------------------------
# Pb:Verifier_Completion --
#
# Verification des relations de completion entre facteurs de forme.

proc Pb:Verifier_Completion {} {
    global Pbnb_faces PbnomFace widget

    set ok 1
    for {set j 0} {$j<$Pbnb_faces} {incr j} {
	set ligne 0
	for {set i 0} {$i<$Pbnb_faces} {incr i} {
	    set x [$widget(cells).$i-$j get]
	    set ligne [expr $ligne+$x]
	}
	if {[expr $ligne!=1.]} {
	    set msg "*** La ligne [lindex $PbnomFace $j] n'a pas une somme egale a 1 : $ligne."
	    InsertWarning $msg bug
	    set ok 0
	}
    }
	    
    return $ok
}

#------------------------------------------------------------------------
# Pb:Modifier_Surfaces --
#
# Si drap = 0 : juste affichage des valeurs surfaces & emissivite.
# Si drap = 1 : modification des valeurs surfaces
# Si drap = 2 : modification des valeurs emissivite

proc Pb:Modifier_Surfaces {drap} {
    global Pbnb_faces PbnomFace Pbsurface cellWidth cellHeight \
	PbmodifieS PbmodifieP Pbinit Pbemissivite widget help_tips

    if {$Pbinit==0} {
	PbInit
    } else {
	if {[winfo exists .affsurf]} {
	    destroy .affsurf
	}
	toplevel .affsurf
	wm resizable .affsurf 1 1
	wm minsize .affsurf 200 270
	if {$drap==0} {
	    wm title .affsurf "Surfaces & Emissivites"
	} elseif {$drap==1} {
	    wm title .affsurf "Surfaces"
	} else {
	    wm title .affsurf "Emissivites"
	}
	if {$drap!=0} {
	    grab .affsurf
	    focus .affsurf
	}

	set textWidth [expr $cellWidth/5]

	frame .affsurf.titre
	label .affsurf.titre.toto -width 2
	label .affsurf.titre.nom -text Face -width $textWidth \
	    -relief raised -borderwidth 2
	if {$drap==0} {
	    label .affsurf.titre.surface -text Surface -width $textWidth \
		-relief raised -borderwidth 2
	    label .affsurf.titre.emissivite -text Emissivite -width $textWidth \
		-relief raised -borderwidth 2
	    pack .affsurf.titre.toto .affsurf.titre.nom .affsurf.titre.surface \
		.affsurf.titre.emissivite -side left
	} elseif {$drap==1} {
	    label .affsurf.titre.surface -text Surface -width $textWidth \
		-relief raised -borderwidth 2
	    pack .affsurf.titre.toto .affsurf.titre.nom \
		.affsurf.titre.surface -side left
	} else {
	    label .affsurf.titre.surface -text Emissivite -width $textWidth \
		-relief raised -borderwidth 2
	    pack .affsurf.titre.toto .affsurf.titre.nom \
		.affsurf.titre.surface -side left
	}
	
	set ii 0
	foreach el $PbnomFace {
	    incr ii
	}
	
	frame .affsurf.fond
	set base ".affsurf.fond.canvas"
	canvas $base -relief ridge -borderwidth 2 \
	    -height [expr $ii * $cellHeight*1.2] -width [expr $cellWidth*3.1] -scrollregion \
	    [list 0 0 [expr 2*$cellWidth] [expr $Pbnb_faces*$cellHeight*1.2]]\
	    -yscrollcommand {.affsurf.fond.yscroll set} \
	    -yscrollincrement $cellHeight
	if {$drap==0} {
	    $base configure -width 230 -scrollregion \
		[list 0 0 [expr 3*$cellWidth] [expr $Pbnb_faces*$cellHeight*1.2]]
	}

	scrollbar .affsurf.fond.yscroll -borderwidth 1 \
	    -command {.affsurf.fond.canvas yview} -orient vert -width 15
	pack .affsurf.fond.yscroll -side left -fill y -expand 1
	pack $base -side left -fill y -expand 1
	
	frame .affsurf.boutons
   
	pack .affsurf.titre -side top -pady 5p
	pack .affsurf.fond -expand 1 -side top -pady 5p -fill y
	pack .affsurf.boutons -side top -pady 5p
	set i 0
	foreach el $PbnomFace {
	    label $base.face$el -text $el -width $textWidth \
		-relief raised -borderwidth 2
	    $base create window \
		0 [expr $i * $cellHeight*1.2] \
		-anchor nw \
		-height $cellHeight \
		-width [expr $cellWidth*1.5] \
		-window $base.face$el

	    if {$drap==0} {
		frame $base.surface$el
		label $base.surface$el.surf -text $Pbsurface($el) \
		    -width $textWidth -bg gray -bd 2 -relief groove
		label $base.surface$el.emiss -text $Pbemissivite($el) \
		    -width $textWidth -bg gray -bd 2 -relief groove
		pack $base.surface$el.surf $base.surface$el.emiss -side left -padx 2
		$base create window \
		    [expr $cellWidth*1.55] [expr $i * $cellHeight*1.2] \
		    -anchor nw \
		    -height $cellHeight \
		    -width [expr $cellWidth*3] \
		    -window $base.surface$el
	    } elseif {$drap==1} {
		entry $base.surface$el -width $textWidth -bg white
		$base.surface$el insert 0 $Pbsurface($el)
		$base create window \
		    [expr $cellWidth*1.55] [expr $i * $cellHeight*1.2] \
		    -anchor nw \
		    -height $cellHeight \
		    -width [expr $cellWidth*1.5] \
		    -window $base.surface$el
	    } else {
		entry $base.surface$el -width $textWidth -bg white
		$base.surface$el insert 0 $Pbemissivite($el)
		$base create window \
		    [expr $cellWidth*1.55] [expr $i * $cellHeight*1.2] \
		    -anchor nw \
		    -height $cellHeight \
		    -width [expr $cellWidth*1.5] \
		    -window $base.surface$el
	    }
	    incr i
	}

	if {$drap!=0} {
	    button .affsurf.boutons.ok -text Ok -command {
		set ind {}
		set i 0
		foreach el $PbnomFace {
		    set S [.affsurf.fond.canvas.surface$el get]
		    if {![string compare [wm title .affsurf] "Surfaces"]} {
			if {![catch {expr $S}] && [expr $S>0.]} {
			    set Pbsurface($el) $S
			    set help_tips($widget(colNames).$i) "$el $S"
			    set help_tips($widget(lineNames).$i) "$el $S"
			} else {
			    lappend ind $el
			}
		    } else {
			if { [expr $S>=0.] ||  [expr $S==-1.] } { set test 1 
			} else { set test -1 }
			
			if {![catch {expr $S}] &&  [expr $test>=0.]  } {
			    set Pbemissivite($el) $S
			} else {
			    lappend ind $el
			}
		    }
		    incr i
		}
		if {$ind=={}} {
		    set PbmodifieS 1
		    set PbmodifieP 1
		    Pb:Symetriser
		    destroy .affsurf
		} else {
		    toplevel .pbsurf
		    wm resizable .pbsurf 0 0
		    wm title .pbsurf "Attention"
		    grab .pbsurf
		    focus .pbsurf
		    label .pbsurf.text -text "ATTENTION\n\n[wm title .affsurf] [join $ind " / "]\n non reelles ou non positives ou non egales a -1!"
		    button .pbsurf.ok -text Ok -command {destroy .pbsurf}
		    pack .pbsurf.text .pbsurf.ok -side top
		}
	    }
	} else {
	    button .affsurf.boutons.ok -text Ok -command {
		destroy .affsurf
	    }
	}
	pack .affsurf.boutons.ok -side left
	
	if {$drap!=0} {
	    button .affsurf.boutons.annuler -text Annuler -command {destroy .affsurf}
	    pack .affsurf.boutons.annuler -side left
	}
    }
}

#------------------------------------------------------------------------
#------------------------------------------------------------------------

wm protocol .top17 WM_DELETE_WINDOW {
    Pb:Quitter
}
wm minsize .top17 450 230

set Path_Aide "[file dirname [info script]]/Aide"
set Pbinit 0
Pb:initialiser
if {$argc==1} {
    set filename [lindex $argv 0]
    set PbPath [file join [pwd] [file dirname $filename]]
    set filename [file tail $filename]
    if {[file exists "$PbPath/$filename"]} {
	Pb:Lit_Fichier $filename
    } else {
	puts "\n\#\#ERREUR\#\#\n\tFichier $PbPath/$filename inexistant.\n"
	exit
    }
} else {
    set PbPath [pwd]
}
InitWarnings


#------------------------------------------------------------------------
#------------------------------------------------------------------------
#------------------------------------------------------------------------
#------------------------------------------------------------------------
# PROCEDURES D'AIDE

#------------------------------------------------------------------------
# Aide:Init_Aide --
#
# Affichage de la liste des fichiers d'aide

proc Aide:Init_Aide {} {
    global Fich_Aide Titre_Aide Cles_Aide Init_Aide Path_Aide Voir_Fich \
	Fen_Aides

    set Fen_Aides {}
    set Init_Aide 0
    set tmp [lsort [glob $Path_Aide/*.hlp]]
    set Fich_Aide {}
    foreach el $tmp {
	lappend Fich_Aide [file tail $el]
    }
    set Titre_Aide {}
    set Cles_Aide {}
    set Voir_Fich {}
    set tmp {}
    foreach el $Fich_Aide {
	set filename "$Path_Aide/[file rootname $el]"
	append filename ".ref"
	set f [open $filename r]
	gets $f ligne
	lappend Titre_Aide $ligne
	lappend tmp [join $ligne ""]
	gets $f ligne
	lappend Cles_Aide $ligne
	gets $f ligne
	lappend Voir_Fich $ligne
	close $f
    }

    set i 0
    foreach ref $Voir_Fich {
	set liste {}
	foreach el $ref {
	    set ind [lsearch $tmp $el]
	    if {$ind!=-1} {
		lappend liste $ind
	    } else {
		puts "reference non trouvee :"
		puts "$el $ind \#\# $tmp"
		puts $Voir_Fich
	    }
	}
	set Voir_Fich [lreplace $Voir_Fich $i $i $liste]
	incr i
    }
}



#------------------------------------------------------------------------
# Aide:Index --
#
# Affichage de la liste des fichiers d'aide

proc Aide:Index {} {
    global Fich_Aide Titre_Aide Cles_Aide Init_Aide Fen_Aides

    if {![info exists Init_Aide]} {
	Aide:Init_Aide
    }

    if {![catch {wm state .aide}]} {
	destroy .aide
    }

    if {[lsearch $Fen_Aides .aide]==-1} {
	lappend Fen_Aides .aide
    }

    toplevel .aide
    wm title .aide "Aide"
    wm resizable .aide 1 1
    wm minsize .aide 300 250

    label .aide.text -text "Liste des entrees d'aide :"
    frame .aide.corps
    scrollbar .aide.corps.scroll -command ".aide.corps.liste yview" \
	-orient vert -width 15
    listbox .aide.corps.liste -borderwidth 2 -relief sunken -bg white\
	-yscrollcommand ".aide.corps.scroll set" -width 40 -height 10
    bind .aide.corps.liste <Double-1> {.aide.lire invoke}
    pack .aide.corps.liste .aide.corps.scroll -side left -fill y -expand 1

    button .aide.lire -text "Ouvrir fichier" -command {
	set el [.aide.corps.liste curselection]
	Aide:Aff_Fich $el
    }
    
    button .aide.fermer -text "Fermer l'aide" -command {destroy .aide}
    pack .aide.text -side top
    pack .aide.corps -side top -fill y -expand 1
    pack .aide.lire .aide.fermer -side top

    foreach el $Titre_Aide {
	.aide.corps.liste insert end $el
    }
}

#------------------------------------------------------------------------
# Aide:Rechercher --
#
# Affichage de la fenetre de recherche par mots-cles

proc Aide:Rechercher {} {
    global Fich_Aide Titre_Aide Cles_Aide booleen Init_Aide resrech Fen_Aides

    if {![info exists Init_Aide]} {
	Aide:Init_Aide
    }

    set resrech {}
    if {![catch {wm state .aiderech}]} {
	destroy .aiderech
    }

    if {[lsearch $Fen_Aides .aiderech]==-1} {
	lappend Fen_Aides .aiderech
    }

    toplevel .aiderech
    wm title .aiderech "Aide - Recherche par mots-cles"
    wm resizable .aiderech 0 0

    label .aiderech.titre -text "Aide - Recherche par mots-cles"

    frame .aiderech.corps -borderwidth 2 -relief ridge
    label .aiderech.corps.titre -text "Donnez les mots cles"
    entry .aiderech.corps.mots -bg white
    focus .aiderech.corps.mots
    bind .aiderech.corps.mots <Return> {.aiderech.corps.rech invoke}
    frame .aiderech.corps.bool
    set booleen et
    radiobutton .aiderech.corps.bool.et -text ET -variable booleen -value et
    radiobutton .aiderech.corps.bool.ou -text OU -variable booleen -value ou
    button .aiderech.corps.rech -text Rechercher -command \
	{
	    set liste [string tolower [.aiderech.corps.mots get]]
	    set resrech [Aide:Rech_Fich $liste $booleen]
	    .aiderech.res.corps.list delete 0 end
	    foreach el $resrech {
		.aiderech.res.corps.list insert end \
		    "[lindex $Titre_Aide [lindex $el 0]] ([lindex $el 1])"
	    }
	}

    frame .aiderech.res
    label .aiderech.res.titre -text "Chapitres trouves (nb de mots contenus)"
    frame .aiderech.res.corps
    scrollbar .aiderech.res.corps.scroll -command ".aiderech.res.corps.list yview" \
	-orient vert -width 15
    listbox .aiderech.res.corps.list -borderwidth 2 -relief sunken -bg white\
	-yscrollcommand ".aiderech.res.corps.scroll set" -width 40 -height 10

    bind .aiderech.res.corps.list <Double-1> {.aiderech.lire invoke}

    button .aiderech.lire -text "Ouvrir fichier" -command {
	set el [.aiderech.res.corps.list curselection]
	Aide:Aff_Fich [lindex [lindex $resrech $el] 0]
    }
    button .aiderech.fermer -text Fermer -command {destroy .aiderech}

    pack .aiderech.titre .aiderech.corps .aiderech.res .aiderech.lire .aiderech.fermer \
	-side top -pady 10 -fill x
    pack .aiderech.corps.titre -side top
    pack .aiderech.corps.mots -side top -fill x
    pack .aiderech.corps.bool .aiderech.corps.rech -side top
    pack .aiderech.corps.bool.et .aiderech.corps.bool.ou -side left -padx 20
    pack .aiderech.res.titre .aiderech.res.corps -side top
    pack .aiderech.res.corps.list .aiderech.res.corps.scroll -side left -fill y
}

#------------------------------------------------------------------------
# Aide:Aff_Fich --
#
# Affichage du contenu d'un fichier d'aide indice $num
# si $num==1 : affiche le contenu de l'A_propos

proc Aide:Aff_Fich {num} {
    global Fich_Aide Titre_Aide Cles_Aide Path_Aide Voir_Fich Fen_Aides

    if {![catch {wm state .aide$num}]} {
	destroy .aide$num
    }

    if {[lsearch $Fen_Aides .aide$num]==-1} {
	lappend Fen_Aides .aide$num
    }

    toplevel .aide$num
    wm title .aide$num "Aide"
    wm resizable .aide$num 0 1
    wm minsize .aide$num 400 380

    label .aide$num.text
    frame .aide$num.corps
    scrollbar .aide$num.corps.scroll -command ".aide$num.corps.texte yview" \
	-orient vert -width 15
    text .aide$num.corps.texte -borderwidth 2 -relief sunken -wrap word\
	-yscrollcommand ".aide$num.corps.scroll set" -width 70 -bg white -height 15
    pack .aide$num.corps.texte  -side left -fill y -expand 1
    pack .aide$num.corps.scroll -side left -fill y -expand 1

    button .aide$num.fermer -text "Fermer l'aide" -command "destroy .aide$num"

    if {$num>=0} {
	label .aide$num.voirtitre -text "Voir aussi :"
	frame .aide$num.voir -borderwidth 2 -relief ridge
	scrollbar .aide$num.voir.scroll -command ".aide$num.voir.liste yview" \
	    -orient vert -width 15
	listbox .aide$num.voir.liste -borderwidth 2 -relief sunken -bg white \
	    -yscrollcommand ".aide$num.voir.scroll set" -width 40 -height 5
	.aide$num.voir.liste delete 0 end
	bind .aide$num.voir.liste <Double-1> {
	    set win %W
	    set el [$win curselection]
	    set num [string range $win 5 [expr [string length $win]-12]]
	    Aide:Aff_Fich [lindex [lindex $Voir_Fich $num] $el]
	}
	foreach el [lindex $Voir_Fich $num] {
	    .aide$num.voir.liste insert end [lindex $Titre_Aide $el]
	}
	pack .aide$num.voir.liste .aide$num.voir.scroll -side left -fill y
	pack .aide$num.voir .aide$num.voirtitre -side bottom
    }
	
    pack .aide$num.text -side top
    pack .aide$num.corps -side top -fill y -expand 1
    pack .aide$num.fermer -side top -expand 0

    .aide$num.corps.texte delete 1.0 end
    if {[expr $num>-1]} {
	.aide$num.text configure -text "Aide sur [lindex $Titre_Aide $num]"
	set filename "$Path_Aide/[lindex $Fich_Aide $num]"
    } else  {
	.aide$num.text configure -text "A propos de l'interface"
	set filename "$Path_Aide/A_propos"
    }
    set f [open $filename r]
    while {![eof $f]} {
	.aide$num.corps.texte insert end [read $f 1000]
    }
    .aide$num.corps.texte configure -state disabled
}

#------------------------------------------------------------------------
# Aide:Rech_Fich --
#
# Recherche des fichiers contenant la liste des mots cles $liste
#renvoie la liste des noms des fichiers

proc Aide:Rech_Fich {liste bool} {
    global Fich_Aide Titre_Aide Cles_Aide

    set res {}

    set i 0
    foreach cles $Cles_Aide {
	set nb 0
	foreach mot $liste {
	    if {[lsearch $cles $mot]!=-1} {
		incr nb
	    }
	}
	switch $bool {
	    et {
		if {[expr $nb==[llength $liste]]} {
		    lappend res [list $i $nb]
		}
	    }
	    ou {
		if {[expr $nb>0]} {
		    lappend res [list $i $nb]
		}
	    }
	}
	incr i
    }
    return $res
}

#------------------------------------------------------------------------
# Aide:Fermer_Aides --
#
# Recherche des fichiers contenant la liste des mots cles $liste
#renvoie la liste des noms des fichiers

proc Aide:Fermer_Aides {} {
    global Fen_Aides Init_Aide

    if {![info exists Init_Aide]} {
	Aide:Init_Aide
    }

    foreach fen $Fen_Aides {
	if {![catch {destroy $fen}]} {
	    destroy $fen
	}
    }

    set Fen_Aides {}
}


#------------------------------------------------------------------------
# Pb:Calc_ANSYS --
#
# Calcul des facteurs de forme avec ANSYS
proc Pb:Calc_ANSYS {} {
    global Pbinit Pbnb_faces widget PbnomProbleme PbPath nb_zones_3D_ANSYS \
	PbPath methode2D methode3D nb_zones_2D_ANSYS dec_ang_ANSYS graph_ANSYS 

    set types {{{Fichiers de geometrie TRUST} {.geom }} \
		   {{Tous} {*}}}
    set filename [tk_getOpenFile -defaultextension ".geom" \
                      -initialdir $PbPath -initialfile $PbnomProbleme \
                      -parent $widget(cells) -filetypes $types]
    if {$filename != ""} {
	set PbPath [file dir $filename]
	set filename [file tail $filename]
    }

#   On lit les 3 premieres lignes  du fichier filename pour determinier la dimenssion
    set f [open "$PbPath/$filename" r]
    gets $f ligne
    gets $f ligne
    gets $f ligne
    set dim [lindex $ligne 1]
    close $f
    
    if { [expr $dim==2] } {
#   on cherche a voir si le cas n'est pas 2D axi
#	set nom [ exec grep Rectangle $filename  ]
# Correction PL: plantage de Preray si on cherche Rectangle dans un .geom
# qui n'en contient pas (ex 2D Triangle)
# Il semble que si le grep retourne rien, cela plante !
        set nom [ exec grep angle $filename  ]
	set axi "Rectangle_2D_axi "
	if { $nom==$axi  } {
	    set dim 3
	}
    }
   
#   On sauve les options de calcul dans le fichier $filename.OPT_ANSYS
#   on recupere le prefixe de $filename
    set nom [ exec basename $filename .geom ]
    set suffix ".OPT_ANSYS"
    set filename_opt  "$nom$suffix"
    set f [open $PbPath/$filename_opt w]
    puts $f "Options d'execussion de ANSYS pour le fichie $filename : \n"
    puts $f "methode2D   $methode2D"
    puts $f "methode3D   $methode3D"
    puts $f "nb_zones_2D_ANSYS   $nb_zones_2D_ANSYS"
    puts $f "nb_zones_3D_ANSYS   $nb_zones_3D_ANSYS"
    puts $f "dec_ang_ANSYS   $dec_ang_ANSYS"
    puts $f "graph_ANSYS   $graph_ANSYS"
    close $f

#   Calcul des facteurs de forme avec ANSYS en appelant le script geom2ansys
    if { [expr $dim==2] } { 
	Dialog3 "[ exec geom2ansys $filename $methode2D $nb_zones_2D_ANSYS $dec_ang_ANSYS $graph_ANSYS 2>$PbPath/$nom.err ]" 1
    } elseif { [expr $dim==3] } { 
	Dialog3 "[ exec geom2ansys $filename $methode3D $nb_zones_3D_ANSYS $dec_ang_ANSYS $graph_ANSYS 2>$PbPath/$nom.err ]" 1
    } else { Gros probleme }

    set suffix ".facesrayo"
    Pb:Lit_Fichier "$nom$suffix"
    Pb:Modifier_Surfaces 2
}


#------------------------------------------------------------------------
# Pb:Opt_ANSYS --
#
# Saisie des options pour ANSYS. Si on ne passe pas
# par cette procedure, on utilise les options par defaut
# du couplage TRIO-U/ANSYS

proc Pb:Opt_ANSYS {} {
    global NbOptions PbnomFace Pbsurface cellWidth cellHeight \
	PbmodifieS PbmodifieP Pbinit Pbemissivite widget help_tips methode2D methode3D \
	nb_zones_2D_ANSYS dec_ang_ANSYS graph_ANSYS nb_zones_3D_ANSYS methode2D_defaut \
	methode3D_defaut nb_zones_2D_ANSYS_defaut nb_zones_3D_ANSYS_defaut dec_ang_ANSYS_defaut \
	graph_ANSYS_defaut

    set NbOptions 6
    set multwidth_opt 5 
    set multwidth_val 1.4
    set entrecell 0.05

    if {[winfo exists .affsurf]} {
	destroy .affsurf
    }
    toplevel .affsurf
    wm resizable .affsurf 1 1
    wm minsize .affsurf 200 270
    wm title .affsurf "Options ANSYS"

    set textWidth [expr 2*$cellWidth/5]

    frame .affsurf.titre
    label .affsurf.titre.toto -width 2
    label .affsurf.titre.option -text Valeur -width $textWidth \
	-relief raised -borderwidth 2
    
    label .affsurf.titre.valeur -text Option -width $textWidth \
	-relief raised -borderwidth 2
    pack .affsurf.titre.toto .affsurf.titre.valeur \
	.affsurf.titre.option -side left
    
	
    frame .affsurf.fond
    set base ".affsurf.fond.canvas"
    canvas $base -relief ridge -borderwidth 2 \
	-height [expr $NbOptions*$cellHeight*1.2] -width [expr $cellWidth*$multwidth_opt+$cellWidth*$multwidth_val] -scrollregion \
	[list 0 0 [expr 2*$cellWidth] [expr $NbOptions*$cellHeight*1.2]]\
	-yscrollcommand {.affsurf.fond.yscroll set} \
	-yscrollincrement $cellHeight

    scrollbar .affsurf.fond.yscroll -borderwidth 1 \
	-command {.affsurf.fond.canvas yview} -orient vert -width 15
    pack .affsurf.fond.yscroll -side left -fill y -expand 1
    pack $base -side left -fill y -expand 1
    
    frame .affsurf.boutons
    
    pack .affsurf.titre -side top -pady 5p
    pack .affsurf.fond -expand 1 -side top -pady 5p -fill y
    pack .affsurf.boutons -side top -pady 5p
    set i 0

    for {set num_opt 1} { $num_opt < ($NbOptions+1) } { incr num_opt } {
#------------------------------------------------------------------------
#   Choix de l'option non_hidden (sans faces cachees)
#------------------------------------------------------------------------
	if { [ expr $num_opt==1 ] } {
	    label $base.option1 -text "Methode (Si cas 2D ou 2D Axi)" -width $textWidth \
		-relief raised -borderwidth 2
	} elseif { [ expr $num_opt==2 ] } {
	    label $base.option2 -text "Methode (Si cas 3D)" -width $textWidth \
		-relief raised -borderwidth 2
	} elseif { [ expr $num_opt==3 ] } {
	    label $base.option3 -text "Nb zones (Si cas 2D)" -width $textWidth \
		-relief raised -borderwidth 2
	} elseif { [ expr $num_opt==4 ] } {
	    label $base.option4 -text "Nb zones (Si cas 3D ou 2D Axi)" -width $textWidth \
		-relief raised -borderwidth 2
	} elseif { [ expr $num_opt==5 ] } {
	    label $base.option5 -text "Decoup. angul. (2D Axi)" -width $textWidth \
		-relief raised -borderwidth 2	    
	} elseif { [ expr $num_opt==6 ] } {
	    label $base.option6 -text "ANSYS Graphique" -width $textWidth \
		-relief raised -borderwidth 2	    
	} else { return 0 }

	$base create window \
	    0 [expr $i * $cellHeight*1.2] \
	    -anchor nw \
	    -height $cellHeight \
	    -width [expr $cellWidth*$multwidth_opt] \
	    -window $base.option$num_opt

	entry $base.valeur$num_opt -width $textWidth -bg white

	if { [ expr $num_opt==1 ] } {
	    $base.valeur$num_opt insert 0 $methode2D
	} elseif { [ expr $num_opt==2 ] } {
	    $base.valeur$num_opt insert 0 $methode3D
	} elseif { [ expr $num_opt==3 ] } {
	    $base.valeur$num_opt insert 0 $nb_zones_2D_ANSYS
	} elseif { [ expr $num_opt==4 ] } {
	    $base.valeur$num_opt insert 0 $nb_zones_3D_ANSYS
	} elseif { [ expr $num_opt==5 ] } {
	    $base.valeur$num_opt insert 0 $dec_ang_ANSYS
	} elseif { [ expr $num_opt==6 ] } {
	    $base.valeur$num_opt insert 0 $graph_ANSYS
	} else { return 0 }
	
	$base create window \
	    [expr $cellWidth*($multwidth_opt+$entrecell)] [expr $i * $cellHeight*1.2] \
	    -anchor nw \
	    -height $cellHeight \
	    -width [expr $cellWidth*$multwidth_val] \
	    -window $base.valeur$num_opt

	incr i
    }

# penser a faire des tests de coherence sur les options choisies
# Il faut sauver les valeurs saisies par l'utilisateur ...

    button .affsurf.boutons.ok -text Ok -command {
	set ind {}
	set i 0

	for {set num_opt 1} { $num_opt < ($NbOptions+1) } { incr num_opt } {
	    set Val [.affsurf.fond.canvas.valeur$num_opt get]
	    if {![catch {expr $Val}]} {
		if { [ expr $num_opt==1 ] } {
		    if {[expr $Val==0] || [ expr $Val==1 ] } {
			set methode2D $Val
		    } else { lappend ind 1 }
		} elseif { [ expr $num_opt==2 ] } {
		    if {[expr $Val==0] || [ expr $Val==1 ] || [ expr $Val==2 ] || [ expr $Val==3 ] } {
			set methode3D $Val
		    } else { lappend ind 2 }
		} elseif { [ expr $num_opt==3 ] } {
		    if {[expr $Val>0] && [ expr $Val<=1000 ] } {
			set nb_zones_2D_ANSYS $Val
		    } else { lappend ind 3 }
		} elseif { [ expr $num_opt==4 ] } {
		    if {[expr $Val>0] && [ expr $Val<=100 ] } {
			set nb_zones_3D_ANSYS $Val
		    } else { lappend ind 4 }
		} elseif { [ expr $num_opt==5 ] } {
		    if {[expr $Val>6] && [ expr $Val<=90 ] } {
			set dec_ang_ANSYS $Val
		    } else { lappend ind 5 }
		} elseif { [ expr $num_opt==6 ] } {
		    if {[expr $Val==0] || [ expr $Val==1 ] } {
			set graph_ANSYS $Val
		    } else { lappend ind 6 }
		} else { return 0 }		
	    } else { lappend ind 1 }
	    incr i 
	}

	if {$ind=={}} {
	    destroy .affsurf
	} else {
	    toplevel .pbsurf
	    wm resizable .pbsurf 0 0
	    wm title .pbsurf "Attention"
	    grab .pbsurf
	    focus .pbsurf
	    if { [expr $ind==1 ] } {
	    	label .pbsurf.text -text "ATTENTION\n\n[wm title .affsurf] [join $ind " / "]\n non egale ou comprise entre 0 et 1!"
	    } elseif { [expr $ind==2] } {
		label .pbsurf.text -text "ATTENTION\n\n[wm title .affsurf] [join $ind " / "]\n non egale ou comprise entre 0 et 3!"
	    } elseif { [expr $ind==3] } {
		label .pbsurf.text -text "ATTENTION\n\n[wm title .affsurf] [join $ind " / "]\n En 2D, cette option \n doit etre comprise entre 0 et 1000!"
	    } elseif { [expr $ind==4] } {
		label .pbsurf.text -text "ATTENTION\n\n[wm title .affsurf] [join $ind " / "]\n En 3D ou 2D Axi, cette option \n doit etre comprise entre 0 et 100!"
	    } elseif { [expr $ind==5] } {
		label .pbsurf.text -text "ATTENTION\n\n[wm title .affsurf] [join $ind " / "]\n non comprise entre 6 et 90!"
	    } elseif { [expr $ind==6] } {
		label .pbsurf.text -text "ATTENTION\n\n[wm title .affsurf] [join $ind " / "]\n non egale a 0 ou 1!!"
	    } else {
		label .pbsurf.text -text "ATTENTION\n\n Cette option n'existe pas"
	    }

	    button .pbsurf.ok -text Ok -command {destroy .pbsurf}
	    pack .pbsurf.text .pbsurf.ok -side top
	}
    }
    pack .affsurf.boutons.ok -side left

    button .affsurf.boutons.defaut -text Defaut -command {
	set methode2D $methode2D_defaut
	set methode3D $methode3D_defaut
	set nb_zones_2D_ANSYS $nb_zones_2D_ANSYS_defaut
	set nb_zones_3D_ANSYS $nb_zones_3D_ANSYS_defaut
	set dec_ang_ANSYS $dec_ang_ANSYS_defaut
	set graph_ANSYS $graph_ANSYS_defaut

	destroy .affsurf
	Pb:Opt_ANSYS
    }
    pack .affsurf.boutons.defaut -side left

    button .affsurf.boutons.lire -text Lire -command {
#       Lecture du fichier d'options ANSYS
	set types {{{Fichiers d'options ANSYS} {.OPT_ANSYS }} \
		       {{Tous} {*}}}
	set file [tk_getOpenFile -defaultextension ".OPT_ANSYS" \
			  -initialdir $PbPath -initialfile $PbnomProbleme \
			  -parent $widget(cells) -filetypes $types]
	if {$file != ""} {
	    set PbPath [file dir $file]
	    set file [file tail $file]
	}	

	set f [open "$PbPath/$file" r]
	gets $f ligne
	gets $f ligne
	gets $f ligne
	set methode2D [lindex $ligne 1]
	gets $f ligne
	set methode3D [lindex $ligne 1]
	gets $f ligne
	set nb_zones_2D_ANSYS [lindex $ligne 1]
	gets $f ligne
	set nb_zones_3D_ANSYS [lindex $ligne 1] 
	gets $f ligne
	set dec_ang_ANSYS [lindex $ligne 1]
	gets $f ligne
	set graph_ANSYS [lindex $ligne 1]
		
	destroy .affsurf
	Pb:Opt_ANSYS
    }
    pack .affsurf.boutons.lire -side left


    button .affsurf.boutons.sauver -text Sauver -command {
#       Sauver le fichier d'options ANSYS dans un fichier


	set types {{{Fichiers d'options ANSYS} {.OPT_ANSYS }} \
		       {{Tous} {*}}}
	set filename [tk_getSaveFile -defaultextension ".OPT_ANSYS" \
			  -initialdir $PbPath \
			  -initialfile $PbnomProbleme \
			  -parent $widget(cells) -filetypes $types]
	if {$filename != ""} {
	    set PbPath [file dirname $filename]
	    set PbnomProbleme [file tail [file rootname $filename]]
	    wm title .top17 \
		"Saisie des facteurs de forme : $PbnomProbleme"
	}

	set f [open $filename w]
	puts $f "Options d'execussion de ANSYS pour le fichie $filename : \n"
	puts $f "methode2D   $methode2D"
	puts $f "methode3D   $methode3D"
	puts $f "nb_zones_2D_ANSYS   $nb_zones_2D_ANSYS"
	puts $f "nb_zones_3D_ANSYS   $nb_zones_3D_ANSYS"
	puts $f "dec_ang_ANSYS   $dec_ang_ANSYS"
	puts $f "graph_ANSYS   $graph_ANSYS"
	close $f	
    }
    pack .affsurf.boutons.sauver -side left



}
#------------------------------------------------------------------------
#------------------------------------------------------------------------



proc Dialog3 { mesg num { options  Ok} } {
    if {[winfo exists .toto]} {
	destroy .toto
    }
    frame .toto
    set base2 .toto.message$num
    set sw [winfo screenwidth .]
    set sh [winfo screenheight .]
    if [winfo exists $base2] { destroy $base2 }
    if {![winfo exists $base2]} {
        toplevel $base2 -class vTcl
        wm title $base2 "Visual Tcl Message"
        wm transient $base2 .toto
        frame $base2.f -bd 2 -relief groove
        text $base2.f.t -bd 0 -relief flat   -yscrollcommand " $base2.f.ys set "
	$base2.f.t insert 1.0 "$mesg"
	scrollbar $base2.f.ys -command   "  $base2.f.t yview " -orient vert
        frame $base2.o -bd 1 -relief sunken
        foreach i $options {
            set n [string tolower $i]
            button $base2.o.$n -text $i -width 5 \
		-command "
                destroy $base2
		"
            pack $base2.o.$n -side left -expand 1 -fill x
        }
        pack $base2.f.t $base2.f.ys -side left -expand 1 -fill both -ipadx 5 -ipady 5
        pack $base2.f -side top -expand 1 -fill both -padx 2 -pady 2
        pack $base2.o -side top -fill x -padx 2 -pady 2
    }
    wm withdraw $base2
    update idletasks
    set w [winfo reqwidth $base2]
    set h [winfo reqheight $base2]
    set x [expr ($sw - $w)/2]
    set y [expr ($sh - $h)/2]
    
    wm geometry $base2 +$x+$y
    
    wm deiconify $base2
    grab $base2
    # tkwait window $base2
    grab release $base2
    #return $res
    #return "Oui"
}
