Parameters
{
    Title "Rotating flow"
    Description ""
    Author "Elie Saikali and Yannick Gorsse"
#Pset(_list_dis_ $list_dis )

#Pforeach _dis_ ( _list_dis_ )
    TestCase _dis_ jdd.data
#Pendforeach(_dis_)
}

Chapter
{
    Title "Comparaisons"
    Description ""
#Pforeach _v_ ( vx vy )
    Figure
    {
        incluredesccourbes 0
        Titre "Comparaison _v_"
        Dimension 2
        LabelX "position"
        LabelY "velocity"
        description ""
        # width 7cm
        legende below

#Pforeach _dis_ ( _list_dis_ )
        Courbe
        {
            Legende "_dis_"
            Origine "Trio_U"
            segment ./_dis_/jdd.data _v_
            Style l
            #Pif("_v_"=="vx") colonnes ($1) ($2) #Pelse colonnes ($1) ($3) #Pendif

        }
#Pendforeach(_dis_)
    }
#Pendforeach(_v_)
}
