Parameters
{
    Title "Rayleigh-Benard convection"
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

#Pforeach _v_ ( tmp0 tmp1 )
    Figure
    {
        incluredesccourbes 0
        Titre "Comparaison _v_"
        Dimension 2
        LabelX "position"
        LabelY "temperature"
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
             colonnes ($1) ($2) 

        }
#Pendforeach(_dis_)
    }
#Pendforeach(_v_)
}
