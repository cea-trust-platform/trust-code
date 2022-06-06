/****************************************************************************
* Copyright (c) 2022, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <Op_Dift_VDF_Face_Axi_base.h>
#include <Mod_turb_hyd_base.h>

Implemente_base(Op_Dift_VDF_Face_Axi_base,"Op_Dift_VDF_Face_Axi_base",Op_Dift_VDF_Face_base);

Sortie& Op_Dift_VDF_Face_Axi_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_VDF_Face_Axi_base::readOn(Entree& s ) { return s ; }

void Op_Dift_VDF_Face_Axi_base::associer(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis, const Champ_Inc& ch_transporte)
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  const Champ_Face& inco = ref_cast(Champ_Face,ch_transporte.valeur());
  la_zone_vdf = zvdf;
  la_zcl_vdf = zclvdf;
  inconnue = inco;
  surface.ref(zvdf.face_surfaces());
  volumes_entrelaces.ref(zvdf.volumes_entrelaces());
  orientation.ref(zvdf.orientation());
  porosite.ref(zvdf.porosite_face());
  xp.ref(zvdf.xp());
  xv.ref(zvdf.xv());
  Qdm.ref(zvdf.Qdm());
  face_voisins.ref(zvdf.face_voisins());
  elem_faces.ref(zvdf.elem_faces());
  type_arete_bord.ref(zclvdf.type_arete_bord());
}

void Op_Dift_VDF_Face_Axi_base::completer()
{
  Op_Dift_VDF_base::completer();
  Equation_base& eqn_hydr = equation();
  Champ_Face& vitesse = ref_cast(Champ_Face,eqn_hydr.inconnue().valeur());
  vitesse.dimensionner_tenseur_Grad();
  const RefObjU& modele_turbulence = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  associer_modele_turbulence(mod_turb);
}

double Op_Dift_VDF_Face_Axi_base::calculer_dt_stab() const
{
  return Op_Dift_VDF_Face_base::calculer_dt_stab(la_zone_vdf.valeur()) ;
}

void Op_Dift_VDF_Face_Axi_base::mettre_a_jour(double )
{
  if (le_modele_turbulence->loi_paroi().non_nul()) tau_tan.ref(le_modele_turbulence->loi_paroi()->Cisaillement_paroi());
}

// XXX E Saikali : j'ai fait comme ca sinon nu_t est pas initialiser dans le cas var
void Op_Dift_VDF_Face_Axi_base::associer_modele_turbulence(const Mod_turb_hyd_base& mod)
{
  le_modele_turbulence = mod;
  associer_diffusivite_turbulente(le_modele_turbulence->viscosite_turbulente());
  associer_loipar(le_modele_turbulence->loi_paroi()); /* on fait rien pour le moment ... */
}

void Op_Dift_VDF_Face_Axi_base::ajouter_elem(const DoubleVect& visco_turb, const DoubleTab& tau_diag, DoubleTab& resu) const
{
  for (int num_elem = 0; num_elem < la_zone_vdf->nb_elem(); num_elem++)
    {
      const int fx0 = elem_faces(num_elem,0), fx1 = elem_faces(num_elem,dimension), fy0 = elem_faces(num_elem,1), fy1 = elem_faces(num_elem,1+dimension);
      const double visc_elem = nu_(num_elem) + 2*visco_turb(num_elem);
      const double flux_X = (visc_elem*tau_diag(num_elem,0))*0.5*(surface(fx0)+surface(fx1)), flux_Y = (visc_elem*tau_diag(num_elem,1))*0.5*(surface(fy0)+surface(fy1));
      resu(fx0) += flux_X;
      resu(fx1) += flux_X; // TODO FIXME EUH .?.?. Yannick help :/
      resu(fy0) += flux_Y;
      resu(fy1) += flux_Y; // TODO FIXME EUH .?.?. Yannick help :/

      // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
      const double coef_laplacien_axi = -0.5*(tau_diag(num_elem,1)*visc_elem);
      resu[fx0] += coef_laplacien_axi*volumes_entrelaces(fx0)*porosite(fx0)/xv(fx0,0);
      resu[fx1] += coef_laplacien_axi*volumes_entrelaces(fx1)*porosite(fx1)/xv(fx1,0);
    }
}

