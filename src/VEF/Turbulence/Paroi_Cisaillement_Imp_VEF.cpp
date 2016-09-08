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
// File:        Paroi_Cisaillement_Imp_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_Cisaillement_Imp_VEF.h>
#include <Champ_P1NC.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Dirichlet_paroi_fixe.h>
#include <Mod_turb_hyd_base.h>

Implemente_instanciable(Paroi_Cisaillement_Imp_VEF,"UTAU_IMP_VEF",Paroi_hyd_base_VEF);

//     printOn()
/////

Sortie& Paroi_Cisaillement_Imp_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_Cisaillement_Imp_VEF::readOn(Entree& s)
{
  return lire_donnees(s);
}


/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_Cisaillement_Imp_VEF
//
/////////////////////////////////////////////////////////////////////

int Paroi_Cisaillement_Imp_VEF::init_lois_paroi()
{
  Paroi_hyd_base_VEF::init_lois_paroi_();
  return 1;
}

int Paroi_Cisaillement_Imp_VEF::calculer_hyd(DoubleTab& tab_k_eps)
{
  calculer_hyd_commun();
  return 1;
}  // fin de calcul_hyd (K-eps)

int Paroi_Cisaillement_Imp_VEF::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k)
{
  calculer_hyd_commun();
  return 1;
}  // fin de calcul_hyd (LES)

int Paroi_Cisaillement_Imp_VEF::calculer_hyd_commun()
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& vit = eqn_hydr.inconnue().valeurs();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  const Zone& zone = zone_VEF.zone();
  int nfac = zone.nb_faces_elem();
  const DoubleTab& xv = zone_VEF.xv();    // centre de gravite des faces
  const DoubleTab& face_normale = zone_VEF.face_normales();

  ArrOfDouble v(dimension);
  DoubleVect pos(dimension);
  double visco=-1;
  int l_unif;
  int n_bord;

  tab_u_star_.resize(la_zone_VEF->nb_faces_tot());


  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    l_unif = 0;

  if ((!l_unif) && (tab_visco.local_min_vect()<DMINFLOAT))
    //   on ne doit pas changer tab_visco ici !
    {
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    }
  //tab_visco+=DMINFLOAT;

  int ndeb,nfin;
  double norm_v=-1;
  double d_visco;

  // Boucle sur les bords
  int nb_bords=zone_VEF.nb_front_Cl();
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const IntTab& elem_faces = zone_VEF.elem_faces();
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              int elem = face_voisins(num_face,0);
              if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur())) // donc triangle ou tetraedre
                {

                  // Calcul la moyenne de la vitesse et de la position aux faces (autre que bord) dans l'elem.
                  v=0.;
                  int Compte_face = 0;
                  for (int i=0; i<nfac; i++)
                    if (zone_VEF.premiere_face_int()<=elem_faces(elem,i))
                      {
                        int face = elem_faces(elem,i);
                        for (int dim=0; dim<dimension; dim++)
                          {
                            v[dim] += vit(face,dim);
                            pos[dim] += xv(face,dim);
                          }
                        Compte_face+=1 ;
                      }
                  for (int dim=0; dim<dimension; dim++)
                    {
                      v[dim]/=Compte_face ;
                      pos[dim]/=Compte_face ;
                    }

                  // Projection de la vitesse dans le plan tangentiel
                  double scal=0;
                  for (int dim=0; dim<dimension; dim++)
                    scal+=v[dim]*face_normale(num_face,dim);
                  double surf=0;
                  for (int dim=0; dim<dimension; dim++)
                    surf+=carre(face_normale(num_face,dim));
                  scal/=surf;
                  for (int dim=0; dim<dimension; dim++)
                    v[dim]-=face_normale(num_face,dim)*scal;

                  // Calcul de la norme
                  norm_v=0;
                  for (int dim=0; dim<dimension; dim++)
                    norm_v+=carre(v[dim]);
                  norm_v=sqrt(norm_v);
                }
              else
                {
                  Cerr << " On ne sait traiter que des champs P1NC dans Paroi_Cisaillement_Imp_VEF::calculer_hyd" << finl;
                  exit();
                }

              if (l_unif)
                d_visco = visco;
              else
                d_visco = tab_visco[elem];

              // Calcul de la contrainte tangentielle
              double ustar = calculer_utau(pos, norm_v, d_visco);

              tab_u_star_(num_face)=ustar;

              double norm_tau=ustar*ustar;
              norm_tau/=(norm_v+DMINFLOAT);
              for (int dim=0; dim<dimension; dim++)
                {
                  Cisaillement_paroi_(num_face,dim) = norm_tau*v[dim];
                }

            }

        } //  fin de Dirichlet_paroi_fixe
    } // fin du for bord CL

  return 1;
}  // fin du calcul_hyd (nu-t)


