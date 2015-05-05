/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Op_Diff_K_Eps_QC_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_K_Eps_QC_VEF_Face.h>
#include <Modele_turbulence_hyd_K_Eps.h>
#include <Champ_P1NC.h>
#include <Les_Cl.h>
#include <Paroi_hyd_base_VEF.h>
#include <Fluide_Quasi_Compressible.h>
#include <DoubleTrav.h>

Implemente_instanciable(Op_Diff_K_Eps_QC_VEF_Face,"Op_Diff_K_Eps_QC_VEF_P1NC",Op_Diff_K_Eps_VEF_Face);

////  printOn
//


Sortie& Op_Diff_K_Eps_QC_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Diff_K_Eps_QC_VEF_Face::readOn(Entree& s )
{
  return s ;
}

// ATTENTION le diffu intervenant dans les fonctions n'est que LOCAL (on appelle d_mu apres)

// Fonction utile visc
// mu <Si, Sj> / |K|
static double viscA_QC(const Zone_VEF& la_zone,int num_face,int num2,int dimension,
                       int num_elem,double diffu)
{
  double pscal;

  pscal = la_zone.face_normales(num_face,0)*la_zone.face_normales(num2,0)
          + la_zone.face_normales(num_face,1)*la_zone.face_normales(num2,1);

  if (dimension == 3)
    pscal += la_zone.face_normales(num_face,2)*la_zone.face_normales(num2,2);

  if ( (la_zone.face_voisins(num_face,0) == la_zone.face_voisins(num2,0)) ||
       (la_zone.face_voisins(num_face,1) == la_zone.face_voisins(num2,1)))
    return -(pscal*diffu)/(la_zone.volumes(num_elem)*la_zone.porosite_elem(num_elem));
  else
    return (pscal*diffu)/(la_zone.volumes(num_elem)*la_zone.porosite_elem(num_elem));

}

DoubleTab& Op_Diff_K_Eps_QC_VEF_Face::ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  DoubleTrav KEps_divided_by_rho(inconnue);
  const Transport_K_Eps& eqn_transport = ref_cast(Transport_K_Eps,mon_equation.valeur());
  const Fluide_Quasi_Compressible& mil = ref_cast(Fluide_Quasi_Compressible,eqn_transport.milieu());
  const DoubleTab& rho=mil.masse_volumique();
  int size = inconnue.dimension_tot(0);
  for (int i=0; i<size; i++)
    {
      KEps_divided_by_rho(i,0) = inconnue(i,0) / rho(i); // k/rho
      KEps_divided_by_rho(i,1) = inconnue(i,1) / rho(i); // eps/rho
    }
  return Op_Diff_K_Eps_VEF_Face::ajouter(KEps_divided_by_rho, resu);
}

/////////////////////////////////////////
// Methode pour l'implicite
/////////////////////////////////////////

void Op_Diff_K_Eps_QC_VEF_Face::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const

