/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Paroi_UTAU_IMP_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#include <Paroi_UTAU_IMP_VDF.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_VDF.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Fluide_Quasi_Compressible.h>
#include <Equation_base.h>
#include <Mod_turb_hyd_base.h>

Implemente_instanciable(Paroi_UTAU_IMP_VDF,"UTAU_IMP_VDF",Paroi_hyd_base_VDF);

//     printOn()
/////

Sortie& Paroi_UTAU_IMP_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_UTAU_IMP_VDF::readOn(Entree& s)
{
  return lire_donnees(s);
}


/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_UTAU_IMP_VDF
//
/////////////////////////////////////////////////////////////////////


int Paroi_UTAU_IMP_VDF::init_lois_paroi()
{
  init_lois_paroi_();

  return 1;
}


int Paroi_UTAU_IMP_VDF::calculer_hyd(DoubleTab& tab_k_eps)
{
  DoubleTab bidon(0);
  // bidon ne servira pas
  return calculer_hyd(tab_k_eps,1,bidon);
}
int Paroi_UTAU_IMP_VDF::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k)
{
  return calculer_hyd(tab_nu_t,0,tab_k);
}


int Paroi_UTAU_IMP_VDF::calculer_hyd(DoubleTab& tab1,int isKeps,DoubleTab& tab2)
{
  // si isKeps =1 tab1=tab_keps
  //                tab2 bidon
  // si isKeps=0 tab1=tab_nu_t
  //             tab2=tab_k
  //  Cerr<<" Paroi_UTAU_IMP_VDF::calculer_hyd"<<finl;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const DoubleVect& vit = eqn_hydr.inconnue().valeurs();
  const DoubleTab& xv=zone_VDF.xv() ;                   // centres de gravite des faces
  DoubleVect pos(dimension);
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();

  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  int l_unif;
  double visco=-1;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      l_unif = 1;
      visco = max(tab_visco(0,0),DMINFLOAT);
    }
  else
    l_unif = 0;

  if ((!l_unif) && (tab_visco.local_min_vect()<DMINFLOAT))
    //   on ne doit pas changer tab_visco ici !
    {
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    }
  //    tab_visco+=DMINFLOAT;



  int ndeb,nfin;
  int elem,ori;
  double signe;
  double norm_v;
  double d_visco;

  // Boucle sur les bords

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = le_bord.num_premiere_face();
      nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || sub_type(Dirichlet_paroi_defilante,la_cl.valeur() ))
        {
          int isdiri=0;
          DoubleTab vitesse_imposee_face_bord(le_bord.nb_faces(),dimension);
          if (sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
            {
              isdiri=1;
              const Dirichlet_paroi_defilante& cl_diri = ref_cast(Dirichlet_paroi_defilante,la_cl.valeur());
              for (int face=ndeb; face<nfin; face++)
                for (int k=0; k<dimension; k++)
                  vitesse_imposee_face_bord(face-ndeb,k) = cl_diri.val_imp(face-ndeb,k);
            }
          ArrOfDouble vit_paroi(dimension);
          ArrOfDouble val(dimension-1);

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              if (isdiri)
                {
                  int rang = num_face-ndeb;
                  for (int k=0; k<dimension; k++)
                    vit_paroi[k]=vitesse_imposee_face_bord(rang,k);
                }
              ori = orientation(num_face);
              if ( (elem =face_voisins(num_face,0)) != -1)
                {
                  norm_v=norm_vit(vit,elem,ori,zone_VDF,vit_paroi,val);
                  signe = -1.;
                }
              else
                {
                  elem = face_voisins(num_face,1);
                  //norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val);
                  norm_v=norm_vit(vit,elem,ori,zone_VDF,vit_paroi,val);
                  signe = 1.;
                }

              if (l_unif)
                d_visco = visco;
              else
                d_visco = tab_visco[elem];

              // Calcul de la position
              for (int i=0; i<dimension; i++)
                pos[i]=xv(num_face,i);


              // Calcul de la contrainte tangentielle

              //double ustar = u_star->valeur_a_compo(pos,0);
              double ustar = calculer_utau(pos, norm_v, d_visco);
              double vit_frot = ustar*ustar;

              if (ori == 0)
                {
                  Cisaillement_paroi_(num_face,1) = vit_frot*val[0]*signe;
                  if (dimension==3)
                    Cisaillement_paroi_(num_face,2) = vit_frot*val[1]*signe;
                  Cisaillement_paroi_(num_face,0) = 0.;
                }
              else if (ori == 1)
                {
                  Cisaillement_paroi_(num_face,0) = vit_frot*val[0]*signe;
                  if (dimension==3)
                    Cisaillement_paroi_(num_face,2) = vit_frot*val[1]*signe;
                  Cisaillement_paroi_(num_face,1) = 0.;
                }
              else
                {
                  Cisaillement_paroi_(num_face,0) = vit_frot*val[0]*signe;
                  Cisaillement_paroi_(num_face,1) = vit_frot*val[1]*signe;
                  Cisaillement_paroi_(num_face,2) = 0.;
                }
            }


        }
    }
  Cisaillement_paroi_.echange_espace_virtuel();
  tab1.echange_espace_virtuel();
  if (! isKeps) tab2.echange_espace_virtuel();
  return 1;
}