void Op_Dift_VDF_Face_Axi_base::ajouter_elem_3D(const DoubleVect& visco_turb, const DoubleTab& tau_diag, DoubleTab& resu) const
{
  for (int num_elem = 0; num_elem < la_zone_vdf->nb_elem(); num_elem++)
    {
      const int fz0 = elem_faces(num_elem,2), fz1 = elem_faces(num_elem,2+dimension);
      const double visc_elem = nu_(num_elem) + 2*visco_turb(num_elem), flux_Z = (visc_elem*tau_diag(num_elem,2))*0.5*(surface(fz0)+surface(fz1));
      resu[fz0] += flux_Z;
      resu[fz1] -= flux_Z;
    }
}

void Op_Dift_VDF_Face_Axi_base::ajouter_aretes_bords(const DoubleVect& visco_turb, const DoubleTab& tau_croises, DoubleTab& resu) const
{
  const int ndeb = la_zone_vdf->premiere_arete_bord(), nfin = ndeb + la_zone_vdf->nb_aretes_bord();
  double d_visco_turb, d_visco_lam;
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int n_type = type_arete_bord(n_arete-ndeb);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI: // paroi-paroi
          {
            const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), ori3 = orientation(fac3);
            const int rang1 = (fac1 - la_zone_vdf->premiere_face_bord()), rang2 = (fac2-la_zone_vdf->premiere_face_bord());
            double coef;
            if (is_VAR()) // XXX : E Saikali : sais pas quoi faire sinon ecarts ...
              {
                // Calcul du frottement identique a celui de TRIOVF : On calcule la moyenne des u_star et on l'eleve au carre. On calcule la moyenne des surfaces
                const double tau_tan_1 = tau_tan(rang1,ori3), tau_tan_2 = tau_tan(rang2,ori3) ;
                double tau = 0.5*(tau_tan_1 + tau_tan_2 ), surf = 0.5*(surface(fac1)+surface(fac2));
                coef = tau*tau*surf;
              }
            else // Autre solution pour le calcul du frottement : On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
              {
                const double tau1 = tau_tan(rang1,ori3)*0.5*surface(fac1), tau2 = tau_tan(rang2,ori3)*0.5*surface(fac2);
                coef = tau1+tau2;
              }

            resu[fac3] += coef;
            break;
          }
        case TypeAreteBordVDF::FLUIDE_FLUIDE: /* fall through */
        case TypeAreteBordVDF::PAROI_FLUIDE:
          {
            const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe  = Qdm(n_arete,3), ori1b = orientation(fac1), ori3b = orientation(fac3);
            d_visco_turb = 0.5*(visco_turb(face_voisins(fac3,0)) + visco_turb(face_voisins(fac3,1)));
            d_visco_lam = nu_mean_2_pts_(face_voisins(fac3,0),face_voisins(fac3,1));

            if (ori1b == 0) // bord d'equation R = cte
              {
                double flux1;
                if (ori3b == 1)
                  {
                    const double tau12 = tau_croises(n_arete,0), tau21 = tau_croises(n_arete,1);
                    flux1 = (d_visco_lam*tau12 + d_visco_turb*(tau12+tau21))*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 2)
                  {
                    assert (ori3b == 2) ;
                    const double tau13 = tau_croises(n_arete,0), tau31 = tau_croises(n_arete,1);
                    flux1 = (d_visco_lam*tau13 + d_visco_turb*(tau13+tau31))*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }

                resu[fac3] += signe*flux1;
              }
            else if (ori1b == 1) // bord d'equation teta = cte
              {
                double flux2;
                if (ori3b == 0)
                  {
                    const double tau21 = tau_croises(n_arete,0), tau12 = tau_croises(n_arete,1);
                    flux2 = (d_visco_lam*tau21 + d_visco_turb*(tau12+tau21))*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
                    const double coef_laplacien_axi = 0.5*(d_visco_lam*tau21 + d_visco_turb*(tau12+tau21));
                    resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
                    resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
                  }
                else // if (ori3b == 2) flux de tau23 a travers le bord
                  {
                    assert (ori3b == 2);
                    const double tau23 = tau_croises(n_arete,0), tau32 = tau_croises(n_arete,1);
                    flux2 = (d_visco_lam*tau23 + d_visco_turb*(tau23+tau32))*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }

                resu[fac3] += signe*flux2;
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                double flux3;
                if (ori3b == 0)
                  {
                    const double tau31 = tau_croises(n_arete,0), tau13 = tau_croises(n_arete,1);
                    flux3 = (d_visco_lam*tau31 + d_visco_turb*(tau13+tau31))*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 1)  flux de tau32 a travers le bord
                  {
                    assert(ori3b == 1);
                    const double tau32 = tau_croises(n_arete,0), tau23 = tau_croises(n_arete,1);
                    flux3 = (d_visco_lam*tau32 + d_visco_turb*(tau23+tau32))*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }

                resu[fac3] += signe*flux3;
              }
            break;
          }
        case TypeAreteBordVDF::SYM_SYM:
          break;
        default :
          {
            Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
            Process::exit();
            break;
          }
        }
    }
}

