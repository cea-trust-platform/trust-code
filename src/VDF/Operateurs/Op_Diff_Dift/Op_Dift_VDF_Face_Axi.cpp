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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Dift_VDF_Face_Axi.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Diff_Dift
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VDF_Face_Axi.h>
#include <Mod_turb_hyd_base.h>
#include <Schema_Temps.h>
#include <Champ_base.h>
#include <SFichier.h>

Implemente_instanciable(Op_Dift_VDF_Face_Axi,"Op_Dift_VDF_Face_Axi",Op_Dift_VDF_Face_base);

Sortie& Op_Dift_VDF_Face_Axi::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Op_Dift_VDF_Face_Axi::readOn(Entree& s ) { return s ; }

void Op_Dift_VDF_Face_Axi::associer(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_cl_dis, const Champ_Inc& ch_transporte)
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

void Op_Dift_VDF_Face_Axi::completer()
{
  Op_Dift_VDF_base::completer();
  Equation_base& eqn_hydr = equation();
  Champ_Face& vitesse = ref_cast(Champ_Face,eqn_hydr.inconnue().valeur());
  vitesse.dimensionner_tenseur_Grad();
  const RefObjU& modele_turbulence = eqn_hydr.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
  associer_modele_turbulence(mod_turb);
}

// Description:
// associe le champ de diffusivite
void Op_Dift_VDF_Face_Axi::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = ref_cast(Champ_Uniforme, diffu);
}

const Champ_base& Op_Dift_VDF_Face_Axi::diffusivite() const
{
  return diffusivite_;
}

void Op_Dift_VDF_Face_Axi::associer_diffusivite_turbulente(const Champ_Fonc& visc_turb)
{
  Op_Diff_Turbulent_base::associer_diffusivite_turbulente(visc_turb);
}

void Op_Dift_VDF_Face_Axi::associer_loipar(const Turbulence_paroi& loi_paroi)
{
  //loipar = loi_paroi;
}

void Op_Dift_VDF_Face_Axi::associer_modele_turbulence(const Mod_turb_hyd_base& mod)
{
  le_modele_turbulence = mod;
  associer_diffusivite_turbulente(le_modele_turbulence->viscosite_turbulente());
  associer_loipar(le_modele_turbulence->loi_paroi());
}

void Op_Dift_VDF_Face_Axi::mettre_a_jour(double )
{
  tau_tan.ref(le_modele_turbulence->loi_paroi().valeur().Cisaillement_paroi());
}

