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

#include <Op_Diff_VDF_Face_Axi_base.h>

Implemente_base(Op_Diff_VDF_Face_Axi_base,"Op_Diff_VDF_Face_Axi_base",Op_Diff_VDF_Face_base);

Sortie& Op_Diff_VDF_Face_Axi_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Diff_VDF_Face_Axi_base::readOn(Entree& s ) { return s ; }

void Op_Diff_VDF_Face_Axi_base::associer(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis, const Champ_Inc& ch_transporte)
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

double Op_Diff_VDF_Face_Axi_base::calculer_dt_stab() const
{
  return Op_Diff_VDF_Face_base::calculer_dt_stab(la_zone_vdf.valeur()) ;
}

void Op_Diff_VDF_Face_Axi_base::ajouter_elem(const DoubleTab& inco, DoubleTab& resu) const
{
  if (inco.line_size() > 1) not_implemented(__func__);
  for (int num_elem = 0; num_elem < la_zone_vdf->nb_elem(); num_elem++)
    {
      const int fx0 = elem_faces(num_elem,0), fx1 = elem_faces(num_elem,dimension), fy0 = elem_faces(num_elem,1), fy1 = elem_faces(num_elem,1+dimension);
      // Calcul de tau11
      const double tau11 = (inco[fx1]-inco[fx0])/(xv(fx1,0) - xv(fx0,0));
      // Calcul de tau22
      double R = xp(num_elem,0), d_teta = xv(fy1,1) - xv(fy0,1);
      if (d_teta < 0) d_teta += deux_pi;
      double tau22 =  (inco[fy1]-inco[fy0])/(R*d_teta);
      tau22 += 0.5*(inco[fx0]+inco[fx1])/R; // termes supplementaires en axi

      const double flux_X = tau11*nu_(num_elem)*0.5*(surface(fx0)+surface(fx1)), flux_Y = tau22*nu_(num_elem)*0.5*(surface(fy0)+surface(fy1));
      resu[fx0] += flux_X;
      resu[fx1] -= flux_X;
      resu[fy0] += flux_Y;
      resu[fy1] -= flux_Y;

      // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
      const double coef_laplacien_axi = +0.5*tau22*nu_(num_elem);
      resu[fx0] -= coef_laplacien_axi*volumes_entrelaces(fx0)*porosite(fx0)/xv(fx0,0);
      resu[fx1] -= coef_laplacien_axi*volumes_entrelaces(fx1)*porosite(fx1)/xv(fx1,0);
    }
}

void Op_Diff_VDF_Face_Axi_base::ajouter_elem_3D(const DoubleTab& inco, DoubleTab& resu) const
{
  for (int num_elem = 0; num_elem < la_zone_vdf->nb_elem(); num_elem++)
    {
      const int fz0 = elem_faces(num_elem,2), fz1 = elem_faces(num_elem,2+dimension);
      // Calcul de tau33
      const double tau33 = (inco[fz1]-inco[fz0])/(xv(fz1,2) - xv(fz0,2)), flux_Z = tau33*nu_(num_elem)*0.5*(surface(fz0)+surface(fz1));
      resu[fz0] += flux_Z;
      resu[fz1] -= flux_Z;
    }
}