void Op_Dift_VDF_Face_Axi_base::fill_resu_aretes_mixtes(const int i , const int j , const int k , const int l , const double d_visco_lam , const double tau , DoubleTab& resu) const
{
  // flux de mu_lam*tau sur la facette a cheval sur les faces i et j
  const double flux = d_visco_lam*tau*0.25*(surface(i)+surface(j))*(porosite(i)+porosite(j));
  resu(k) += flux;
  resu(l) -= flux;
}

void Op_Dift_VDF_Face_Axi_base::ajouter_aretes_mixtes(const DoubleTab& tau_croises, DoubleTab& resu) const
{
  // Sur les aretes mixtes les termes croises du tenseur de Reynolds sont nuls: il ne reste donc que la diffusion laminaire
  const int ndeb = la_zone_vdf->premiere_arete_mixte(), nfin = la_zone_vdf->premiere_arete_interne();
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int fac1=Qdm(n_arete,0), fac2=Qdm(n_arete,1), fac3=Qdm(n_arete,2), fac4=Qdm(n_arete,3), ori1 = orientation(fac1), ori3 = orientation(fac3);
      const double d_visco_lam = nu_mean_4_pts_(fac3,fac4); // XXX : BUG corrige dans cette fonction si ecart ...

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          const double tau12 = tau_croises(n_arete,0), tau21 = tau_croises(n_arete,1);
          fill_resu_aretes_mixtes(fac1,fac2,fac3,fac4,d_visco_lam,tau21,resu);

          // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
          const double coef_laplacien_axi = 0.5*d_visco_lam*tau21;
          resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
          resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

          fill_resu_aretes_mixtes(fac3,fac4,fac1,fac2,d_visco_lam,tau12,resu);
        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {
          const double tau23 = tau_croises(n_arete,0), tau32 = tau_croises(n_arete,1);
          fill_resu_aretes_mixtes(fac1,fac2,fac3,fac4,d_visco_lam,tau32,resu);
          fill_resu_aretes_mixtes(fac3,fac4,fac1,fac2,d_visco_lam,tau23,resu);
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {
          const double tau13 = tau_croises(n_arete,0), tau31 = tau_croises(n_arete,1);
          fill_resu_aretes_mixtes(fac1,fac2,fac3,fac4,d_visco_lam,tau31,resu);
          fill_resu_aretes_mixtes(fac3,fac4,fac1,fac2,d_visco_lam,tau13,resu);
        }
    }
}

void Op_Dift_VDF_Face_Axi_base::fill_resu_aretes_internes(const int i, const int j, const int k, const int l, const double v_lam, const double v_turb, const double tau1, const double tau2, DoubleTab& resu) const
{
  // flux de v_lam*tau2 + v_turb*(tau2+tau1) sur la facette a cheval sur les faces i et j
  const double flux = (v_lam*tau2 + v_turb*(tau2+tau1))*0.25*(surface(i)+surface(j))*(porosite(i)+porosite(j));
  resu(k) += flux;
  resu(l) -= flux;
}