DoubleTab& Op_Dift_VDF_Face_Axi::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  if (inco.line_size() > 1)
    {
      Cerr << "Erreur dans Op_Dift_VDF_Face_Axi::ajouter" << finl;
      Cerr << "On ne sait pas traiter la diffusion d'un Champ_Face a plusieurs inconnues" << finl;
      Process::exit();
    }

  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  const Champ_Uniforme& viscosite_lam = diffusivite_;
  double visco_lam = viscosite_lam(0,0);
  const DoubleVect& visco_turb = diffusivite_turbulente()->valeurs();
  double d_visco_turb;

  const DoubleTab& tau_diag = inconnue->tau_diag();
  const DoubleTab& tau_croises = inconnue->tau_croises();
  ref_cast_non_const(Champ_Face,inconnue.valeur()).calculer_dercov_axi(zclvdf);


  // Boucle sur les elements pour traiter les facettes situees
  // a l'interieur des elements
  int fx0,fx1,fy0,fy1;
  double flux_X,flux_Y, coef_laplacien_axi;

  for (int num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
    {
      fx0 = elem_faces(num_elem,0);
      fx1 = elem_faces(num_elem,dimension);
      fy0 = elem_faces(num_elem,1);
      fy1 = elem_faces(num_elem,1+dimension);

      flux_X = ((visco_lam + 2*visco_turb(num_elem))*tau_diag(num_elem,0))*0.5*(surface(fx0)+surface(fx1));

      flux_Y = ((visco_lam + 2*visco_turb(num_elem))*tau_diag(num_elem,1))*0.5*(surface(fy0)+surface(fy1));

      resu(fx0) += flux_X;
      resu(fx1) += flux_X;

      resu(fy0) += flux_Y;
      resu(fy1) += flux_Y;

      // Termes supplementaires dans le laplacien en axi
      // Ils sont integres comme des termes sources

      coef_laplacien_axi = -0.5*(tau_diag(num_elem,1)*(visco_lam+2*visco_turb(num_elem)));

      resu[fx0] += coef_laplacien_axi*volumes_entrelaces(fx0)*porosite(fx0)/xv(fx0,0);
      resu[fx1] += coef_laplacien_axi*volumes_entrelaces(fx1)*porosite(fx1)/xv(fx1,0);

    }

  if (dimension == 3)
    {
      int fz0,fz1;
      double flux_Z;

      for (int num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
        {
          fz0 = elem_faces(num_elem,2);
          fz1 = elem_faces(num_elem,2+dimension);
          flux_Z = ((visco_lam + 2*visco_turb(num_elem))*tau_diag(num_elem,2)) *0.5*(surface(fz0)+surface(fz1));

          resu[fz0] += flux_Z;
          resu[fz1] -= flux_Z;
        }
    }



  // Boucle sur les bord pour traiter les faces de type sortie_libre

  int ndeb, nfin;

  // Boucle sur les aretes bord

  int n_arete;
  ndeb = zvdf.premiere_arete_bord();
  nfin = ndeb + zvdf.nb_aretes_bord();

  // double flux;
  int ori1,ori3,n_type,fac1,fac2,fac3,signe;
  double tau12,tau21,tau13,tau31,tau23,tau32;

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      n_type=type_arete_bord(n_arete-ndeb);

      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI: // paroi-paroi
          {
            double flux0;
            fac1 = Qdm(n_arete,0);
            fac2 = Qdm(n_arete,1);
            fac3 = Qdm(n_arete,2);
            signe  = Qdm(n_arete,3);
            ori1 = orientation(fac1);
            ori3 = orientation(fac3);
            int rang1 = (fac1-zvdf.premiere_face_bord());
            int rang2 = (fac2-zvdf.premiere_face_bord());
            double vit = inco(fac3);
            double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori3,la_zcl_vdf)+
                                  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori3,la_zcl_vdf));

            double tau1 = tau_tan(rang1,ori3)*0.5*surface(fac1);
            double tau2 = tau_tan(rang2,ori3)*0.5*surface(fac2);
            double coef = tau1+tau2;

            int signe_terme;

            if ( vit < vit_imp )
              signe_terme = -1;
            else
              signe_terme = 1;

            //YB : 30/01/2004 :
            //Elimination de la determination du signe du flux parietal par l'evaluateur de l'operateur de diffusion,
            //en contre-partie le cisaillement parietal fournit par la loi paroi est signe (voir modif dans distances_VDF.cpp)
            signe_terme = 1;
            //Fin modif YB

            flux0 = signe_terme*coef;

            resu[fac3]+= flux0;
            break;
          }

        case TypeAreteBordVDF::FLUIDE_FLUIDE: // fluide-fluide
        case TypeAreteBordVDF::PAROI_FLUIDE: // paroi-fluide
          {
            double flux1;
            fac1 = Qdm(n_arete,0);
            fac2 = Qdm(n_arete,1);
            fac3 = Qdm(n_arete,2);
            signe  = Qdm(n_arete,3);
            int ori1b = orientation(fac1);
            int ori3b = orientation(fac3);

            d_visco_turb = 0.5*(visco_turb(face_voisins(fac3,0))
                                + visco_turb(face_voisins(fac3,1)));

            if (ori1b == 0) // bord d'equation R = cte
              {
                if (ori3b == 1)
                  {
                    tau12 = tau_croises(n_arete,0);
                    tau21 = tau_croises(n_arete,1);

                    flux1 = (visco_lam*tau12 + d_visco_turb*(tau12+tau21))
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 2)
                  {
                    assert(ori3b == 2) ;
                    tau13 = tau_croises(n_arete,0);
                    tau31 = tau_croises(n_arete,1);

                    flux1 = (visco_lam*tau13 + d_visco_turb*(tau13+tau31))
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }

                resu[fac3]+=signe*flux1;

                // Calcul de flux1_2 a coder
                // resu[fac1] += flux1_2;
                // resu[fac2] -= flux1_2;

              }
            else if (ori1b == 1) // bord d'equation teta = cte
              {
                if (ori3b == 0)
                  {
                    double flux2;
                    tau21 = tau_croises(n_arete,0);
                    tau12 = tau_croises(n_arete,1);

                    flux2 = (visco_lam*tau21 + d_visco_turb*(tau12+tau21))
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    resu[fac3]+=signe*flux2;

                    // Calcul de flux1_2 a coder
                    // resu[fac1]+= flux1_2;
                    // resu[fac2]-= flux1_2;

                    // Termes supplementaires dans le laplacien en axi
                    // Ils sont integres comme des termes sources

                    coef_laplacien_axi = 0.5*(visco_lam*tau21 + d_visco_turb*(tau12+tau21));
                    resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
                    resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
                  }
                else if (ori3b == 2) // flux de tau23 a travers le bord
                  {
                    double flux3;
                    tau23 = tau_croises(n_arete,0);
                    tau32 = tau_croises(n_arete,1);

                    flux3 = (visco_lam*tau23 + d_visco_turb*(tau23+tau32))
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    resu[fac3]+=signe*flux3;

                    // Calcul de flux1_2 a coder
                    // resu[fac1]+= flux1_2;
                    // resu[fac2]-= flux1_2;
                  }

              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                double flux4;
                if (ori3b == 0)
                  {

                    tau31 = tau_croises(n_arete,0);
                    tau13 = tau_croises(n_arete,1);

                    flux4 = (visco_lam*tau31 + d_visco_turb*(tau13+tau31))
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 1)  flux de tau32 a travers le bord
                  {
                    assert(ori3b == 1);
                    tau32 = tau_croises(n_arete,0);
                    tau23 = tau_croises(n_arete,1);

                    flux4 = (visco_lam*tau32 + d_visco_turb*(tau23+tau32))
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                // ATTENTION GROSSE CORRECTION LA PARANTHESE SUIVANTE ETAIT
                // MAL PLACEE
                // }


                resu[fac3] += signe*flux4;
              }
            break;
          }
        case TypeAreteBordVDF::SYM_SYM:
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

  int fac4;

  // Boucle sur les aretes mixtes
  // Sur les aretes mixtes les termes croises du tenseur de Reynolds
  // sont nuls: il ne reste donc que la diffusion laminaire

  ndeb = zvdf.premiere_arete_mixte();
  nfin = zvdf.premiere_arete_interne();

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
          double flux1;
          tau12 = tau_croises(n_arete,0);
          tau21 = tau_croises(n_arete,1);

          // flux de mu_lam*tau21
          // sur la facette a cheval sur les faces fac1 et fac2

          flux1 = visco_lam*tau21*0.25*(surface(fac1)+surface(fac2))
                  *(porosite(fac1)+porosite(fac2));

          resu(fac3) += flux1;
          resu(fac4) -= flux1;

          // Termes supplementaires dans le laplacien en axi
          // Ils sont integres comme des termes sources

          coef_laplacien_axi = 0.5*visco_lam*tau21;

          resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
          resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

          // flux de mu_lam*tau12
          // sur la facette a cheval sur les faces fac3 et fac4

          flux1 = visco_lam*tau12*0.25*(surface(fac3)+surface(fac4))
                  *(porosite(fac3)+porosite(fac4));

          resu(fac1) += flux1;
          resu(fac2) -= flux1;

        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {
          double flux2;
          tau23 = tau_croises(n_arete,0);
          tau32 = tau_croises(n_arete,1);

          // flux de mu_lam*tau32
          // sur la facette a cheval sur les faces fac1 et fac2

          flux2 = visco_lam*tau32*0.25*(surface(fac1)+surface(fac2))
                  *(porosite(fac1)+porosite(fac2));

          resu(fac3) += flux2;
          resu(fac4) -= flux2;

          // flux de mu_lam*tau23
          // sur la facette a cheval sur les faces fac3 et fac4

          flux2 = visco_lam*tau23*0.25*(surface(fac3)+surface(fac4))
                  *(porosite(fac3)+porosite(fac4));

          resu(fac1) += flux2;
          resu(fac2) -= flux2;
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {
          double flux3;
          tau13 = tau_croises(n_arete,0);
          tau31 = tau_croises(n_arete,1);

          // flux de mu_lam*tau31
          // sur la facette a cheval sur les faces fac1 et fac2

          flux3 = visco_lam*tau31*0.25*(surface(fac1)+surface(fac2))
                  *(porosite(fac1)+porosite(fac2));

          resu(fac3) += flux3;
          resu(fac4) -= flux3;

          // flux de mu_lam*tau13
          // sur la facette a cheval sur les faces fac3 et fac4

          flux3 = visco_lam*tau13*0.25*(surface(fac3)+surface(fac4))
                  *(porosite(fac3)+porosite(fac4));

          resu(fac1) += flux3;
          resu(fac2) -= flux3;
        }
    }

  // Boucle sur les aretes internes

  ndeb = zvdf.premiere_arete_interne();
  nfin = zvdf.nb_aretes();

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      fac1=Qdm(n_arete,0);
      fac2=Qdm(n_arete,1);
      fac3=Qdm(n_arete,2);
      fac4=Qdm(n_arete,3);
      ori1 = orientation(fac1);
      ori3 = orientation(fac3);

      // Calcul de la viscosite turbulente au milieu de l'arete

      d_visco_turb = 0.25*(visco_turb(face_voisins(fac3,0)) + visco_turb(face_voisins(fac3,1))
                           + visco_turb(face_voisins(fac4,0)) + visco_turb(face_voisins(fac4,1)));

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          double flux1;
          tau12 = tau_croises(n_arete,0);
          tau21 = tau_croises(n_arete,1);

          // flux de mu_lam*tau21 + mu_turb*(tau21+tau12)
          // sur la facette a cheval sur les faces fac1 et fac2

          flux1 = (visco_lam*tau21 + d_visco_turb*(tau21+tau12))
                  *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

          resu(fac3) += flux1;
          resu(fac4) -= flux1;

          // Termes supplementaires dans le laplacien en axi
          // Ils sont integres comme des termes sources

          coef_laplacien_axi = 0.5*(visco_lam*tau21 + d_visco_turb*(tau21+tau12));

          resu(fac1) += coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
          resu(fac2) += coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);

          // flux de mu_lam*tau12 + mu_turb*(tau21+tau12)
          // sur la facette a cheval sur les faces fac3 et fac4

          flux1 = (visco_lam*tau12 + d_visco_turb*(tau12+tau21))
                  *0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));

          resu(fac1) += flux1;
          resu(fac2) -= flux1;

        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {
          double flux2;
          tau23 = tau_croises(n_arete,0);
          tau32 = tau_croises(n_arete,1);

          // flux de mu_lam*tau32 + mu_turb*(tau32+tau23)
          // sur la facette a cheval sur les faces fac1 et fac2

          flux2 = (visco_lam*tau32 + d_visco_turb*(tau32+tau23))
                  *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

          resu(fac3) += flux2;
          resu(fac4) -= flux2;

          // flux de mu_lam*tau23 + mu_turb*(tau32+tau23)
          // sur la facette a cheval sur les faces fac3 et fac4

          flux2 = (visco_lam*tau23 + d_visco_turb*(tau32+tau23))
                  *0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));

          resu(fac1) += flux2;
          resu(fac2) -= flux2;
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {
          double flux3;
          tau13 = tau_croises(n_arete,0);
          tau31 = tau_croises(n_arete,1);

          // flux de  mu_lam*tau31 + mu_turb*(tau31+tau13)
          // sur la facette a cheval sur les faces fac1 et fac2

          flux3 = (visco_lam*tau31 + d_visco_turb*(tau31+tau13))
                  *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

          resu(fac3) += flux3;
          resu(fac4) -= flux3;

          // flux de mu_lam*tau13 + mu_turb*(tau13+tau31)
          // sur la facette a cheval sur les faces fac3 et fac4

          flux3 = (visco_lam*tau13 + d_visco_turb*(tau31+tau13))
                  *0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));

          resu(fac1) += flux3;
          resu(fac2) -= flux3;
        }

    }
  return resu;
}