{
  const Transport_K_Eps& eqn_transport = ref_cast(Transport_K_Eps,mon_equation.valeur());
  const Fluide_Quasi_Compressible& mil = ref_cast(Fluide_Quasi_Compressible,eqn_transport.milieu());
  const DoubleTab& mvol=mil.masse_volumique();

  modifier_matrice_pour_periodique_avant_contribuer(matrice,equation());
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int i,j,nc0,l,num_face;
  int n1 = zone_VEF.nb_faces();
  int elem;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double  d_mu,Prdt[2];
  Prdt[0]=Prdt_K;
  Prdt[1]=Prdt_Eps;
  const DoubleTab& mu_turb=diffusivite_turbulente_->valeurs();
  int nb_comp = 1;
  int nb_dim = transporte.nb_dim();

  if(nb_dim==2)
    nb_comp=transporte.dimension(1);
  assert(nb_comp==2);

  // On traite les faces bord
  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      if (sub_type(Periodique,la_cl.valeur()))
        {
          int nfinp = ndeb + le_bord.nb_faces()/2;
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;


          for (num_face=ndeb; num_face<nfinp; num_face++)
            {
              int elem1 = face_voisins(num_face,0);
              d_mu = mu_turb(elem1);
              fac_asso = la_cl_perio.face_associee(num_face-ndeb)+ndeb;
              int ok=1;
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( ( (j= elem_faces(elem1,i)) > num_face ))
                    {
                      double valAp = viscA_QC(zone_VEF,num_face,j,dimension,elem1,d_mu);
                      int fac_loc=0;
                      while ((fac_loc<nb_faces_elem) && (elem_faces(elem1,fac_loc)!=num_face)) fac_loc++;
                      if (fac_loc==nb_faces_elem)
                        ok=0;
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          int n0=num_face*nb_comp+nc;

                          int j0=j*nb_comp+nc;
                          double val=valAp/Prdt[nc];
                          matrice(n0,n0)+=val/mvol(num_face);
                          matrice(n0,j0)-=val/mvol(j);
                          if (!ok)
                            n0=fac_asso*nb_comp+nc;
                          matrice(j0,n0)-=val/mvol(num_face);
                          matrice(j0,j0)+=val/mvol(j);

                        }
                    }
                }
              int elem2 = face_voisins(num_face,1);
              d_mu = mu_turb(elem2);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( ( (j= elem_faces(elem2,i)) > num_face ))
                    {
                      double valAp = viscA_QC(zone_VEF,num_face,j,dimension,elem2,d_mu);
                      int fac_loc=0;
                      while ((fac_loc<nb_faces_elem) && (elem_faces(elem2,fac_loc)!=num_face)) fac_loc++;
                      if (fac_loc==nb_faces_elem)
                        ok=0;
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          int n0=num_face*nb_comp+nc;
                          int j0=j*nb_comp+nc;
                          double val=valAp/Prdt[nc];
                          matrice(n0,n0)+=val/mvol(num_face);
                          matrice(n0,j0)-=val/mvol(j);
                          if (!ok)
                            n0=fac_asso*nb_comp+nc;
                          matrice(j0,n0)-=val/mvol(num_face);
                          matrice(j0,j0)+=val/mvol(j);

                        }
                    }
                }
            }
        }

      else   //  conditions aux limites autres que periodicite
        {
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              elem = face_voisins(num_face,0);
              d_mu = mu_turb(elem);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem,i)) > num_face )
                    {
                      double valAp = viscA_QC(zone_VEF,num_face,j,dimension,elem,d_mu);
                      for ( nc0=0; nc0<nb_comp; nc0++)
                        {
                          int n0=num_face*nb_comp+nc0;
                          int j0=j*nb_comp+nc0;
                          double valA=valAp/Prdt[nc0];
                          matrice(n0,n0)+=valA/mvol(num_face);
                          matrice(n0,j0)-=valA/mvol(j);
                          matrice(j0,n0)-=valA/mvol(num_face);
                          matrice(j0,j0)+=valA/mvol(j);

                        }

                    }
                }
            }
        }
    }

  // On traite les faces internes

  for (num_face=zone_VEF.premiere_face_int(); num_face<n1; num_face++)
    {
      for ( l=0; l<2; l++)
        {
          elem = face_voisins(num_face,l);
          d_mu = mu_turb(elem);
          for (i=0; i<nb_faces_elem; i++)
            {
              if ( (j= elem_faces(elem,i)) > num_face )
                {
                  double valAp = viscA_QC(zone_VEF,num_face,j,dimension,elem,d_mu);
                  for ( nc0=0; nc0<nb_comp; nc0++)
                    {
                      double valA=valAp/Prdt[nc0];
                      int n0=num_face*nb_comp+nc0;
                      int j0=j*nb_comp+nc0;
                      matrice(n0,n0)+=valA/mvol(num_face);
                      matrice(n0,j0)-=valA/mvol(j);
                      matrice(j0,n0)-=valA/mvol(num_face);
                      matrice(j0,j0)+=valA/mvol(j);

                    }

                }
            }
        }
    }
  modifier_matrice_pour_periodique_apres_contribuer(matrice,equation());
}