void Op_Dift_VDF_Face_Axi_base::ajouter_aretes_internes(const DoubleVect& visco_turb, const DoubleTab& tau_croises, DoubleTab& resu) const
{
  const int ndeb = la_zone_vdf->premiere_arete_interne(), nfin = la_zone_vdf->nb_aretes();
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3), ori1 = orientation(fac1), ori3 = orientation(fac3);

      // Calcul de la viscosite turbulente au milieu de l'arete
      const double d_visco_turb = 0.25*(visco_turb(face_voisins(fac3,0)) + visco_turb(face_voisins(fac3,1)) + visco_turb(face_voisins(fac4,0)) + visco_turb(face_voisins(fac4,1)));
      const double d_visco_lam = nu_mean_4_pts_(face_voisins(fac3,0), face_voisins(fac3,1), face_voisins(fac4,0), face_voisins(fac4,1));

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          const double tau12 = tau_croises(n_arete,0), tau21 = tau_croises(n_arete,1);
          fill_resu_aretes_internes(fac1,fac2,fac3,fac4,d_visco_lam,d_visco_turb,tau12,tau21,resu);

          // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
          const double coef_laplacien_axi = 0.5*(d_visco_lam*tau21 + d_visco_turb*(tau21+tau12));
          resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
          resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

          fill_resu_aretes_internes(fac3,fac4,fac1,fac2,d_visco_lam,d_visco_turb,tau21,tau12,resu);
        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {
          const double tau23 = tau_croises(n_arete,0), tau32 = tau_croises(n_arete,1);
          fill_resu_aretes_internes(fac1,fac2,fac3,fac4,d_visco_lam,d_visco_turb,tau23,tau32,resu);
          fill_resu_aretes_internes(fac3,fac4,fac1,fac2,d_visco_lam,d_visco_turb,tau32,tau23,resu);
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {
          const double tau13 = tau_croises(n_arete,0), tau31 = tau_croises(n_arete,1);
          fill_resu_aretes_internes(fac1,fac2,fac3,fac4,d_visco_lam,d_visco_turb,tau13,tau31,resu);
          fill_resu_aretes_internes(fac3,fac4,fac1,fac2,d_visco_lam,d_visco_turb,tau31,tau13,resu);
        }
    }
}

DoubleTab& Op_Dift_VDF_Face_Axi_base::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  if (inco.line_size() > 1) not_implemented(__func__);
  const double temps = equation().schema_temps().temps_courant();
  mettre_a_jour_var(temps); // seulement pour var_axi !

  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  const DoubleVect& visco_turb = diffusivite_turbulente()->valeurs();
  const DoubleTab& tau_diag = inconnue->tau_diag(), &tau_croises = inconnue->tau_croises();
  ref_cast_non_const(Champ_Face,inconnue.valeur()).calculer_dercov_axi(zclvdf);

  ajouter_elem(visco_turb,tau_diag,resu); // Boucle sur les elements pour traiter les facettes situees a l'interieur des elements
  if (dimension == 3) ajouter_elem_3D(visco_turb,tau_diag,resu);
  ajouter_aretes_bords(visco_turb,tau_croises,resu); // Boucle sur les aretes bord
  ajouter_aretes_mixtes(tau_croises,resu); // Boucle sur les aretes mixtes
  ajouter_aretes_internes(visco_turb,tau_croises,resu); // Boucle sur les aretes internes

  return resu;
}

DoubleTab& Op_Dift_VDF_Face_Axi_base::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inco,resu);
}

void Op_Dift_VDF_Face_Axi_base::fill_coeff_matrice_morse(const int fac1, const int fac2, const double flux, Matrice_Morse& matrice) const
{
  const IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  for (int k = tab1[fac1]-1; k < tab1[fac1+1]-1; k++)
    {
      if (tab2[k]-1 == fac1) coeff[k] += flux;
      if (tab2[k]-1 == fac2) coeff[k] -= flux;
    }
  for (int k = tab1[fac2]-1; k < tab1[fac2+1]-1; k++)
    {
      if (tab2[k]-1 == fac1) coeff[k] -= flux;
      if (tab2[k]-1 == fac2) coeff[k] += flux;
    }
}

