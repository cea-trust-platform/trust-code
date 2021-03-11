/****************************************************************************
* Copyright (c) 2021, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Diff_VDF_Face_Axi.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Diff
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VDF_Face_Axi.h>
#include <math.h>
#include <Champ_base.h>
#include <Champ_Inc.h>
#include <SFichier.h>

Implemente_instanciable(Op_Diff_VDF_Face_Axi,"Op_Diff_VDF_const_Face_Axi",Op_Diff_VDF_Face_base2);

Sortie& Op_Diff_VDF_Face_Axi::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Op_Diff_VDF_Face_Axi::readOn(Entree& s )
{
  return s ;
}

void Op_Diff_VDF_Face_Axi::associer(const Zone_dis& zone_dis,
                                    const Zone_Cl_dis& zone_cl_dis,
                                    const Champ_Inc& ch_transporte)
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

// Description:
// associe le champ de diffusivite
void Op_Diff_VDF_Face_Axi::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = ref_cast(Champ_Uniforme, diffu);
}

const Champ_base& Op_Diff_VDF_Face_Axi::diffusivite() const
{
  return diffusivite_;
}

DoubleTab& Op_Diff_VDF_Face_Axi::ajouter(const DoubleTab& inco,  DoubleTab& resu) const
{

  if (inco.nb_dim() > 1)
    {
      Cerr << "Erreur dans Op_Diff_VDF_Face_Axi::ajouter" << finl;
      Cerr << "On ne sait pas traiter la diffusion d'un Champ_Face a plusieurs inconnues" << finl;
      exit();
    }

  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  double db_diffusivite = diffusivite_.valeur()(0,0);
  double d_teta;
  double tau11, tau22, tau12, tau21, R;
  double tau13, tau31, tau23, tau32, tau33;
  int fac1,fac2,fac3,fac4;
  double coef_laplacien_axi;
  //  int nb_faces_elem = zvdf.zone().nb_faces_elem();

  double deux_pi = M_PI*2.0;

  int ndeb ;
  int nfin ;

  double flux;

  // Boucle sur les elements

  int fx0,fx1,fy0,fy1;
  double flux_X,flux_Y;
  int num_elem;
  for (num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
    {
      fx0 = elem_faces(num_elem,0);
      fx1 = elem_faces(num_elem,dimension);
      fy0 = elem_faces(num_elem,1);
      fy1 = elem_faces(num_elem,1+dimension);

      // Calcul de tau11
      tau11 = (inco[fx1]-inco[fx0])/(xv(fx1,0) - xv(fx0,0));

      // Calcul de tau22
      R = xp(num_elem,0);
      d_teta = xv(fy1,1) - xv(fy0,1);
      if (d_teta < 0)
        d_teta += deux_pi;
      tau22 =  (inco[fy1]-inco[fy0])/(R*d_teta);
      // termes supplementaires en axi
      tau22 += 0.5*(inco[fx0]+inco[fx1])/R;


      flux_X = tau11*db_diffusivite*0.5*(surface(fx0)+surface(fx1));
      flux_Y = tau22*db_diffusivite*0.5*(surface(fy0)+surface(fy1));

      resu[fx0] += flux_X;
      resu[fx1] -= flux_X;
      resu[fy0] += flux_Y;
      resu[fy1] -= flux_Y;

      // Termes supplementaires dans le laplacien en axi
      // Ils sont integres comme des termes sources
      // coef_laplacien_axi = -0.5*tau22*db_diffusivite;
      //       resu[fx0] += coef_laplacien_axi*volumes_entrelaces(fx0)*porosite(fx0)/xv(fx0,0);
      //       resu[fx1] += coef_laplacien_axi*volumes_entrelaces(fx1)*porosite(fx1)/xv(fx1,0);
      coef_laplacien_axi = +0.5*tau22*db_diffusivite;
      resu[fx0] -= coef_laplacien_axi*volumes_entrelaces(fx0)*porosite(fx0)/xv(fx0,0);
      resu[fx1] -= coef_laplacien_axi*volumes_entrelaces(fx1)*porosite(fx1)/xv(fx1,0);

    }

  if (dimension == 3)
    {
      int fz0,fz1;
      double flux_Z;

      for (num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
        {
          fz0 = elem_faces(num_elem,2);
          fz1 = elem_faces(num_elem,2+dimension);

          // Calcul de tau33
          tau33 = (inco[fz1]-inco[fz0])/(xv(fz1,2) - xv(fz0,2));

          flux_Z = tau33*db_diffusivite*0.5*(surface(fz0)+surface(fz1));

          resu[fz0] += flux_Z;
          resu[fz1] -= flux_Z;
        }
    }

  // Boucle sur les aretes bord

  ajouter_bord(inco, resu);

  // Boucle sur les aretes mixtes et internes

  ndeb = zvdf.premiere_arete_mixte();
  nfin = zvdf.nb_aretes();
  int ori1, ori3, n_arete;

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {

      fac1=Qdm(n_arete,0);
      fac2=Qdm(n_arete,1);
      fac3=Qdm(n_arete,2);
      fac4=Qdm(n_arete,3);
      ori1 = orientation(fac1);
      ori3 = orientation(fac3);

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          // Calcul de tau21
          R = xv(fac3,0);
          d_teta = xv(fac4,1) - xv(fac3,1);
          if (d_teta < 0)
            d_teta += deux_pi;
          tau21 = (inco(fac4)-inco(fac3))/(R*d_teta);
          // Terme supplementaire en axi
          tau21 -= 0.5*(inco[fac1]+inco[fac2])/R;

          // flux de tau21 sur la facette a cheval sur les faces fac1 et fac2

          flux = db_diffusivite*tau21*0.25*(surface(fac1)+surface(fac2))
                 *(porosite(fac1)+porosite(fac2));


          resu(fac3) += flux;
          resu(fac4) -= flux;

          // Termes supplementaires dans le laplacien en axi
          // Ils sont integres comme des termes sources

          coef_laplacien_axi = 0.5*db_diffusivite*tau21;

          resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
          resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

          // Calcul de tau12
          tau12 = (inco(fac2)-inco(fac1))/(xv(fac2,0) - xv(fac1,0));

          // flux de tau12 sur la facette a cheval sur les faces fac3 et fac4

          flux = db_diffusivite*tau12*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));

          resu(fac1) += flux;
          resu(fac2) -= flux;
        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {

          // Calcul de tau32
          tau32 = (inco(fac4)-inco(fac3))/(xv(fac4,2) - xv(fac3,2));

          // flux de tau32 sur la facette a cheval sur les faces fac1 et fac2
          flux = db_diffusivite*tau32*0.25*(surface(fac1)+surface(fac2))
                 *(porosite(fac1)+porosite(fac2));

          resu(fac3) += flux;
          resu(fac4) -= flux;

          // Calcul de tau23
          R = xv(fac1,0);
          d_teta = xv(fac2,1) - xv(fac1,1);
          if (d_teta < 0)
            d_teta += deux_pi;
          tau23 = (inco(fac2)-inco(fac1))/(R*d_teta);

          // flux de tau23 sur la facette a cheval sur les faces fac3 et fac4
          flux = db_diffusivite*tau23*0.25*(surface(fac3)+surface(fac4))
                 *(porosite(fac3)+porosite(fac4));
          resu(fac1) += flux;
          resu(fac2) -= flux;
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {

          // Calcul de tau31
          tau31 = (inco(fac4)-inco(fac3))/(xv(fac4,2) - xv(fac3,2));

          // flux de tau31 sur la facette a cheval sur les faces fac1 et fac2
          flux = db_diffusivite*tau31*0.25*(surface(fac1)+surface(fac2))
                 *(porosite(fac1)+porosite(fac2));

          resu(fac3) += flux;
          resu(fac4) -= flux;

          // Calcul de tau13
          tau13 = (inco(fac2)-inco(fac1))/(xv(fac2,0) - xv(fac1,0));

          // flux de tau13 sur la facette a cheval sur les faces fac3 et fac4
          flux = db_diffusivite*tau13*0.25*(surface(fac3)+surface(fac4))
                 *(porosite(fac3)+porosite(fac4));

          resu(fac1) += flux;
          resu(fac2) -= flux;
        }

    }
  return resu;
}



double Op_Diff_VDF_Face_Axi::calculer_dt_stab() const
{
  return Op_Diff_VDF_Face_base2::calculer_dt_stab(la_zone_vdf.valeur()) ;
}



DoubleTab& Op_Diff_VDF_Face_Axi::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}


void Op_Diff_VDF_Face_Axi::ajouter_bord(const DoubleTab& inco, DoubleTab& resu) const
{



  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  double db_diffusivite = diffusivite_.valeur()(0,0);
  //  int nb_faces_elem = zvdf.zone().nb_faces_elem();
  double deux_pi = M_PI*2.0;

  double d_teta, R, tmp, coef_laplacien_axi, dist3;
  int fac1,fac2,fac3,signe, n_type, n_arete;

  int ndeb = zvdf.premiere_arete_bord();
  int nfin = ndeb + zvdf.nb_aretes_bord();
  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      n_type=type_arete_bord(n_arete-ndeb);

      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI :
          // paroi-paroi
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          // fluide-fluide
        case TypeAreteBordVDF::PAROI_FLUIDE:
          // paroi-fluide
          {
            fac1 = Qdm(n_arete,0);
            fac2 = Qdm(n_arete,1);
            fac3 = Qdm(n_arete,2);
            signe  = Qdm(n_arete,3);
            int ori1 = orientation(fac1);
            int ori3 = orientation(fac3);
            int rang1 = fac1 - zvdf.premiere_face_bord();
            int rang2 = fac2 - zvdf.premiere_face_bord();
            double vit_imp;

            if (n_type == TypeAreteBordVDF::PAROI_FLUIDE)
              // arete paroi_fluide :il faut determiner qui est la face fluide
              {
                if (est_egal(inco[fac1],0))
                  vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori3,la_zcl_vdf);
                else
                  vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori3,la_zcl_vdf);
              }
            else
              vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori3,la_zcl_vdf)+
                             Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori3,la_zcl_vdf));

            if (ori1 == 0) // bord d'equation R = cte
              {
                double flux1;
                if (ori3 == 1)  // flux de tau12 a travers le bord
                  {
                    dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1; //SB dist3 *= 2;
                    tmp = (inco[fac3]-vit_imp)/dist3;
                    flux1 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 2)  flux de tau13 a travers le bord
                  {
                    assert(ori3==2);
                    dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1; //SB dist3 *= 2
                    tmp = (inco[fac3]-vit_imp)/dist3;
                    flux1 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }

                resu[fac3]+=signe*flux1;

                // Calcul de flux1_2 a coder
                // resu[fac1] += flux1_2;
                // resu[fac2] -= flux1_2;

              }
            else if (ori1 == 1) // bord d'equation teta = cte
              {
                double flux2;
                R = xv(fac3,0);
                d_teta = xv(fac3,1) - xv(fac1,1);
                if (d_teta < 0)
                  d_teta += deux_pi;
                dist3  = R*d_teta;
                if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                  dist3 *= 1; //SB dist3 *= 2

                if (ori3 == 0) // flux de tau21 a travers le bord
                  {

                    // Terme supplementaire en axi
                    double tmpbis=(inco[fac3]-vit_imp)/dist3;

                    tmpbis -=(0.5*(inco[fac1]+inco[fac2])/R);
                    flux2 = db_diffusivite*tmpbis*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    resu[fac3]+=signe*flux2;

                    // Calcul de flux1_2 a coder
                    // resu[fac1]+= flux1_2;
                    // resu[fac2]-= flux1_2;

                    // Termes supplementaires dans le laplacien en axi
                    // Ils sont integres comme des termes sources

                    coef_laplacien_axi = 0.5*db_diffusivite*tmpbis;
                    resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
                    resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

                  }
                else if (ori3 == 2) // flux de tau23 a travers le bord
                  {
                    double flux3;
                    tmp = signe*(vit_imp-inco[fac3])/dist3;
                    flux3 = db_diffusivite*tmp*0.25;
                    flux3*=(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    resu[fac3]+=signe*flux3;

                    // Calcul de flux1_2 a coder
                    // resu[fac1]+= flux1_2;
                    // resu[fac2]-= flux1_2;
                  }
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                double flux4;
                if (ori3 == 0)  // flux de tau31 a travers le bord
                  {
                    dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1; //SB         dist3 *= 2;
                    tmp = (inco[fac3]-vit_imp)/dist3;
                    flux4 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 1)  flux de tau32 a travers le bord
                  {
                    assert(ori3==1);
                    dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1; //SB 2
                    tmp = (inco[fac3]-vit_imp)/dist3;
                    flux4 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }

                resu[fac3] += signe*flux4;
              }
            break;
          }
        case 3:
          {
            // symetrie-symetrie
            // pas de flux diffusif calcule
            break;
          }
        default :
          {
            Cerr << "On a rencontre un type d'arete non prevu\n";
            Cerr << "num arete : " << n_arete;
            Cerr << " type : " << n_type;
            exit();
            break;
          }
        }
    }

}


/////////////////////////////////////////
// Methode pour l'implicite
/////////////////////////////////////////

void Op_Diff_VDF_Face_Axi::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  if (inco.nb_dim() > 1)
    {
      Cerr << "Erreur dans Op_Diff_VDF_Face_Axi::ajouter" << finl;
      Cerr << "On ne sait pas traiter la diffusion d'un Champ_Face a plusieurs inconnues" << finl;
      exit();
    }

  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  double db_diffusivite = diffusivite_.valeur()(0,0);
  double d_teta;
  double tau11, tau22, tau12, tau21, R;
  double tau13, tau31, tau23, tau32, tau33;
  int fac1,fac2,fac3,fac4;
  double coef_laplacien_axi;
  //  int nb_faces_elem = zvdf.zone().nb_faces_elem();

  double deux_pi = M_PI*2.0;

  int ndeb ;
  int nfin,k ;
  double flux;
  IntVect& tab1 = matrice.get_set_tab1();
  IntVect& tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();


  // Boucle sur les elements

  int fx0,fx1,fy0,fy1;
  double flux_X,flux_Y;
  int num_elem;
  for (num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
    {
      fx0 = elem_faces(num_elem,0);
      fx1 = elem_faces(num_elem,dimension);
      fy0 = elem_faces(num_elem,1);
      fy1 = elem_faces(num_elem,1+dimension);

      // Calcul de tau11
      tau11 = 1/(xv(fx1,0) - xv(fx0,0));

      // Calcul de tau22
      R = xp(num_elem,0);
      d_teta = xv(fy1,1) - xv(fy0,1);
      if (d_teta < 0)
        d_teta += deux_pi;
      tau22 =  1/(R*d_teta);
      // termes supplementaires en axi
      tau22 += 0.5/R;

      flux_X = tau11*db_diffusivite*0.5*(surface(fx0)+surface(fx1));
      flux_Y = tau22*db_diffusivite*0.5*(surface(fy0)+surface(fy1));

      for (k=tab1[fx0]-1; k<tab1[fx0+1]-1; k++)
        {
          if (tab2[k]-1==fx0)
            coeff[k]+=flux_X;
          if (tab2[k]-1==fx1)
            coeff[k]-=flux_X;
        }
      for (k=tab1[fx1]-1; k<tab1[fx1+1]-1; k++)
        {
          if (tab2[k]-1==fx0)
            coeff[k]-=flux_X;
          if (tab2[k]-1==fx1)
            coeff[k]+=flux_X;
        }
      for (k=tab1[fy0]-1; k<tab1[fy0+1]-1; k++)
        {
          if (tab2[k]-1==fy0)
            coeff[k]+=flux_Y;
          if (tab2[k]-1==fy1)
            coeff[k]-=flux_Y;
        }
      for (k=tab1[fy1]-1; k<tab1[fy1+1]-1; k++)
        {
          if (tab2[k]-1==fy0)
            coeff[k]-=flux_Y;
          if (tab2[k]-1==fy1)
            coeff[k]+=flux_Y;
        }

      // Termes supplementaires dans le laplacien en axi
      // Ils sont integres comme des termes sources
      // SB - en +
      coef_laplacien_axi = +0.5*tau22*db_diffusivite;
      for (k=tab1[fx0]-1; k<tab1[fx0+1]-1; k++)
        {
          if (tab2[k]-1==fx0)
            coeff[k]+=coef_laplacien_axi*volumes_entrelaces(fx0)*porosite(fx0)/xv(fx0,0);
        }
      for (k=tab1[fx1]-1; k<tab1[fx1+1]-1; k++)
        {
          if (tab2[k]-1==fx1)
            coeff[k]+=
              coef_laplacien_axi*volumes_entrelaces(fx1)*porosite(fx1)/xv(fx1,0);
        }
    }

  if (dimension == 3)
    {
      int fz0,fz1;
      double flux_Z;

      for (num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
        {
          fz0 = elem_faces(num_elem,2);
          fz1 = elem_faces(num_elem,2+dimension);

          // Calcul de tau33
          tau33 =1/(xv(fz1,2) - xv(fz0,2));
          flux_Z = tau33*db_diffusivite*0.5*(surface(fz0)+surface(fz1));

          for (k=tab1[fz0]-1; k<tab1[fz0+1]-1; k++)
            {
              if (tab2[k]-1==fz0)
                coeff[k]+=flux_Z;
              if (tab2[k]-1==fz1)
                coeff[k]-=flux_Z;
            }
          for (k=tab1[fz1]-1; k<tab1[fz1+1]-1; k++)
            {
              if (tab2[k]-1==fz0)
                coeff[k]-=flux_Z;
              if (tab2[k]-1==fz1)
                coeff[k]+=flux_Z;
            }
        }
    }

  // Boucle sur les aretes bord
  ajouter_contribution_bord(inco, matrice);

  // Boucle sur les aretes mixtes et internes

  ndeb = zvdf.premiere_arete_mixte();
  nfin = zvdf.nb_aretes();
  int ori1, ori3, n_arete;
  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {

      fac1=Qdm(n_arete,0);
      fac2=Qdm(n_arete,1);
      fac3=Qdm(n_arete,2);
      fac4=Qdm(n_arete,3);
      ori1 = orientation(fac1);
      ori3 = orientation(fac3);

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          // Calcul de tau21
          R = xv(fac3,0);
          d_teta = xv(fac4,1) - xv(fac3,1);
          if (d_teta < 0)
            d_teta += deux_pi;
          tau21 = 1/(R*d_teta);

          // Terme supplementaire en axi
          tau21 -= 0.5/R;

          // flux de tau21 sur la facette a cheval sur les faces fac1 et fac2

          flux = db_diffusivite*tau21*0.25*(surface(fac1)+surface(fac2))
                 *(porosite(fac1)+porosite(fac2));

          for (k=tab1[fac3]-1; k<tab1[fac3+1]-1; k++)
            {
              if (tab2[k]-1==fac3)
                coeff[k]+=flux;
              if (tab2[k]-1==fac4)
                coeff[k]-=flux;
            }
          for (k=tab1[fac4]-1; k<tab1[fac4+1]-1; k++)
            {
              if (tab2[k]-1==fac3)
                coeff[k]-=flux;
              if (tab2[k]-1==fac4)
                coeff[k]+=flux;
            }

          // Termes supplementaires dans le laplacien en axi
          // Ils sont integres comme des termes sources

          coef_laplacien_axi = 0.5*db_diffusivite*tau21;
          for (k=tab1[fac1]-1; k<tab1[fac1+1]-1; k++)
            {
              if (tab2[k]-1==fac1)
                coeff[k]+=coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
            }
          for (k=tab1[fac2]-1; k<tab1[fac2+1]-1; k++)
            {
              if (tab2[k]-1==fac2)
                coeff[k]+=coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
            }

          // Calcul de tau12
          tau12 = 1/(xv(fac2,0) - xv(fac1,0));

          // flux de tau12 sur la facette a cheval sur les faces fac3 et fac4

          flux = db_diffusivite*tau12*0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          for (k=tab1[fac1]-1; k<tab1[fac1+1]-1; k++)
            {
              if (tab2[k]-1==fac1)
                coeff[k]+=flux;
              if (tab2[k]-1==fac2)
                coeff[k]-=flux;
            }
          for (k=tab1[fac2]-1; k<tab1[fac2+1]-1; k++)
            {
              if (tab2[k]-1==fac1)
                coeff[k]-=flux;
              if (tab2[k]-1==fac2)
                coeff[k]+=flux;
            }
        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {

          // Calcul de tau32
          tau32 = 1/(xv(fac4,2) - xv(fac3,2));

          // flux de tau32 sur la facette a cheval sur les faces fac1 et fac2
          flux = db_diffusivite*tau32*0.25*(surface(fac1)+surface(fac2))
                 *(porosite(fac1)+porosite(fac2));

          for (k=tab1[fac3]-1; k<tab1[fac3+1]-1; k++)
            {
              if (tab2[k]-1==fac3)
                coeff[k]+=flux;
              if (tab2[k]-1==fac4)
                coeff[k]-=flux;
            }
          for (k=tab1[fac4]-1; k<tab1[fac4+1]-1; k++)
            {
              if (tab2[k]-1==fac3)
                coeff[k]-=flux;
              if (tab2[k]-1==fac4)
                coeff[k]+=flux;
            }

          // Calcul de tau23
          R = xv(fac1,0);
          d_teta = xv(fac2,1) - xv(fac1,1);
          if (d_teta < 0)
            d_teta += deux_pi;
          tau23 = 1/(R*d_teta);

          // flux de tau23 sur la facette a cheval sur les faces fac3 et fac4
          flux = db_diffusivite*tau23*0.25*(surface(fac3)+surface(fac4))
                 *(porosite(fac3)+porosite(fac4));
          for (k=tab1[fac1]-1; k<tab1[fac1+1]-1; k++)
            {
              if (tab2[k]-1==fac1)
                coeff[k]+=flux;
              if (tab2[k]-1==fac2)
                coeff[k]-=flux;
            }
          for (k=tab1[fac2]-1; k<tab1[fac2+1]-1; k++)
            {
              if (tab2[k]-1==fac1)
                coeff[k]-=flux;
              if (tab2[k]-1==fac2)
                coeff[k]+=flux;
            }
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {

          // Calcul de tau31
          tau31 = 1/(xv(fac4,2) - xv(fac3,2));

          // flux de tau31 sur la facette a cheval sur les faces fac1 et fac2
          flux = db_diffusivite*tau31*0.25*(surface(fac1)+surface(fac2))
                 *(porosite(fac1)+porosite(fac2));

          for (k=tab1[fac3]-1; k<tab1[fac3+1]-1; k++)
            {
              if (tab2[k]-1==fac3)
                coeff[k]+=flux;
              if (tab2[k]-1==fac4)
                coeff[k]-=flux;
            }
          for (k=tab1[fac4]-1; k<tab1[fac4+1]-1; k++)
            {
              if (tab2[k]-1==fac3)
                coeff[k]-=flux;
              if (tab2[k]-1==fac4)
                coeff[k]+=flux;
            }
          // Calcul de tau13
          tau13 = 1/(xv(fac2,0) - xv(fac1,0));

          // flux de tau13 sur la facette a cheval sur les faces fac3 et fac4
          flux = db_diffusivite*tau13*0.25*(surface(fac3)+surface(fac4))
                 *(porosite(fac3)+porosite(fac4));
          for (k=tab1[fac1]-1; k<tab1[fac1+1]-1; k++)
            {
              if (tab2[k]-1==fac1)
                coeff[k]+=flux;
              if (tab2[k]-1==fac2)
                coeff[k]-=flux;
            }
          for (k=tab1[fac2]-1; k<tab1[fac2+1]-1; k++)
            {
              if (tab2[k]-1==fac1)
                coeff[k]-=flux;
              if (tab2[k]-1==fac2)
                coeff[k]+=flux;
            }
        }
    }
}

void Op_Diff_VDF_Face_Axi::ajouter_contribution_bord(const DoubleTab& inco, Matrice_Morse& matrice ) const
{

  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  double db_diffusivite = diffusivite_.valeur()(0,0);
  //  int nb_faces_elem = zvdf.zone().nb_faces_elem();
  double deux_pi = M_PI*2.0;

  double d_teta, R, tmp, coef_laplacien_axi, dist3;
  int fac1,fac2,fac3,signe, n_type, n_arete,k;

  int ndeb = zvdf.premiere_arete_bord();
  int nfin = ndeb + zvdf.nb_aretes_bord();

  IntVect& tab1 = matrice.get_set_tab1();
  IntVect& tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      n_type=type_arete_bord(n_arete-ndeb);

      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          // paroi-paroi
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          // fluide-fluide
        case TypeAreteBordVDF::PAROI_FLUIDE:
          // paroi-fluide
          {
            double flux1;
            fac1 = Qdm(n_arete,0);
            fac2 = Qdm(n_arete,1);
            fac3 = Qdm(n_arete,2);
            signe  = Qdm(n_arete,3);
            int ori1 = orientation(fac1);
            int ori3 = orientation(fac3);
            if (ori1 == 0) // bord d'equation R = cte
              {
                if (ori3 == 1)  // flux de tau12 a travers le bord
                  {
                    dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1; //SB 2
                    tmp = 1/dist3;
                    flux1 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 2)  flux de tau13 a travers le bord
                  {
                    assert(ori3 == 2);
                    dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1; // SB
                    tmp =   1/dist3; //SB signe/dist3
                    flux1 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }
                for (k=tab1[fac3]-1; k<tab1[fac3+1]-1; k++)
                  {
                    if (tab2[k]-1==fac3)
                      coeff[k]+=signe*flux1;
                  }

                // Calcul de flux1_2 a coder
                // remplir les coeff des face fac1 et fac2
              }
            else if (ori1 == 1) // bord d'equation teta = cte
              {
                R = xv(fac3,0);
                d_teta = xv(fac3,1) - xv(fac1,1);
                if (d_teta < 0)
                  d_teta += deux_pi;
                dist3  = R*d_teta;
                if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                  dist3 *= 1; // 2
                double flux2;
                if (ori3 == 0) // flux de tau21 a travers le bord
                  {

                    // Terme supplementaire en axi
                    double tmpbis=(1/dist3); // signe/dist3

                    tmpbis -=(0.5/R); // +=
                    flux2 = db_diffusivite*tmpbis*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    for (k=tab1[fac3]-1; k<tab1[fac3+1]-1; k++)
                      {
                        if (tab2[k]-1==fac3)
                          coeff[k]+=signe*flux2;
                      }

                    // Calcul de flux1_2 a coder
                    // resu[fac1]+= flux1_2;
                    // resu[fac2]-= flux1_2;

                    // Termes supplementaires dans le laplacien en axi
                    // Ils sont integres comme des termes sources

                    coef_laplacien_axi = 0.5*db_diffusivite*tmpbis;
                    for (k=tab1[fac1]-1; k<tab1[fac1+1]-1; k++)
                      {
                        if (tab2[k]-1==fac1)
                          coeff[k]+=coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
                      }
                    for (k=tab1[fac2]-1; k<tab1[fac2+1]-1; k++)
                      {
                        if (tab2[k]-1==fac2)
                          coeff[k]+=coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
                      }
                  }
                else //if (ori3 == 2)  flux de tau23 a travers le bord
                  {
                    assert(ori3==2);
                    tmp = 1/dist3; //SB signe/dist3
                    flux2 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    for (k=tab1[fac3]-1; k<tab1[fac3+1]-1; k++)
                      {
                        if (tab2[k]-1==fac3)
                          coeff[k]+=signe*flux2;
                      }

                    // Calcul de flux1_2 a coder
                    // resu[fac1]+= flux1_2;
                    // resu[fac2]-= flux1_2;
                  }
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                double flux3;
                if (ori3 == 0)  // flux de tau31 a travers le bord
                  {
                    dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1;        //SB 2
                    tmp = 1/dist3; //SB signe/dist3
                    flux3 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 1) flux de tau32 a travers le bord
                  {
                    assert(ori3 == 1);
                    dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1;        //SB 2
                    tmp = 1/dist3; //SB signe/dist3
                    flux3 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }
                for (k=tab1[fac3]-1; k<tab1[fac3+1]-1; k++)
                  {
                    if (tab2[k]-1==fac3)
                      coeff[k]+=signe*flux3;
                  }
              }
            break;
          }
        case 3:
          {
            // symetrie-symetrie
            // pas de flux diffusif calcule
            break;
          }
        default :
          {
            Cerr << "On a rencontre un type d'arete non prevu\n";
            Cerr << "num arete : " << n_arete;
            Cerr << " type : " << n_type;
            exit();
            break;
          }
        }
    }

}

void Op_Diff_VDF_Face_Axi::contribue_au_second_membre(DoubleTab& resu ) const
{

  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  double db_diffusivite = diffusivite_.valeur()(0,0);
  //  int nb_faces_elem = zvdf.zone().nb_faces_elem();
  double deux_pi = M_PI*2.0;

  double d_teta, R, tmp, coef_laplacien_axi, dist3;
  int fac1,fac2,fac3,signe, n_type, n_arete;

  int ndeb = zvdf.premiere_arete_bord();
  int nfin = ndeb + zvdf.nb_aretes_bord();
  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      n_type=type_arete_bord(n_arete-ndeb);

      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          // paroi-paroi
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          // fluide-fluide
        case TypeAreteBordVDF::PAROI_FLUIDE:
          // paroi-fluide
          {
            fac1 = Qdm(n_arete,0);
            fac2 = Qdm(n_arete,1);
            fac3 = Qdm(n_arete,2);
            signe  = Qdm(n_arete,3);
            int ori1 = orientation(fac1);
            int ori3 = orientation(fac3);
            int rang1 = fac1 - zvdf.premiere_face_bord();
            int rang2 = fac2 - zvdf.premiere_face_bord();
            double vit_imp;

            if (n_type == TypeAreteBordVDF::PAROI_FLUIDE)
              // arete paroi_fluide :il faut determiner qui est la face fluide
              {
                if (est_egal(inconnue.valeur()(fac1),0))
                  vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori3,la_zcl_vdf);
                else
                  vit_imp = Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori3,la_zcl_vdf);
              }
            else
              vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori3,la_zcl_vdf)+
                             Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori3,la_zcl_vdf));

            if (ori1 == 0) // bord d'equation R = cte
              {
                double flux1;
                if (ori3 == 1)  // flux de tau12 a travers le bord
                  {
                    dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1; //SB 2
                    tmp = (-1*vit_imp)/dist3;
                    flux1 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 2)  flux de tau13 a travers le bord
                  {
                    assert(ori3 == 2);
                    dist3 = xv(fac3,0)-xv(fac1,0);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1;
                    tmp =   -1*(vit_imp)/dist3;
                    flux1 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }

                resu[fac3]+=signe*flux1;

                // Calcul de flux1_2 a coder
                // resu[fac1] += flux1_2;
                // resu[fac2] -= flux1_2;

              }
            else if (ori1 == 1) // bord d'equation teta = cte
              {
                R = xv(fac3,0);
                d_teta = xv(fac3,1) - xv(fac1,1);
                if (d_teta < 0)
                  d_teta += deux_pi;
                dist3  = R*d_teta;
                if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                  dist3 *= 1;

                if (ori3 == 0) // flux de tau21 a travers le bord
                  {
                    double flux2;
                    // Terme supplementaire en axi
                    double tmpbis=(-1*(vit_imp)/dist3);

                    flux2 = db_diffusivite*tmpbis *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    resu[fac3]+=signe*flux2;

                    // Calcul de flux1_2 a coder
                    // resu[fac1]+= flux1_2;
                    // resu[fac2]-= flux1_2;

                    // Termes supplementaires dans le laplacien en axi
                    // Ils sont integres comme des termes sources

                    coef_laplacien_axi = 0.5*db_diffusivite*tmpbis;
                    resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
                    resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

                  }
                else if (ori3 == 2) // flux de tau23 a travers le bord
                  {
                    double flux3;
                    tmp = -1*(vit_imp)/dist3;
                    flux3 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    resu[fac3]+=signe*flux3;

                    // Calcul de flux1_2 a coder
                    // resu[fac1]+= flux1_2;
                    // resu[fac2]-= flux1_2;
                  }
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                double flux4;
                if (ori3 == 0)  // flux de tau31 a travers le bord
                  {
                    dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1;
                    tmp = -1*(vit_imp)/dist3;
                    flux4 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }
                else // if (ori3 == 1)  flux de tau32 a travers le bord
                  {
                    assert(ori3 == 1);
                    dist3 = xv(fac3,2)-xv(fac1,2);
                    if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                      dist3 *= 1;
                    tmp = -1*(vit_imp)/dist3;
                    flux4 = db_diffusivite*tmp*0.25*(surface(fac1)+surface(fac2))
                            *(porosite(fac1)+porosite(fac2));
                  }

                resu[fac3] += signe*flux4;
              }
            break;
          }
        case 3:
          {
            // symetrie-symetrie
            // pas de flux diffusif calcule
            break;
          }
        default :
          {
            Cerr << "On a rencontre un type d'arete non prevu\n";
            Cerr << "num arete : " << n_arete;
            Cerr << " type : " << n_type;
            exit();
            break;
          }
        }
    }

}