void  Op_Diff_VDF_Face_Axi_base::ajouter_aretes_bords(const DoubleTab& inco, DoubleTab& resu) const
{
  int ndeb = la_zone_vdf->premiere_arete_bord(), nfin = ndeb + la_zone_vdf->nb_aretes_bord();
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int n_type = type_arete_bord(n_arete-ndeb);

      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI: /* fall through */
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
        case TypeAreteBordVDF::PAROI_FLUIDE:
          {
            const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe  = Qdm(n_arete,3), ori1 = orientation(fac1), ori3 = orientation(fac3);
            const int rang1 = fac1 - la_zone_vdf->premiere_face_bord(), rang2 = fac2 - la_zone_vdf->premiere_face_bord();
            double vit_imp, dist3, tps = inconnue->temps();

            if (n_type == TypeAreteBordVDF::PAROI_FLUIDE) // arete paroi_fluide :il faut determiner qui est la face fluide
              {
                if (est_egal(inco[fac1],0)) vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang2,ori3,la_zcl_vdf);
                else vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang1,ori3,la_zcl_vdf);
              }
            else vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(tps,rang1,ori3,la_zcl_vdf)+Champ_Face_get_val_imp_face_bord(tps,rang2,ori3,la_zcl_vdf));

            const double db_diffusivite = nu_mean_2_pts_(face_voisins(fac3,0),face_voisins(fac3,1));

            if (ori1 == 0) // bord d'equation R = cte
              {
                double flux1;
                if (ori3 == 1)  // flux de tau12 a travers le bord
                  {
                    dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;

                    const double tau12 = (inco[fac3]-vit_imp)/dist3;
                    flux1 = db_diffusivite*tau12*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 2)  flux de tau13 a travers le bord
                  {
                    assert(ori3 == 2);
                    dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                    const double tau13 = (inco[fac3]-vit_imp)/dist3;
                    flux1 = db_diffusivite*tau13*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                resu[fac3] += signe*flux1;
              }
            else if (ori1 == 1) // bord d'equation teta = cte
              {
                double R = xv(fac3,0), d_teta = xv(fac3,1) - xv(fac1,1);
                if (d_teta < 0) d_teta += deux_pi;

                dist3  = R*d_teta;
                if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                if (ori3 == 0) // flux de tau21 a travers le bord
                  {
                    double tau21 = (inco[fac3]-vit_imp)/dist3;
                    tau21 -= 0.5*(inco[fac1]+inco[fac2])/R; // Terme supplementaire en axi
                    const double flux2 = db_diffusivite*tau21*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                    resu[fac3]+=signe*flux2;

                    // Termes supplementaires dans le laplacien en axi
                    // Ils sont integres comme des termes sources
                    const double coef_laplacien_axi = 0.5*db_diffusivite*tau21;
                    resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
                    resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
                  }
                else if (ori3 == 2) // flux de tau23 a travers le bord
                  {
                    // XXX : attention si ecart : dans le cas constant c'etait tau23 = signe*(vit_imp-inco[fac3])/dist3 (normalement c'est la meme mais bon)
                    const double tau23 = (inco[fac3]-vit_imp)/dist3;
                    const double flux3 = db_diffusivite*tau23*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                    resu[fac3] += signe*flux3;
                  }
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                double flux4;
                if (ori3 == 0) // flux de tau31 a travers le bord
                  {
                    dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;

                    const double tau31 = (inco[fac3]-vit_imp)/dist3;
                    flux4 = db_diffusivite*tau31*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else // if (ori3 == 1)  flux de tau32 a travers le bord
                  {
                    assert(ori3 == 1) ;
                    dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;

                    const double tau32 = (inco[fac3]-vit_imp)/dist3;
                    flux4 = db_diffusivite*tau32*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                resu[fac3] += signe*flux4;
              }
            break;
          }
        case TypeAreteBordVDF::SYM_SYM: // pas de flux diffusif calcule
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

void Op_Diff_VDF_Face_Axi_base::ajouter_aretes_mixtes_internes(const DoubleTab& inco, DoubleTab& resu) const
{
  const int ndeb = la_zone_vdf->premiere_arete_mixte(), nfin = la_zone_vdf->nb_aretes();
  for (int n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3), ori1 = orientation(fac1), ori3 = orientation(fac3);
      const double db_diffusivite = nu_mean_4_pts_(fac3,fac4);

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          double flux1;
          // Calcul de tau21
          const double R = xv(fac3,0);
          double d_teta = xv(fac4,1) - xv(fac3,1);
          if (d_teta < 0) d_teta += deux_pi;
          double tau21 = (inco(fac4)-inco(fac3))/(R*d_teta);

          // Terme supplementaire en axi
          tau21 -= 0.5*(inco[fac1]+inco[fac2])/R;

          // flux de tau21 sur la facette a cheval sur les faces fac1 et fac2
          flux1 = db_diffusivite*tau21*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          resu(fac3) += flux1;
          resu(fac4) -= flux1;

          // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
          const double coef_laplacien_axi = 0.5*db_diffusivite*tau21;
          resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
          resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

          // Calcul de tau12
          const double tau12 = (inco(fac2)-inco(fac1))/(xv(fac2,0) - xv(fac1,0));
          // flux de tau12 sur la facette a cheval sur les faces fac3 et fac4
          flux1 = db_diffusivite*tau12*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          resu(fac1) += flux1;
          resu(fac2) -= flux1;
        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {
          double flux2;
          // Calcul de tau32
          const double tau32 = (inco(fac4)-inco(fac3))/(xv(fac4,2) - xv(fac3,2));

          // flux de tau32 sur la facette a cheval sur les faces fac1 et fac2
          flux2 = db_diffusivite*tau32*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          resu(fac3) += flux2;
          resu(fac4) -= flux2;

          // Calcul de tau23
          const double R = xv(fac1,0);
          double d_teta = xv(fac2,1) - xv(fac1,1);
          if (d_teta < 0) d_teta += deux_pi;
          const double tau23 = (inco(fac2)-inco(fac1))/(R*d_teta);
          // flux de tau23 sur la facette a cheval sur les faces fac3 et fac4
          flux2 = db_diffusivite*tau23*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          resu(fac1) += flux2;
          resu(fac2) -= flux2;
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {
          double flux3;
          // Calcul de tau31
          const double tau31 = (inco(fac4)-inco(fac3))/(xv(fac4,2) - xv(fac3,2));

          // flux de tau31 sur la facette a cheval sur les faces fac1 et fac2
          flux3 = db_diffusivite*tau31*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          resu(fac3) += flux3;
          resu(fac4) -= flux3;

          // Calcul de tau13
          const double tau13 = (inco(fac2)-inco(fac1))/(xv(fac2,0) - xv(fac1,0));
          // flux de tau13 sur la facette a cheval sur les faces fac3 et fac4
          flux3 = db_diffusivite*tau13*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          resu(fac1) += flux3;
          resu(fac2) -= flux3;
        }
    }
}

DoubleTab& Op_Diff_VDF_Face_Axi_base::ajouter(const DoubleTab& inco,  DoubleTab& resu) const
{
  ajouter_elem(inco,resu); // Boucle sur les elements
  if (dimension == 3) ajouter_elem_3D(inco,resu); // Boucle sur les elements supplementaires si 3D
  ajouter_aretes_bords(inco, resu); // Boucle sur les aretes bord
  ajouter_aretes_mixtes_internes(inco, resu); // Boucle sur les aretes mixtes et internes
  return resu;
}

DoubleTab& Op_Diff_VDF_Face_Axi_base::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inco,resu);
}