void Op_Dift_VDF_Face_Axi_base::ajouter_contribution_elem(const DoubleVect& visco_turb, const DoubleTab& tau_diag, Matrice_Morse& matrice) const
{
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  for (int num_elem = 0; num_elem < la_zone_vdf->nb_elem(); num_elem++)
    {
      const int fx0 = elem_faces(num_elem,0), fx1 = elem_faces(num_elem,dimension), fy0 = elem_faces(num_elem,1), fy1 = elem_faces(num_elem,1+dimension);
      const double visc_elem = nu_(num_elem) + 2*visco_turb(num_elem);
      double flux_X, flux_Y;

      if (is_VAR())
        {
          flux_X = (visc_elem*tau_diag(num_elem,0))*0.5*(surface(fx0)+surface(fx1));
          flux_Y = (visc_elem*tau_diag(num_elem,1))*0.5*(surface(fy0)+surface(fy1));
        }
      else
        {
          flux_X = visc_elem*0.5*(surface(fx0)+surface(fx1));
          flux_Y = visc_elem*0.5*(surface(fy0)+surface(fy1));
        }

      fill_coeff_matrice_morse(fx0,fx1,flux_X,matrice);
      fill_coeff_matrice_morse(fy0,fy1,flux_Y,matrice);

      // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
      double coef_laplacien_axi;
      if (is_VAR()) coef_laplacien_axi = -0.5*(tau_diag(num_elem,1)*visc_elem);
      else coef_laplacien_axi = -0.5*visc_elem; // XXX : comprends rien la

      for (int l = tab1[fx0]-1; l < tab1[fx0+1]-1; l++)
        if (tab2[l]-1 == fx0) coeff[l] += coef_laplacien_axi*volumes_entrelaces(fx0)*porosite(fx0)/xv(fx0,0);

      for (int l = tab1[fx1]-1; l < tab1[fx1+1]-1; l++)
        if (tab2[l]-1 == fx1) coeff[l] += coef_laplacien_axi*volumes_entrelaces(fx1)*porosite(fx1)/xv(fx1,0);
    }
}

void Op_Dift_VDF_Face_Axi_base::ajouter_contribution_elem_3D(const DoubleVect& visco_turb, const DoubleTab& tau_diag, Matrice_Morse& matrice) const
{
  for (int num_elem = 0; num_elem < la_zone_vdf->nb_elem(); num_elem++)
    {
      const int fz0 = elem_faces(num_elem,2), fz1 = elem_faces(num_elem,2+dimension);
      const double visc_elem = nu_(num_elem) + 2*visco_turb(num_elem);
      double flux_Z;

      if (is_VAR()) flux_Z = (visc_elem*tau_diag(num_elem,2))*0.5*(surface(fz0)+surface(fz1));
      else flux_Z = visc_elem*0.5*(surface(fz0)+surface(fz1));

      fill_coeff_matrice_morse(fz0,fz1,flux_Z,matrice);
    }
}

void Op_Dift_VDF_Face_Axi_base::ajouter_contribution_aretes_bords(const DoubleVect& visco_turb, const DoubleTab& tau_diag, Matrice_Morse& matrice) const
{
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  int ndeb = la_zone_vdf->premiere_arete_bord(), nfin = ndeb + la_zone_vdf->nb_aretes_bord();
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int n_type = type_arete_bord(n_arete-ndeb);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          break;
        case TypeAreteBordVDF::FLUIDE_FLUIDE: /* fall through */
        case TypeAreteBordVDF::PAROI_FLUIDE:
          {
            const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe  = Qdm(n_arete,3), ori1b = orientation(fac1), ori3b = orientation(fac3);
            const double d_visco_turb = 0.5*(visco_turb(face_voisins(fac3,0))+ visco_turb(face_voisins(fac3,1)));
            const double d_visco_lam = nu_mean_2_pts_(face_voisins(fac3,0),face_voisins(fac3,1));

            if (ori1b == 0) // bord d'equation R = cte
              {
                // XXX j'ai supprime le if
                const double flux1 = (d_visco_lam+ d_visco_turb)*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                for (int l = tab1[fac3]-1; l < tab1[fac3+1]-1; l++)
                  if (tab2[l]-1 == fac3) coeff[l] += signe*flux1;
              }
            else if (ori1b == 1) // bord d'equation teta = cte
              {
                if (ori3b == 0)
                  {
                    const double flux2 = (d_visco_lam + d_visco_turb)*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                    for (int l = tab1[fac3]-1; l < tab1[fac3+1]-1; l++)
                      if (tab2[l]-1 == fac3) coeff[l] += signe*flux2;

                    // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
                    const double coef_laplacien_axi = 0.5*(d_visco_lam + d_visco_turb);

                    for (int l = tab1[fac1]-1; l < tab1[fac1+1]-1; l++)
                      if (tab2[l]-1 == fac1) coeff[l] += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);

                    for (int l = tab1[fac2]-1; l < tab1[fac2+1]-1; l++)
                      if (tab2[l]-1 == fac2) coeff[l] += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
                  }
                else if (ori3b == 2) // flux de tau23 a travers le bord
                  {
                    const double flux3 = (d_visco_lam + d_visco_turb)*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                    for (int l = tab1[fac3]-1; l < tab1[fac3+1]-1; l++)
                      if (tab2[l]-1 == fac3) coeff[l] += signe*flux3;
                  }
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                const double flux4 = (d_visco_lam + d_visco_turb)*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                for (int l = tab1[fac3]-1; l < tab1[fac3+1]-1; l++)
                  if (tab2[l]-1 == fac3) coeff[l] += signe*flux4;
              }
            break;
          }
        case TypeAreteBordVDF::SYM_SYM:
          break;
        default :
          {
            Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
            Process::exit();
            break;
          }
        }
    }
}