DoubleTab& Op_Dift_VDF_Face_Axi::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}


/////////////////////////////////////////
// Methode pour l'implicite
/////////////////////////////////////////

void Op_Dift_VDF_Face_Axi::ajouter_contribution(const DoubleTab& inco,                                                         Matrice_Morse& matrice ) const
{

  if (inco.nb_dim() > 1)
    {
      Cerr << "Erreur dans Op_Dift_VDF_Face_Axi::ajouter" << finl;
      Cerr << "On ne sait pas traiter la diffusion d'un Champ_Face a plusieurs inconnues" << finl;
      exit();
    }
  int l;
  //double temps=equation().schema_temps().temps_courant();
  //ref_cast_non_const(Op_Dift_VDF_Face_Axi, (*this)).mettre_a_jour(temps);
  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  //  int nb_elem = zvdf.nb_elem();
  //  int nb_aretes = zvdf.nb_aretes();

  IntVect& tab1 = matrice.get_set_tab1();
  IntVect& tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();


  const Champ_Uniforme& viscosite_lam = diffusivite_;
  double visco_lam = viscosite_lam(0,0);
  const DoubleVect& visco_turb = diffusivite_turbulente()->valeurs();
  double d_visco_turb;

  //  DoubleTab& tau_diag = inconnue->tau_diag();
  //  DoubleTab& tau_croises = inconnue->tau_croises();
  ref_cast_non_const(Champ_Face, inconnue.valeur()).calculer_dercov_axi(zclvdf);


  // Boucle sur les elements pour traiter les facettes situees
  // a l'interieur des elements

  int fx0,fx1,fy0,fy1;
  double flux_X,flux_Y;
  double coef_laplacien_axi;

  for (int num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
    {
      fx0 = elem_faces(num_elem,0);
      fx1 = elem_faces(num_elem,dimension);
      fy0 = elem_faces(num_elem,1);
      fy1 = elem_faces(num_elem,1+dimension);
      flux_X = ((visco_lam + 2*visco_turb(num_elem)))*0.5*(surface(fx0)+surface(fx1));

      flux_Y = ((visco_lam + 2*visco_turb(num_elem)))*0.5*(surface(fy0)+surface(fy1));

      for (l=tab1[fx0]-1; l<tab1[fx0+1]-1; l++)
        {
          if (tab2[l]-1==fx0)
            coeff[l]+=flux_X;
          if (tab2[l]-1==fx1)
            coeff[l]-=flux_X;
        }
      for (l=tab1[fx1]-1; l<tab1[fx1+1]-1; l++)
        {
          if (tab2[l]-1==fx0)
            coeff[l]-=flux_X;
          if (tab2[l]-1==fx1)
            coeff[l]+=flux_X;
        }
      for (l=tab1[fy0]-1; l<tab1[fy0+1]-1; l++)
        {
          if (tab2[l]-1==fy0)
            coeff[l]+=flux_Y;
          if (tab2[l]-1==fy1)
            coeff[l]-=flux_Y;
        }
      for (l=tab1[fy1]-1; l<tab1[fy1+1]-1; l++)
        {
          if (tab2[l]-1==fy0)
            coeff[l]-=flux_Y;
          if (tab2[l]-1==fy1)
            coeff[l]+=flux_Y;
        }
      // Termes supplementaires dans le laplacien en axi
      // Ils sont integres comme des termes sources

      coef_laplacien_axi = -0.5*(visco_lam+2*visco_turb(num_elem));
      for (l=tab1[fx0]-1; l<tab1[fx0+1]-1; l++)
        {
          if (tab2[l]-1==fx0)
            coeff[l]+= coef_laplacien_axi*volumes_entrelaces(fx0)*porosite(fx0)/xv(fx0,0);

        }
      for (l=tab1[fx1]-1; l<tab1[fx1+1]-1; l++)
        {
          if (tab2[l]-1==fx1)
            coeff[l]+=coef_laplacien_axi*volumes_entrelaces(fx1)*porosite(fx1)/xv(fx1,0);
        }
    }
  if (dimension == 3)
    {
      int fz0,fz1;
      double flux_Z;

      for (int num_elem=0; num_elem<zvdf.nb_elem(); num_elem++)
        {
          fz0 = elem_faces(num_elem,2);
          fz1 = elem_faces(num_elem,2+dimension);

          flux_Z = ((visco_lam + 2*visco_turb(num_elem))) *0.5*(surface(fz0)+surface(fz1));
          for (l=tab1[fz0]-1; l<tab1[fz0+1]-1; l++)
            {
              if (tab2[l]-1==fz0)
                coeff[l]+=flux_Z;
              if (tab2[l]-1==fz1)
                coeff[l]-=flux_Z;
            }
          for (l=tab1[fz1]-1; l<tab1[fz1+1]-1; l++)
            {
              if (tab2[l]-1==fz0)
                coeff[l]-=flux_Z;
              if (tab2[l]-1==fz1)
                coeff[l]+=flux_Z;
            }
        }
    }


  // Boucle sur les bord pour traiter les faces de type sortie_libre

  int ndeb, nfin;
  int n_arete;
  ndeb = zvdf.premiere_arete_bord();
  nfin = ndeb + zvdf.nb_aretes_bord();


  int ori1,ori3,n_type,fac1,fac2,fac3,signe;


  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      n_type=type_arete_bord(n_arete-ndeb);

      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI: // paroi-paroi
          {
            break;
          }

        case TypeAreteBordVDF::FLUIDE_FLUIDE: // fluide-fluide
        case TypeAreteBordVDF::PAROI_FLUIDE: // paroi-fluide
          {
            fac1 = Qdm(n_arete,0);
            fac2 = Qdm(n_arete,1);
            fac3 = Qdm(n_arete,2);
            signe  = Qdm(n_arete,3);
            int ori1b = orientation(fac1);
            int ori3b = orientation(fac3);

            d_visco_turb = 0.5*(visco_turb(face_voisins(fac3,0))
                                + visco_turb(face_voisins(fac3,1)));

            if (ori1b == 0) // bord d'equation R = cte
              {
                double flux1;
                if (ori3b == 1)
                  {
                    flux1 = (visco_lam + d_visco_turb)
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else //if (ori3 == 2)
                  {
                    assert(ori3b == 2);
                    flux1 = (visco_lam + d_visco_turb)
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
                  {
                    if (tab2[l]-1==fac3)
                      coeff[l]+=signe*flux1;
                  }

                // Calcul de flux1_2 a coder
                // remplir les coeff des face fac1 et fac2
              }
            else if (ori1b == 1) // bord d'equation teta = cte
              {
                if (ori3b == 0)
                  {
                    double flux2;
                    flux2 = (visco_lam + d_visco_turb)
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
                      {
                        if (tab2[l]-1==fac3)
                          coeff[l]+=signe*flux2;
                      }

                    // Calcul de flux1_2 a coder
                    // et des coeff

                    // Termes supplementaires dans le laplacien en axi
                    // Ils sont integres comme des termes sources

                    coef_laplacien_axi = 0.5*(visco_lam + d_visco_turb);
                    for (l=tab1[fac1]-1; l<tab1[fac1+1]-1; l++)
                      {
                        if (tab2[l]-1==fac1)
                          coeff[l]+=coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
                      }
                    for (l=tab1[fac2]-1; l<tab1[fac2+1]-1; l++)
                      {
                        if (tab2[l]-1==fac2)
                          coeff[l]+=coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
                      }
                  }
                else if (ori3b == 2) // flux de tau23 a travers le bord
                  {
                    double flux3;
                    flux3 = (visco_lam+ d_visco_turb)
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

                    for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
                      {
                        if (tab2[l]-1==fac3)
                          coeff[l]+=signe*flux3;
                      }

                    // Calcul de flux1_2 a coder
                    // resu[fac1]+= flux1_2;
                    // resu[fac2]-= flux1_2;
                  }

              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                double flux4;
                if (ori3b == 0)
                  {
                    flux4 = (visco_lam + d_visco_turb)
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                else// if (ori3 == 1)  flux de tau32 a travers le bord
                  {
                    assert(ori3b == 1);
                    flux4 = (visco_lam + d_visco_turb)
                            *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
                  }
                // ATTENTION GROSSE CORRECTION LA PARANTHESE SUIVANTE ETAIT
                // MAL PLACEE
                // }
                for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
                  {
                    if (tab2[l]-1==fac3)
                      coeff[l]+=signe*flux4;
                  }
              }
            break;
          }
        case TypeAreteBordVDF::SYM_SYM:
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

  int fac4;

  // Boucle sur les aretes mixtes
  // Sur les aretes mixtes les termes croises du tenseur de Reynolds
  // sont nuls: il ne reste donc que la diffusion laminaire

  ndeb = zvdf.premiere_arete_mixte();
  nfin = zvdf.premiere_arete_interne();

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
          double flux1;
          // flux de mu_lam
          // sur la facette a cheval sur les faces fac1 et fac2

          flux1 = visco_lam*0.25*(surface(fac1)+surface(fac2))
                  *(porosite(fac1)+porosite(fac2));
          for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]+=flux1;
              if (tab2[l]-1==fac4)
                coeff[l]-=flux1;
            }
          for (l=tab1[fac4]-1; l<tab1[fac4+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]-=flux1;
              if (tab2[l]-1==fac4)
                coeff[l]+=flux1;
            }

          // Termes supplementaires dans le laplacien en axi
          // Ils sont integres comme des termes sources

          coef_laplacien_axi = 0.5*visco_lam;
          for (l=tab1[fac1]-1; l<tab1[fac1+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]+=coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
            }
          for (l=tab1[fac2]-1; l<tab1[fac2+1]-1; l++)
            {
              if (tab2[l]-1==fac2)
                coeff[l]+=coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
            }

          // flux de mu_lam
          // sur la facette a cheval sur les faces fac3 et fac4

          flux1 = visco_lam*0.25*(surface(fac3)+surface(fac4))
                  *(porosite(fac3)+porosite(fac4));
          for (l=tab1[fac1]-1; l<tab1[fac1+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]+=flux1;
              if (tab2[l]-1==fac2)
                coeff[l]-=flux1;
            }
          for (l=tab1[fac2]-1; l<tab1[fac2+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]-=flux1;
              if (tab2[l]-1==fac2)
                coeff[l]+=flux1;
            }
        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {
          // flux de mu_lam*tau32
          // sur la facette a cheval sur les faces fac1 et fac2
          double flux2;
          flux2 = visco_lam*0.25*(surface(fac1)+surface(fac2))
                  *(porosite(fac1)+porosite(fac2));
          for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]+=flux2;
              if (tab2[l]-1==fac4)
                coeff[l]-=flux2;
            }
          for (l=tab1[fac4]-1; l<tab1[fac4+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]-=flux2;
              if (tab2[l]-1==fac4)
                coeff[l]+=flux2;
            }

          // flux de mu_lam*tau23
          // sur la facette a cheval sur les faces fac3 et fac4

          flux2 = visco_lam*0.25*(surface(fac3)+surface(fac4))
                  *(porosite(fac3)+porosite(fac4));
          for (l=tab1[fac1]-1; l<tab1[fac1+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]+=flux2;
              if (tab2[l]-1==fac2)
                coeff[l]-=flux2;
            }
          for (l=tab1[fac2]-1; l<tab1[fac2+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]-=flux2;
              if (tab2[l]-1==fac2)
                coeff[l]+=flux2;
            }
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {
          // flux de mu_lam*tau31
          // sur la facette a cheval sur les faces fac1 et fac2
          double flux3;
          flux3 = visco_lam*0.25*(surface(fac1)+surface(fac2))
                  *(porosite(fac1)+porosite(fac2));
          for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]+=flux3;
              if (tab2[l]-1==fac4)
                coeff[l]-=flux3;
            }
          for (l=tab1[fac4]-1; l<tab1[fac4+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]-=flux3;
              if (tab2[l]-1==fac4)
                coeff[l]+=flux3;
            }

          // flux de mu_lam*tau13
          // sur la facette a cheval sur les faces fac3 et fac4

          flux3 = visco_lam*0.25*(surface(fac3)+surface(fac4))
                  *(porosite(fac3)+porosite(fac4));
          for (l=tab1[fac1]-1; l<tab1[fac1+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]+=flux3;
              if (tab2[l]-1==fac2)
                coeff[l]-=flux3;
            }
          for (l=tab1[fac2]-1; l<tab1[fac2+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]-=flux3;
              if (tab2[l]-1==fac2)
                coeff[l]+=flux3;
            }
        }
    }

  // Boucle sur les aretes internes

  ndeb = zvdf.premiere_arete_interne();
  nfin = zvdf.nb_aretes();

  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      fac1=Qdm(n_arete,0);
      fac2=Qdm(n_arete,1);
      fac3=Qdm(n_arete,2);
      fac4=Qdm(n_arete,3);
      ori1 = orientation(fac1);
      ori3 = orientation(fac3);

      // Calcul de la viscosite turbulente au milieu de l'arete

      d_visco_turb = 0.25*(visco_turb(face_voisins(fac3,0)) + visco_turb(face_voisins(fac3,1))
                           + visco_turb(face_voisins(fac4,0)) + visco_turb(face_voisins(fac4,1)));

      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          // flux de mu_lam + mu_turb
          // sur la facette a cheval sur les faces fac1 et fac2
          double flux1;
          flux1 = (visco_lam + d_visco_turb)
                  *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]+=flux1;
              if (tab2[l]-1==fac4)
                coeff[l]-=flux1;
            }
          for (l=tab1[fac4]-1; l<tab1[fac4+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]-=flux1;
              if (tab2[l]-1==fac4)
                coeff[l]+=flux1;
            }

          // Termes supplementaires dans le laplacien en axi
          // Ils sont integres comme des termes sources

          coef_laplacien_axi = 0.5*(visco_lam + d_visco_turb);
          for (l=tab1[fac1]-1; l<tab1[fac1+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]+=coef_laplacien_axi*volumes_entrelaces(fac1)*porosite(fac1)/xv(fac1,0);
            }
          for (l=tab1[fac2]-1; l<tab1[fac2+1]-1; l++)
            {
              if (tab2[l]-1==fac2)
                coeff[l]+=coef_laplacien_axi*volumes_entrelaces(fac2)*porosite(fac2)/xv(fac2,0);
            }

          // flux de mu_lam*tau12 + mu_turb*(tau21+tau12)
          // sur la facette a cheval sur les faces fac3 et fac4

          flux1 = (visco_lam + d_visco_turb)
                  *0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          for (l=tab1[fac1]-1; l<tab1[fac1+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]+=flux1;
              if (tab2[l]-1==fac2)
                coeff[l]-=flux1;
            }
          for (l=tab1[fac2]-1; l<tab1[fac2+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]-=flux1;
              if (tab2[l]-1==fac2)
                coeff[l]+=flux1;
            }
        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {
          double flux2;
          // flux de mu_lam*tau32 + mu_turb*(tau32+tau23)
          // sur la facette a cheval sur les faces fac1 et fac2

          flux2 = (visco_lam + d_visco_turb)
                  *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));

          for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]+=flux2;
              if (tab2[l]-1==fac4)
                coeff[l]-=flux2;
            }
          for (l=tab1[fac4]-1; l<tab1[fac4+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]-=flux2;
              if (tab2[l]-1==fac4)
                coeff[l]+=flux2;
            }

          // flux de mu_lam*tau23 + mu_turb*(tau32+tau23)
          // sur la facette a cheval sur les faces fac3 et fac4

          flux2 = (visco_lam + d_visco_turb)
                  *0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          for (l=tab1[fac1]-1; l<tab1[fac1+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]+=flux2;
              if (tab2[l]-1==fac2)
                coeff[l]-=flux2;
            }
          for (l=tab1[fac2]-1; l<tab1[fac2+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]-=flux2;
              if (tab2[l]-1==fac2)
                coeff[l]+=flux2;
            }
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {
          // flux de  mu_lam*tau31 + mu_turb*(tau31+tau13)
          // sur la facette a cheval sur les faces fac1 et fac2
          double flux3;
          flux3 = (visco_lam + d_visco_turb)
                  *0.25*(surface(fac1)+surface(fac2))*(porosite(fac1)+porosite(fac2));
          for (l=tab1[fac3]-1; l<tab1[fac3+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]+=flux3;
              if (tab2[l]-1==fac4)
                coeff[l]-=flux3;
            }
          for (l=tab1[fac4]-1; l<tab1[fac4+1]-1; l++)
            {
              if (tab2[l]-1==fac3)
                coeff[l]-=flux3;
              if (tab2[l]-1==fac4)
                coeff[l]+=flux3;
            }

          // flux de mu_lam*tau13 + mu_turb*(tau13+tau31)
          // sur la facette a cheval sur les faces fac3 et fac4

          flux3 = (visco_lam + d_visco_turb)
                  *0.25*(surface(fac3)+surface(fac4))*(porosite(fac3)+porosite(fac4));
          for (l=tab1[fac1]-1; l<tab1[fac1+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]+=flux3;
              if (tab2[l]-1==fac2)
                coeff[l]-=flux3;
            }
          for (l=tab1[fac2]-1; l<tab1[fac2+1]-1; l++)
            {
              if (tab2[l]-1==fac1)
                coeff[l]-=flux3;
              if (tab2[l]-1==fac2)
                coeff[l]+=flux3;
            }
        }

    }
}

