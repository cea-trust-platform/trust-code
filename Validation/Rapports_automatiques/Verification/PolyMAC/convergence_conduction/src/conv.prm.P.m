Parameters
{
    Title "Tests de convergence de l'operateur de diffusion aux elements"
#Pset(_listmesh_ %meshes% )
#Pset(_listdisplay_ 1 2 3 )
#Pset(_listdisplayT_ 1 2 3 )
# bloc testcases #

    Author "YG"
    IncludeData 2
    Prerequisite "./extract_convergence"
    Description "Cas test avec cl de Dirichlet, dont la solution analytique est un produit de sinus."
}

#Pforeach _mesh_ (_listmesh_)
Chapter
{
    Title Maillages _mesh_
    #Pforeach _resol_ (_listdisplay_)
    visu
    {
        normal3D 0. 0. 1.
        # Title "Maillage _mesh_"
        mesh _mesh_/jdd__resol_/cas.lata dom
        Description ""
        nb_img_without_newline 3
        width 5cm
    }
    #Pendforeach(_resol_)
}
#Pendforeach(_mesh_)

Chapter
{
    Title Convergence en maillage en vitesse
    Figure
    {
        labelx N
        labely erreur max
        logx
        logy
        Include_Description_Curves 0
        width 12cm
        Title "Ordres de convergence"
        #Pforeach _mesh_ (_listmesh_)
        Curve
        {
            legend _mesh_
            file _mesh_/conv_glob
            colonnes (1./sqrt($1)) $2
            style linespoints
        }
        #Pendforeach(_mesh_)
        Curve
        {
            legend Order 1
            fonction x
            style lines
        }
        Curve
        {
            legend Order 2
            fonction x*x
            style lines
        }
    }

    #Pforeach _mesh_ (_listmesh_)
    Figure
    {
        labelx diag
        labely solution
        Include_Description_Curves 0
        Title "Convergence de la solution sur maillage _mesh_"
        #Pforeach _resol_ (_listdisplayT_)
        Curve
        {
            legend "_mesh_,_resol_"
            segment _mesh_/jdd__resol_/cas temperature_diag
            style linespoints
        }
        #Pendforeach(_resol_)
    }
    #Pendforeach(_mesh_)
}