void Op_Dift_VDF_Face_Axi_base::ajouter_contribution_aretes_mixtes(Matrice_Morse& matrice) const
{
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  // Sur les aretes mixtes les termes croises du tenseur de Reynolds sont nuls: il ne reste donc que la diffusion laminaire
  const int ndeb = la_zone_vdf->premiere_arete_mixte(), nfin = la_zone_vdf->premiere_arete_interne();
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3), ori1 = orientation(fac1);
      const double d_visco_lam = nu_mean_4_pts_(fac3,fac4); // XXX : BUG corrige dans cette fonction si ecart ...

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          double flux1;
          // flux de mu_lam*tau21 sur la facette a cheval sur les faces fac1 et fac2
          flux1 = d_visco_lam*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          fill_coeff_matrice_morse(fac3,fac4,flux1,matrice);

          // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
          const double coef_laplacien_axi = 0.5*d_visco_lam;

          for (int l = tab1[fac1]-1; l < tab1[fac1+1]-1; l++)
            if (tab2[l]-1 == fac1) coeff[l] += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);

          for (int l = tab1[fac2]-1; l < tab1[fac2+1]-1; l++)
            if (tab2[l]-1 == fac2) coeff[l] += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

          // flux de mu_lam*tau12 sur la facette a cheval sur les faces fac3 et fac4
          flux1 = d_visco_lam*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          fill_coeff_matrice_morse(fac1,fac2,flux1,matrice);
        }
      else  // 2 possibilites : ori1 = 2 et ori3 == 1: arete YZ OU  ori1 = 2 et ori3 = 0:  arete XZ
        {
          // flux de mu_lam*tau32 sur la facette a cheval sur les faces fac1 et fac2 ou
          // flux de mu_lam*tau31 sur la facette a cheval sur les faces fac1 et fac2

          double flux2;
          flux2 = d_visco_lam*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          fill_coeff_matrice_morse(fac3,fac4,flux2,matrice);

          // flux de mu_lam*tau23 sur la facette a cheval sur les faces fac3 et fac4 ou
          // flux de mu_lam*tau13 sur la facette a cheval sur les faces fac3 et fac4
          flux2 = d_visco_lam*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          fill_coeff_matrice_morse(fac1,fac2,flux2,matrice);
        }
    }
}

void Op_Dift_VDF_Face_Axi_base::ajouter_contribution_aretes_internes(const DoubleVect& visco_turb, Matrice_Morse& matrice) const
{
  IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  const int ndeb = la_zone_vdf->premiere_arete_interne(), nfin = la_zone_vdf->nb_aretes();
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3), ori1 = orientation(fac1);
      // Calcul de la viscosite turbulente au milieu de l'arete
      const double d_visco_turb = 0.25*(visco_turb(face_voisins(fac3,0)) + visco_turb(face_voisins(fac3,1)) + visco_turb(face_voisins(fac4,0)) + visco_turb(face_voisins(fac4,1)));
      const double d_visco_lam = nu_mean_4_pts_(face_voisins(fac3,0), face_voisins(fac3,1), face_voisins(fac4,0), face_voisins(fac4,1));

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          // flux de mu_lam*tau21 + mu_turb*(tau21+tau12) sur la facette a cheval sur les faces fac1 et fac2
          double flux1;
          flux1 = (d_visco_lam + d_visco_turb)*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          fill_coeff_matrice_morse(fac3,fac4,flux1,matrice);

          // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
          const double coef_laplacien_axi = 0.5*(d_visco_lam + d_visco_turb);

          for (int l = tab1[fac1]-1; l < tab1[fac1+1]-1; l++)
            if (tab2[l]-1 == fac1) coeff[l] += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);

          for (int l = tab1[fac2]-1; l < tab1[fac2+1]-1; l++)
            if (tab2[l]-1 == fac2) coeff[l] += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

          // flux de mu_lam*tau12 + mu_turb*(tau21+tau12) sur la facette a cheval sur les faces fac3 et fac4
          flux1 = (d_visco_lam + d_visco_turb)*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          fill_coeff_matrice_morse(fac1,fac2,flux1,matrice);
        }
      else // 2 possibilites : ori1 = 2 et ori3 == 1: arete YZ OU  ori1 = 2 et ori3 = 0:  arete XZ
        {
          double flux2;
          // flux de  mu_lam*tau31 + mu_turb*(tau31+tau13) sur la facette a cheval sur les faces fac1 et fac2
          flux2 = (d_visco_lam+ d_visco_turb)*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          fill_coeff_matrice_morse(fac3,fac4,flux2,matrice);

          // flux de mu_lam*tau13 + mu_turb*(tau13+tau31) sur la facette a cheval sur les faces fac3 et fac4
          flux2 = (d_visco_lam + d_visco_turb)*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          fill_coeff_matrice_morse(fac1,fac2,flux2,matrice);
        }
    }
}

