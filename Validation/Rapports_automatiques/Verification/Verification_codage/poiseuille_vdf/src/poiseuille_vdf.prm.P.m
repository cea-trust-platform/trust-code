Parameters
{
    Title "Ecoulement mixte Poiseuille Couette plan."
    Author "Y. Gorsse"
    code TRUST
    Prerequisite "./extract_convergence"

#Pset(_list_dis_ $list_dis )

#Pforeach _dis_ ( _list_dis_ )
    TestCase _dis_ jdd.data
#Pendforeach(_dis_)
}

Chapter
{
    Title "Comparaisons a une solution analytique"
    Description ""
#Pforeach _var_ ( p v )
    Figure
    {
        incluredesccourbes 0
        Titre "Comparaison _var_"
        Dimension 2
        LabelX "position"
        #Pif("_var_"=="v") LabelY "velocity" #Pelse LabelY "pressure" #Pendif

        description ""
        # width 7cm
        legende below

        Courbe
        {
            Legende "reference"
            segment ./const/explicit/jdd.data _var__EX
            Style l
            colonnes ($1) ($2)
            Style p
            TypePoints 7
            TypeLigne 3
        }
#Pforeach _dis_ ( _list_dis_ )
        Courbe
        {
            Legende "_dis_"
            Origine "Trio_U"
            segment ./_dis_/jdd.data _var_
            Style l
            colonnes ($1) ($2)

        }
#Pendforeach(_dis_)
    }
#Pendforeach(_var_)

    Figure
    {
        incluredesccourbes 0
        Titre "Comparaison dt"
        Dimension 2
        LabelX "Time"
        LabelY "dt"

        description ""
        # width 7cm
        legende below
        logy

#Pforeach _dis_ ( _list_dis_ )
        Courbe
        {
            Legende "_dis_"
            Origine "Trio_U"
            fichier ./_dis_/jdd.dt_ev
            Style lp
            colonnes ($1) ($2)

        }
#Pendforeach(_dis_)
    }
}