void Op_Dift_VDF_Face_Axi::contribue_au_second_membre(DoubleTab& resu ) const
{

  double temps=equation().schema_temps().temps_courant();
  ref_cast_non_const(Op_Dift_VDF_Face_Axi, (*this)).mettre_a_jour(temps);
  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  //  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  //  int nb_elem = zvdf.nb_elem();
  //  int nb_aretes = zvdf.nb_aretes();

  //  const Champ_Uniforme& viscosite_lam = diffusivite_;
  //  double visco_lam = viscosite_lam(0,0);
  //  const DoubleVect& visco_turb = diffusivite_turbulente()->valeurs();

  // Boucle sur les bord pour traiter les faces de type sortie_libre

  int ndeb, nfin;

  // Boucle sur les aretes bord

  int n_arete;
  ndeb = zvdf.premiere_arete_bord();
  nfin = ndeb + zvdf.nb_aretes_bord();

  double flux;
  int n_type,ori3,fac1,fac2,fac3;


  for (n_arete=ndeb; n_arete<nfin; n_arete++)
    {
      n_type=type_arete_bord(n_arete-ndeb);

      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI: // paroi-paroi
          {
            fac1 = Qdm(n_arete,0);
            fac2 = Qdm(n_arete,1);
            fac3 = Qdm(n_arete,2);
            ori3 = orientation(fac3);
            int rang1 = (fac1-zvdf.premiere_face_bord());
            int rang2 = (fac2-zvdf.premiere_face_bord());
            double vit = inconnue.valeur()(fac3);
            double vit_imp = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori3,la_zcl_vdf)+
                                  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori3,la_zcl_vdf));

            double tau1 = tau_tan(rang1,ori3)*0.5*surface(fac1);
            double tau2 = tau_tan(rang2,ori3)*0.5*surface(fac2);
            double coef = tau1+tau2;

            int signe_terme = ( vit < vit_imp ) ? -1 : 1;

            //YB : 30/01/2004 :
            //Elimination de la determination du signe du flux parietal par l'evaluateur de l'operateur de diffusion,
            //en contre-partie le cisaillement parietal fournit par la loi paroi est signe (voir modif dans distances_VDF.cpp)
            signe_terme = 1;
            //Fin modif YB

            flux = signe_terme*coef;

            resu[fac3]+= flux;
            break;
          }

        case TypeAreteBordVDF::FLUIDE_FLUIDE : // fluide-fluide
        case TypeAreteBordVDF::PAROI_FLUIDE: // paroi-fluide
        case TypeAreteBordVDF::SYM_SYM:
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
double Op_Dift_VDF_Face_Axi::calculer_dt_stab() const
{
  return Op_Dift_VDF_Face_base::calculer_dt_stab(la_zone_vdf.valeur()) ;
}

