Parameters
{
    Titre "Tubes a choc"
    Description "Issus du chapitre 10 du livre de Eleuterio F. Toro, Riemann Solvers and Numerical Methods for Fluid Dynamics, Third edition, Springer, 2009. \\ \\ Courament utilisés comme benchmark pour le comportement de méthodes numériques dans les ouvrages et publications scientifiques, ainsi que pour la vérification du comportement de certains codes.\\"
    Description "\latex_(Objectifs : montrer la la robutesse du code et évaluer \begin{enumerate} \item l'apparition d'oscillations parasites, \item la propagation des ondes avec la bonne vitesse, \item la capture des états intermédiaires à la bonne hauteur.\end{enumerate}\latex_)"
    IncludeData 0

    Auteur "Y. GORSSE, M. NDJINGA"
    code TRUST

#Pset(_list_meshes_ 100 800 )
#Pset(_list_tests_ Toro1 PWR1 PWR2 PWR3 )

#Pforeach _v_ (_list_tests_)
#Pforeach _d_ (_list_meshes_)
    TestCase n_d_ _v_.data
#Pendforeach(_d_)
#Pendforeach(_v_)
}

Chapter
{
    Title "Description des cas tests"

    Description "\latex_(These tests are designed to assess the robustness and accuracy of numerical methods at the core of the solver, independently from the boundary condition, and source term treatment involving correlations.\\ They consist in the numerical resolution of the Shock tube problem for a perfect gas with $\gamma=1.4$. \\ The initial state consists in two constant states $W_L=(\rho_L,u_L,p_L)$ and $W_R=(\rho_R,u_R,p_R)$ separated by a discontinuity at $x=x_d$. The following table gives the values of $W_L$ and $W_R$ for each test.\latex_)"

    Description "\latex_(\begin{center}\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|}\hline Test&$gamma$&$p_\infty$&$\rho_L$&$u_L$&$p_L$&$\rho_R$&$u_R$&$p_R$&$x_d$\\__tabular__\hline\end{tabular}\end{center}%\caption{Initial data for the seven shock tube tests}\latex_)"

}

#Pforeach _v_ (_list_tests_)
Chapter
{
    Title "Cas test _v_"

#Pforeach _p_ ( RHO P V EINT )
    figure
    {
        title_figure Profil de _p_
        include_description_curves 0
        legend left Left reverse
        labelX "z[m]"
        labelY _p_
        nb_img_without_newline 2
        width 7.5cm
#Pforeach _d_ (_list_meshes_)
        curve
        {
            segment n_d_/_v_ _p_
            legend Trust - N=_d_
            style lines
            linewidth 1.5
        }
#Pendforeach(_d_)
        Curve
        {
            file _v_.ex
            columns "x" "_p_"
            legend "Exact solution"
            style lines
            typeligne -1
            linewidth 2
        }
    }
#Pendforeach(_p_)
}
#Pendforeach(_v_)
