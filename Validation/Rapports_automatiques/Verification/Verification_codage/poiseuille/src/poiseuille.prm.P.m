Parameters
{
    Title "Ecoulement mixte Poiseuille Couette plan."
    Author "Y. Gorsse"
    code TRUST
    Prerequisite "./extract_convergence"

#Pset(_list_meshes_ $meshes )
#Pset(_list_dis_ $list_dis )

#Pforeach _dis_ ( _list_dis_ )
#Pforeach _n_ ( _list_meshes_ )
    TestCase _dis_/maillage__n_ jdd.data
#Pendforeach(_n_)
#Pendforeach(_dis_)
}

Chapter
{
    Title "Ordres de convergence"
    Description " "

#Pforeach _var_ ( v p )
    Figure
    {
        labelx "Ne^{-1/2}"
        labely erreur L2
        logx
        logy
        Include_Description_Curves 0
        legend below title "Legend"
        width 15cm
        Title "Ordres de convergence - _var_"
#Pforeach _dis_ ( _list_dis_ )
        Curve
        {
            legend _dis_
            file _dis_/conv_glob__var_
            colonnes 1 2
            style linespoints
        }
#Pendforeach(_dis_)
    }
#Pendforeach(_var_)

}
