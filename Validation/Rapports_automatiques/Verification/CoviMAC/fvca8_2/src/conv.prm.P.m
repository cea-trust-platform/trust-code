Parameters
{
    Title "Benchmark FVCA8"
#Pset(_listmesh2d_ %meshes_2d% )
#Pset(_listmesh3d_ %meshes_3d% )
# bloc testcases #

    Author "A. Gerschenfeld, Y. Gorsse"
    IncludeData 2
    Prerequisite "./extract_convergence"
    Description "Cas tests de Stokes 2D sur les maillages FVCA8, et Navier-Stokes 3D sur maillages FVCA8-3D et FVCA6-3D"
}

Chapter
{
    Title "Stokes stationnaire 2D : Bercovier-Engelman."


    #Pforeach _mesh_ (_listmesh2d_)
    visu
    {
        normal3D 0. 0. 1.
        mesh _mesh_/jdd_1/cas.lata dom
        Description "_mesh_"
        nb_img_without_newline 2
        width 7cm
    }
    #Pendforeach(_mesh_)

    Figure
    {
        labelx N
        labely erreur max
        logx
        logy
        Include_Description_Curves 0
        width 12cm
        Title "Ordres de convergence vitesse"
        #Pforeach _mesh_ (_listmesh2d_)
        Curve
        {
            legend _mesh_
            file _mesh_/conv_glob_v
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

    Figure
    {
        labelx N
        labely erreur max
        logx
        logy
        Include_Description_Curves 0
        width 12cm
        Title "Ordres de convergence pression"
        #Pforeach _mesh_ (_listmesh2d_)
        Curve
        {
            legend _mesh_
            file _mesh_/conv_glob_p
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
}

Chapter
{
    Title "Navier-Stokes stationnaire 3D"
    Description ""

    #Pforeach _mesh_ (_listmesh3d_)
    visu
    {
        normal3D 0. 0. 1.
        #Pif ( "_mesh_" == "Kershaw" ) mesh _mesh_/jdd_2/cas.lata dom
        #Pelse mesh _mesh_/jdd_1/cas.lata dom #Pendif
        Description "_mesh_"
        nb_img_without_newline 3
        width 5cm
    }
    #Pendforeach(_mesh_)

    Figure
    {
        labelx N
        labely erreur max
        logx
        logy
        Include_Description_Curves 0
        width 12cm
        Title "Ordres de convergence vitesse"

        #Pforeach _mesh_ (_listmesh3d_)
        Curve
        {
            legend _mesh_
            file _mesh_/conv_glob_v
            colonnes ($1**-(1./3)) $2
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

    Figure
    {
        labelx N
        labely erreur max
        logx
        logy
        Include_Description_Curves 0
        width 12cm
        Title "Ordres de convergence pression"
        #Pforeach _mesh_ (_listmesh3d_)
        Curve
        {
            legend _mesh_
            file _mesh_/conv_glob_p
            colonnes ($1**-(1./3)) $2
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
}
