Parameters
{
    Title "Ecoulement avec saut de porosite en 2D"
    Description "Ecoulement unidirectionel, sans diffusion. Le domaine est discretise en 2D. Le calcul est mene sur differents maillages (cartesien conforme, triangles et non conforme)."
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
    Title "Results"
    Description " "

    Figure
    {
        title_figure residus
        include_description_curves 0
        labelX "t (s)"
        labelY "residuals"
        logY

        width 18cm
        grid
        legend outside title "Legende"
#Pforeach _dis_ ( _list_dis_ )
#Pforeach _n_ ( _list_meshes_ )
        Curve
        {
            legend "_dis_ maillage__n_"
            file ./_dis_/maillage__n_/jdd.dt_ev
            columns 1 4
            style lines
            Linewidth 2
        }
#Pendforeach(_n_)
#Pendforeach(_dis_)
    }

Figure
    {
        title_figure vitesse
        include_description_curves 0
        labelX "z (m)"
        labelY "v"

        width 18cm
        grid
        legend below title "Legende"
        nb_img_without_newline 2
#Pforeach _dis_ ( _list_dis_ )
        Curve
        {
            legend _dis_
            segment ./_dis_/maillage_20x30/jdd V
            columns 1 3
            style lines
            Linewidth 2
        }
#Pendforeach(_dis_)
        # Curve
        # {
        #     legend Analytique
        #     segment ./VDF/jdd V_ex
        #     columns 1 3
        #     style points
        # }
    }
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