void Op_Dift_VDF_Face_Axi_base::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  if (inco.line_size() > 1) not_implemented(__func__);
  const double temps = equation().schema_temps().temps_courant();
  mettre_a_jour_var(temps); // seulement pour var_axi !

  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  const DoubleVect& visco_turb = diffusivite_turbulente()->valeurs();
  const DoubleTab& tau_diag = inconnue->tau_diag();
  ref_cast_non_const(Champ_Face,inconnue.valeur()).calculer_dercov_axi(zclvdf);

  ajouter_contribution_elem(visco_turb,tau_diag,matrice); // Boucle sur les elements pour traiter les facettes situees a l'interieur des elements
  if (dimension == 3) ajouter_contribution_elem_3D(visco_turb,tau_diag,matrice);
  ajouter_contribution_aretes_bords(visco_turb,tau_diag,matrice); // Boucle sur les aretes bord
  ajouter_contribution_aretes_mixtes(matrice); // Boucle sur les aretes mixtes
  ajouter_contribution_aretes_internes(visco_turb,matrice); // Boucle sur les aretes internes
}

void Op_Dift_VDF_Face_Axi_base::contribue_au_second_membre(DoubleTab& resu ) const
{
  if (resu.line_size() > 1) not_implemented(__func__);
  const double temps = equation().schema_temps().temps_courant();
  mettre_a_jour_var(temps); // seulement pour var_axi !

  const int ndeb = la_zone_vdf->premiere_arete_bord(), nfin = ndeb + la_zone_vdf->nb_aretes_bord();
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int n_type = type_arete_bord(n_arete-ndeb);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          {
            const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), ori3 = orientation(fac3);
            const int rang1 = (fac1 - la_zone_vdf->premiere_face_bord()), rang2 = (fac2 - la_zone_vdf->premiere_face_bord());
            double coef;
            if (is_VAR())
              {
                // Calcul du frottement identique a celui de TRIOVF : On calcule la moyenne des u_star et on l'eleve au carre. On calcule la moyenne des surfaces
                const double tau = 0.5*(sqrt(tau_tan(rang1,ori3)) + sqrt(tau_tan(rang2,ori3))), surf = 0.5*(surface(fac1)+surface(fac2));
                coef = tau*tau*surf;
              }
            else // Autre solution pour le calcul du frottement : On calcule u_star*u_star*surf sur chaque partie de la facette de Qdm
              {
                const double tau1 = tau_tan(rang1,ori3)*0.5*surface(fac1), tau2 = tau_tan(rang2,ori3)*0.5*surface(fac2);
                coef = tau1+tau2;
              }

            resu[fac3] += coef;
            break;
          }
        case TypeAreteBordVDF::FLUIDE_FLUIDE: /* fall through */
        case TypeAreteBordVDF::PAROI_FLUIDE:
        case TypeAreteBordVDF::SYM_SYM:
          break;
        default :
          {
            Cerr << "On a rencontre un type d'arete non prevu : [ num arete : " << n_arete << " ], [ type : " << n_type << " ]" << finl;
            Process::exit();
            break;
          }
        }
    }
}