void Op_Diff_VDF_Face_Axi_base::fill_coeff_matrice_morse(const int fac1, const int fac2, const double flux, Matrice_Morse& matrice) const
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

void Op_Diff_VDF_Face_Axi_base::ajouter_contribution_elem(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if (inco.line_size() > 1) not_implemented(__func__);

  const IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  for (int num_elem = 0; num_elem < la_zone_vdf->nb_elem(); num_elem++)
    {
      const int fx0 = elem_faces(num_elem,0), fx1 = elem_faces(num_elem,dimension), fy0 = elem_faces(num_elem,1), fy1 = elem_faces(num_elem,1+dimension);
      // Calcul de tau11
      const double tau11 = 1/(xv(fx1,0) - xv(fx0,0));

      // Calcul de tau22
      const double R = xp(num_elem,0);
      double d_teta = xv(fy1,1) - xv(fy0,1);
      if (d_teta < 0) d_teta += deux_pi;

      double tau22 = 1/(R*d_teta);
      // termes supplementaires en axi
      tau22 += 0.5/R;
      const double flux_X = tau11*nu_(num_elem)*0.5*(surface(fx0)+surface(fx1));
      const double flux_Y = tau22*nu_(num_elem)*0.5*(surface(fy0)+surface(fy1));
      fill_coeff_matrice_morse(fx0,fx1,flux_X,matrice);
      fill_coeff_matrice_morse(fy0,fy1,flux_Y,matrice);

      // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
      const double coef_laplacien_axi = +0.5*tau22*nu_(num_elem);

      for (int k = tab1[fx0]-1; k < tab1[fx0+1]-1; k++)
        if (tab2[k]-1 == fx0) coeff[k] += coef_laplacien_axi*volumes_entrelaces(fx0)*porosite(fx0)/xv(fx0,0);

      for (int k=tab1[fx1]-1; k<tab1[fx1+1]-1; k++)
        if (tab2[k]-1 == fx1) coeff[k] += coef_laplacien_axi*volumes_entrelaces(fx1)*porosite(fx1)/xv(fx1,0);
    }
}

