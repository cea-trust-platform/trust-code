Parameters
{
    Title "Cavite entrainee - Re = 400"
    Description ""
    Author "Y.G."
#Pset(_list_dis_ $list_dis )

#Pforeach _dis_ ( _list_dis_ )
    TestCase _dis_ jdd.data
#Pendforeach(_dis_)
}

Chapter
{
    Title "Comparaisons a une solution de reference"
    Description "La reference est dans le src"
#Pforeach _v_ ( vx vy )
    Figure
    {
        incluredesccourbes 0
        Titre "Comparaison _v_"
        Dimension 2
        LabelX "position"
        LabelY "velocity"
        description "Reference : The lid-driven square cavity flow: numerical solution with a 1024 x 1024 grid, Marchi et al., Journal of the Brazilian Society of Mechanical Sciences and Engineering, 2009"
        # width 7cm
        legende below

        Courbe
        {
            Legende "reference"
            fichier ./ref__v_.txt
            Style p
            TypePoints 7
            TypeLigne 3
        }
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