Parameters
{
    Title "Sedimentation"
#Pset(_listmesh2d_ %meshes_2d% )
# bloc testcases #

    Author "A. Gerschenfeld, Y. Gorsse"
    IncludeData 2
    Description "Cas tests de sedimentation. Etat initial melange, et separation des phases par gravite."
}

Chapter
{
    Title "Resultats"

#Pforeach _mesh_ (_listmesh2d_)
    Figure
    {
        title_figure "Alpha, mesh _mesh_"
        include_description_curves 0
        nb_img_without_newline 2
        labelx "y [m]"
        labely "alpha [-]"
        width 7.5cm
        #legend below title "Legende"
#Pforeach _t_ (0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1 1.5 2 3 4 5 6 7 8 9 10)
        Curve
        {
            legend ""
            segment _mesh_/cas.data alpha _t_
            columns 1 2
            Style lines
        }
#Pendforeach(_t_)
    }
    Figure
    {
        title_figure "Vg, mesh _mesh_"
        include_description_curves 0
        nb_img_without_newline 2
        width 7.5cm
        labelx "y [m]"
        labely "vg [m/s]"
        #legend below title "Legende"
        rangey -0.2 0.2
#Pforeach _t_ (0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1 1.5 2 3 4 5 6 7 8 9 10)
        Curve
        {
            legend ""
            segment _mesh_/cas.data profil_vg _t_
            columns 1 3
            Style lines
        }
#Pendforeach(_t_)
    }
#Pendforeach(_mesh_)
}