void Op_Diff_VDF_Face_Axi_base::ajouter_contribution_elem_3D(Matrice_Morse& matrice) const
{
  for (int num_elem = 0; num_elem < la_zone_vdf->nb_elem(); num_elem++)
    {
      const int fz0 = elem_faces(num_elem,2), fz1 = elem_faces(num_elem,2+dimension);
      // Calcul de tau33
      const double tau33 = 1/(xv(fz1,2) - xv(fz0,2));
      const double flux_Z = tau33*nu_(num_elem)*0.5*(surface(fz0)+surface(fz1));
      fill_coeff_matrice_morse(fz0,fz1,flux_Z,matrice);
    }
}

void Op_Diff_VDF_Face_Axi_base::ajouter_contribution_aretes_bords(Matrice_Morse& matrice) const
{
  const IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  const int ndeb = la_zone_vdf->premiere_arete_bord(), nfin = ndeb + la_zone_vdf->nb_aretes_bord();
  for (int n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      const int n_type = type_arete_bord(n_arete-ndeb);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI: /* fall through */
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
        case TypeAreteBordVDF::PAROI_FLUIDE:
          {
            const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe  = Qdm(n_arete,3), ori1 = orientation(fac1), ori3 = orientation(fac3);
            const double db_diffusivite = nu_mean_2_pts_(face_voisins(fac3,0),face_voisins(fac3,1));

            if (ori1 == 0) // bord d'equation R = cte
              {
                double flux1;
                if (ori3 == 1)  // flux de tau12 a travers le bord
                  {
                    double dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                    const double tau12 = 1/dist3;
                    flux1 = db_diffusivite*tau12*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 2)  flux de tau13 a travers le bord
                  {
                    assert (ori3 == 2);
                    double dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                    const double tau13 =  1/dist3;
                    flux1 = db_diffusivite*tau13*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }

                for (int k = tab1[fac3]-1; k < tab1[fac3+1]-1; k++)
                  if (tab2[k]-1 == fac3) coeff[k] += signe*flux1;
              }
            else if (ori1 == 1) // bord d'equation teta = cte
              {
                const double R = xv(fac3,0);
                double d_teta = xv(fac3,1) - xv(fac1,1);
                if (d_teta < 0) d_teta += deux_pi;

                double dist3  = R*d_teta;
                if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;

                if (ori3 == 0) // flux de tau21 a travers le bord
                  {
                    double tau21 = 1/dist3;

                    // Terme supplementaire en axi
                    tau21 -= 0.5/R;

                    const double flux2 = db_diffusivite*tau21*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    for (int k = tab1[fac3]-1; k < tab1[fac3+1]-1; k++)
                      if (tab2[k]-1 == fac3) coeff[k] += signe*flux2;

                    // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
                    const double coef_laplacien_axi = 0.5*db_diffusivite*tau21;

                    for (int k = tab1[fac1]-1; k < tab1[fac1+1]-1; k++)
                      if (tab2[k]-1 == fac1) coeff[k] += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);

                    for (int k = tab1[fac2]-1; k < tab1[fac2+1]-1; k++)
                      if (tab2[k]-1 == fac2) coeff[k] += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
                  }
                else // if (ori3 == 2) flux de tau23 a travers le bord
                  {
                    assert(ori3 == 2);
                    const double tau23 = 1/dist3;
                    const double flux3 = db_diffusivite*tau23*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                    for (int k = tab1[fac3]-1; k < tab1[fac3+1]-1; k++)
                      if (tab2[k]-1 == fac3) coeff[k] += signe*flux3;
                  }
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                double flux4;
                if (ori3 == 0) // flux de tau31 a travers le bord
                  {
                    double dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                    const double tau31 = 1/dist3;
                    flux4 = db_diffusivite*tau31*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 1) flux de tau32 a travers le bord
                  {
                    assert(ori3 == 1);
                    double dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                    const double tau32 = 1/dist3;
                    flux4 = db_diffusivite*tau32*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }

                for (int k = tab1[fac3]-1; k < tab1[fac3+1]-1; k++)
                  if (tab2[k]-1 == fac3) coeff[k] += signe*flux4;
              }
            break;
          }
        case TypeAreteBordVDF::SYM_SYM: // pas de flux diffusif calcule
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

void Op_Diff_VDF_Face_Axi_base::ajouter_contribution_aretes_mixtes_internes(Matrice_Morse& matrice) const
{
  const IntVect& tab1 = matrice.get_set_tab1(), &tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();
  const int ndeb = la_zone_vdf->premiere_arete_mixte(), nfin = la_zone_vdf->nb_aretes();
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), fac4 = Qdm(n_arete,3), ori1 = orientation(fac1), ori3 = orientation(fac3);
      const double db_diffusivite = nu_mean_4_pts_(fac3,fac4);

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          double flux1;
          // Calcul de tau21
          const double R = xv(fac3,0);
          double d_teta = xv(fac4,1) - xv(fac3,1);
          if (d_teta < 0) d_teta += deux_pi;
          double tau21 = 1/(R*d_teta);
          // Terme supplementaire en axi
          tau21 -= 0.5/R;

          // flux de tau21 sur la facette a cheval sur les faces fac1 et fac2
          flux1 = db_diffusivite*tau21*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          fill_coeff_matrice_morse(fac3,fac4,flux1,matrice);

          // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
          const double coef_laplacien_axi = 0.5*db_diffusivite*tau21;
          for (int k = tab1[fac1]-1; k < tab1[fac1+1]-1; k++)
            if (tab2[k]-1 == fac1) coeff[k] += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);

          for (int k = tab1[fac2]-1; k < tab1[fac2+1]-1; k++)
            if (tab2[k]-1 == fac2) coeff[k] += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

          // Calcul de tau12
          const double tau12 = 1/(xv(fac2,0) - xv(fac1,0));

          // flux de tau12 sur la facette a cheval sur les faces fac3 et fac4
          flux1 = db_diffusivite*tau12*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          fill_coeff_matrice_morse(fac1,fac2,flux1,matrice);
        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {
          double flux2;
          // Calcul de tau32
          const double tau32 = 1/(xv(fac4,2) - xv(fac3,2));

          // flux de tau32 sur la facette a cheval sur les faces fac1 et fac2
          flux2 = db_diffusivite*tau32*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          fill_coeff_matrice_morse(fac3,fac4,flux2,matrice);

          // Calcul de tau23
          const double R = xv(fac1,0);
          double d_teta = xv(fac2,1) - xv(fac1,1);
          if (d_teta < 0) d_teta += deux_pi;
          const double tau23 = 1/(R*d_teta);

          // flux de tau23 sur la facette a cheval sur les faces fac3 et fac4
          flux2 = db_diffusivite*tau23*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          fill_coeff_matrice_morse(fac1,fac2,flux2,matrice);
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {
          double flux3;
          // Calcul de tau31
          const double tau31 = 1/(xv(fac4,2) - xv(fac3,2));

          // flux de tau31 sur la facette a cheval sur les faces fac1 et fac2
          flux3 = db_diffusivite*tau31*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          fill_coeff_matrice_morse(fac3,fac4,flux3,matrice);

          // Calcul de tau13
          const double tau13 = 1/(xv(fac2,0) - xv(fac1,0));

          // flux de tau13 sur la facette a cheval sur les faces fac3 et fac4
          flux3 = db_diffusivite*tau13*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          fill_coeff_matrice_morse(fac1,fac2,flux3,matrice);
        }
    }
}

void Op_Diff_VDF_Face_Axi_base::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  ajouter_contribution_elem(inco,matrice); // Boucle sur les elements
  if (dimension == 3) ajouter_contribution_elem_3D(matrice); // Boucle sur les elements supplementaires si 3D
  ajouter_contribution_aretes_bords(matrice); // Boucle sur les aretes bord
  ajouter_contribution_aretes_mixtes_internes(matrice); // Boucle sur les aretes mixtes et internes
}

void Op_Diff_VDF_Face_Axi_base::contribue_au_second_membre(DoubleTab& resu) const
{
  const int ndeb = la_zone_vdf->premiere_arete_bord(), nfin = ndeb + la_zone_vdf->nb_aretes_bord();
  for (int n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      const int n_type = type_arete_bord(n_arete-ndeb);
      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI: /* fall through */
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
        case TypeAreteBordVDF::PAROI_FLUIDE:
          {
            const int fac1 = Qdm(n_arete,0), fac2 = Qdm(n_arete,1), fac3 = Qdm(n_arete,2), signe  = Qdm(n_arete,3);
            const int ori1 = orientation(fac1), ori3 = orientation(fac3), rang1 = fac1 - la_zone_vdf->premiere_face_bord(), rang2 = fac2 - la_zone_vdf->premiere_face_bord();
            double vit_imp, tps = inconnue->temps();

            if (n_type == TypeAreteBordVDF::PAROI_FLUIDE) // arete paroi_fluide :il faut determiner qui est la face fluide
              {
                if (est_egal(inconnue.valeur()(fac1),0)) vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang2,ori3,la_zcl_vdf);
                else vit_imp = Champ_Face_get_val_imp_face_bord(tps,rang1,ori3,la_zcl_vdf);
              }
            else vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(tps,rang1,ori3,la_zcl_vdf)+Champ_Face_get_val_imp_face_bord(tps,rang2,ori3,la_zcl_vdf));

            const double db_diffusivite =  nu_mean_2_pts_(face_voisins(fac3,0),face_voisins(fac3,1));

            if (ori1 == 0) // bord d'equation R = cte
              {
                double flux1;
                if (ori3 == 1)  // flux de tau12 a travers le bord
                  {
                    double dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                    const double tau12 = (-vit_imp)/dist3;
                    flux1 = db_diffusivite*tau12*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else// if (ori3 == 2)  flux de tau13 a travers le bord
                  {
                    assert(ori3 == 2);
                    double dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                    const double tau13 = (-vit_imp)/dist3;
                    flux1 = db_diffusivite*tau13*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                resu[fac3] += signe*flux1;
              }
            else if (ori1 == 1) // bord d'equation teta = cte
              {
                const double R = xv(fac3,0);
                double d_teta = xv(fac3,1) - xv(fac1,1);
                if (d_teta < 0) d_teta += deux_pi;
                double dist3  = R*d_teta;
                if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                if (ori3 == 0) // flux de tau21 a travers le bord
                  {
                    // Terme supplementaire en axi
                    const double tau21 = (-vit_imp)/dist3;
                    const double flux2 = db_diffusivite*tau21*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                    resu[fac3] += signe*flux2;

                    // Termes supplementaires dans le laplacien en axi : Ils sont integres comme des termes sources
                    const double coef_laplacien_axi = 0.5*db_diffusivite*tau21;
                    resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
                    resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
                  }
                else if (ori3 == 2) // flux de tau23 a travers le bord
                  {
                    const double tau23 = (-vit_imp)/dist3;
                    const double flux3 = db_diffusivite*tau23*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                    resu[fac3] += signe*flux3;
                  }
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                double flux4;
                if (ori3 == 0)  // flux de tau31 a travers le bord
                  {
                    double dist3 = xv(fac3,2) - xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                    const double tau31 = (-vit_imp)/dist3;
                    flux4 = db_diffusivite*tau31*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 1)  flux de tau32 a travers le bord
                  {
                    assert(ori3 == 1);
                    double dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI) dist3 *= 1;
                    const double tau32 = (-vit_imp)/dist3;
                    flux4 = db_diffusivite*tau32*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                resu[fac3] += signe*flux4;
              }
            break;
          }
        case TypeAreteBordVDF::SYM_SYM: // pas de flux diffusif calcule
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
