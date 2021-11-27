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
// File:        Op_Grad_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Divers
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Grad_VDF_Face.h>
#include <Champ_P0_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Neumann_sortie_libre.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Option_VDF.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Check_espace_virtuel.h>
#include <communications.h>
#include <DoubleTrav.h>
#include <Array_tools.h>
#include <Matrix_tools.h>

Implemente_instanciable(Op_Grad_VDF_Face,"Op_Grad_VDF_Face",Operateur_Grad_base);


//// printOn
//

Sortie& Op_Grad_VDF_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Grad_VDF_Face::readOn(Entree& s)
{
  return s ;
}



// Description:
void Op_Grad_VDF_Face::associer(const Zone_dis& zone_dis,
                                const Zone_Cl_dis& zone_Cl_dis,
                                const Champ_Inc& )
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF, zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
  la_zone_vdf = zvdf;
  la_zcl_vdf = zclvdf;

  porosite_surf.ref(zvdf.porosite_face());
  volume_entrelaces.ref(zvdf.volumes_entrelaces());
  face_voisins.ref(zvdf.face_voisins());
  orientation.ref(zvdf.orientation());
  xp.ref(zvdf.xp());

}

void Op_Grad_VDF_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  int e;
  for (int f = 0; f < zvdf.nb_faces(); f++) for (int i = 0; i < 2; i++)
      if ((e = zvdf.face_voisins(f, i)) >= 0) stencil.append_line(f, e);
  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(zvdf.nb_faces_tot(), zvdf.nb_elem_tot(), stencil, mat);
}

DoubleTab& Op_Grad_VDF_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  assert_espace_virtuel_vect(inco);
  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  const DoubleVect& face_surfaces = zvdf.face_surfaces();

  double coef;
  int n0, n1;

  // Boucle sur les bords pour traiter les conditions aux limites
  int ndeb, nfin, num_face;
  for (int n_bord=0; n_bord<zvdf.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
      if ( sub_type(Neumann_sortie_libre,la_cl.valeur()) )
        {
          const Neumann_sortie_libre& la_cl_typee =
            ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              double P_imp = la_cl_typee.flux_impose(num_face-ndeb);
              n0 = face_voisins(num_face,0);
              if (n0 != -1)
                {
                  coef = face_surfaces(num_face)*porosite_surf(num_face)*Option_VDF::coeff_P_neumann;
                  resu(num_face) += (coef*(P_imp - inco(n0)));
                }
              else
                {
                  n1 = face_voisins(num_face,1);
                  coef = face_surfaces(num_face)*porosite_surf(num_face)*Option_VDF::coeff_P_neumann;
                  resu(num_face) += (coef*(inco(n1) - P_imp));
                }
            }
        }

      // Correction periodicite :
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              n0 = face_voisins(num_face,0);
              n1 = face_voisins(num_face,1);
              coef = face_surfaces(num_face)*porosite_surf(num_face);
              resu(num_face) += (coef*(inco(n1) - inco(n0)));
            }
        }
      else if (sub_type(Symetrie,la_cl.valeur()))
        ;
      else if ( (sub_type(Dirichlet,la_cl.valeur()))
                ||
                (sub_type(Dirichlet_homogene,la_cl.valeur()))
              )
        {
          // do nothing
          ;
        }

      // Fin de la boucle for
    }

  // Boucle sur les faces internes
  for (num_face=zvdf.premiere_face_int(); num_face<zvdf.nb_faces(); num_face++)
    {
      n0 = face_voisins(num_face,0);
      n1 = face_voisins(num_face,1);
      coef = face_surfaces(num_face)*porosite_surf(num_face);
      resu(num_face) += coef*(inco(n1)-inco(n0));
    }
  resu.echange_espace_virtuel();
  return resu;
}

DoubleTab& Op_Grad_VDF_Face::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}

void Op_Grad_VDF_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& la_matrice) const
{
  assert_espace_virtuel_vect(inco);
  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  const DoubleVect& face_surfaces = zvdf.face_surfaces();

  double coef;
  int n0, n1;

  // Boucle sur les bords pour traiter les conditions aux limites
  int ndeb, nfin, num_face;
  for (int n_bord=0; n_bord<zvdf.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
      if ( sub_type(Neumann_sortie_libre,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              n0 = face_voisins(num_face,0);
              if (n0 != -1)
                {
                  coef = face_surfaces(num_face)*porosite_surf(num_face) * Option_VDF::coeff_P_neumann;
                  la_matrice(num_face, n0) += coef;
                }
              else
                {
                  n1 = face_voisins(num_face,1);
                  coef = face_surfaces(num_face)*porosite_surf(num_face) * Option_VDF::coeff_P_neumann;
                  la_matrice(num_face, n1) -= coef;
                }
            }
        }
      // Fin de la boucle for
    }

  // Boucle sur les faces internes
  for (num_face=zvdf.premiere_face_int(); num_face<zvdf.nb_faces(); num_face++)
    {
      n0 = face_voisins(num_face,0);
      n1 = face_voisins(num_face,1);
      coef = face_surfaces(num_face)*porosite_surf(num_face);
      la_matrice(num_face, n0) += coef;
      la_matrice(num_face, n1) -= coef;
    }
}

int Op_Grad_VDF_Face::impr(Sortie& os) const
{
  const int& impr_mom=la_zone_vdf->zone().Moments_a_imprimer();
  const int impr_sum=(la_zone_vdf->zone().Bords_a_imprimer_sum().est_vide() ? 0:1);
  const int impr_bord=(la_zone_vdf->zone().Bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = equation().probleme().schema_temps();
  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  const DoubleVect& face_surfaces = zvdf.face_surfaces();
  const Equation_base& eqn = equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,eqn);
  const Champ_P0_VDF& la_pression_P0 = ref_cast(Champ_P0_VDF,eqn_hydr.pression_pa().valeur());
  const DoubleTab& pression_P0 = la_pression_P0.valeurs();
  int elem0, elem1 ;
  int face, ori;
  double n0;//, n1, n2 ;
  const int nb_faces =  zvdf.nb_faces_tot();
  DoubleTab xgr(nb_faces,dimension);
  xgr=0.;
  if (impr_mom)
    {
      const DoubleTab& xgrav = zvdf.xv();
      const ArrOfDouble& c_grav=zvdf.zone().cg_moments();
      for (int num_face=0; num_face <nb_faces; num_face++)
        for (int i=0; i<dimension; i++)
          xgr(num_face,i)=xgrav(num_face,i)-c_grav(i);
    }

  flux_bords_.resize(zvdf.nb_faces_bord(),dimension);
  flux_bords_ = 0.;
  // flux_bords contains the sum of flux on each boundary:
  DoubleTrav tab_flux_bords(3,zvdf.nb_front_Cl(),3);
  tab_flux_bords=0.;
  /*  flux_bord(k)          ->   flux_bords2(0,num_cl,k)
      flux_bord_perio1(k)   ->   flux_bords2(1,num_cl,k)
      flux_bord_perio2(k)   ->   flux_bords2(2,num_cl,k)
      moment(k)             ->   flux_bords2(3,num_cl,k) */
  int nb_bord =  zvdf.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int impr_boundary = (zvdf.zone().Bords_a_imprimer_sum().contient(le_bord.le_nom()) ? 1 : 0);
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      for (face=ndeb; face<nfin; face++)
        {
          elem0 = face_voisins(face,0);
          ori = orientation(face);
          n0 = face_surfaces(face)*porosite_surf(face);
          if (ori == 0)
            {
              if (elem0 != -1) flux_bords_(face,0) = (pression_P0(elem0))*n0 ;
              else
                {
                  elem1 = face_voisins(face,1);
                  flux_bords_(face,0) = -(pression_P0(elem1))*n0 ;
                }
              tab_flux_bords(0, n_bord, 0) += flux_bords_(face,0) ;
            }
          else if (ori == 1)
            {
              if (elem0 != -1) flux_bords_(face,1) = (pression_P0(elem0))*n0 ;
              else
                {
                  elem1 = face_voisins(face,1);
                  flux_bords_(face,1) = -(pression_P0(elem1))*n0 ;
                }
              tab_flux_bords(0, n_bord, 1) += flux_bords_(face,1) ;
            }
          else if (ori == 2)
            {
              if (elem0 != -1) flux_bords_(face,2) = (pression_P0(elem0))*n0 ;
              else
                {
                  elem1 = face_voisins(face,1);
                  flux_bords_(face,2) = -(pression_P0(elem1))*n0 ;
                }
              tab_flux_bords(0, n_bord, 2) += flux_bords_(face,2) ;
            }

          if (dimension == 2)
            {
              if (impr_mom)
                tab_flux_bords(2, n_bord, 2) +=flux_bords_(face,1)*xgr(face,0)-flux_bords_(face,0)*xgr(face,1);
              if (impr_boundary)
                {
                  tab_flux_bords(1, n_bord, 0) += flux_bords_(face,0) ;
                  tab_flux_bords(1, n_bord, 1) += flux_bords_(face,1) ;
                }
            }
          else if (dimension == 3)
            {
              if (impr_mom)
                {
                  tab_flux_bords(2, n_bord, 0) +=flux_bords_(face,2)*xgr(face,1)-flux_bords_(face,1)*xgr(face,2);
                  tab_flux_bords(2, n_bord, 1) +=flux_bords_(face,0)*xgr(face,2)-flux_bords_(face,2)*xgr(face,0);
                  tab_flux_bords(2, n_bord, 2) +=flux_bords_(face,1)*xgr(face,0)-flux_bords_(face,0)*xgr(face,1);
                }
              if (impr_boundary)
                {
                  tab_flux_bords(1, n_bord, 0) += flux_bords_(face,0) ;
                  tab_flux_bords(1, n_bord, 1) += flux_bords_(face,1) ;
                  tab_flux_bords(1, n_bord, 2) += flux_bords_(face,2) ;
                }
            }
        } // fin for face
    } // fin for n_bord

  // Sum on all process:
  mp_sum_for_each_item(tab_flux_bords);

  // Write the boundary fluxes:
  if (je_suis_maitre())
    {
      //SFichier Flux_grad;
      if (!Flux_grad.is_open()) ouvrir_fichier(Flux_grad,"",1);
      //SFichier Flux_grad_moment;
      if (!Flux_grad_moment.is_open()) ouvrir_fichier(Flux_grad_moment,"moment",impr_mom);
      //SFichier Flux_grad_sum;
      if (!Flux_grad_sum.is_open()) ouvrir_fichier(Flux_grad_sum,"sum",impr_sum);
      Flux_grad.add_col(sch.temps_courant());
      if (impr_mom) Flux_grad_moment.add_col(sch.temps_courant());
      if (impr_sum) Flux_grad_sum.add_col(sch.temps_courant());
      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          if (dimension == 2)
            {
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 0));
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 1));
              if (impr_sum)
                {
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 0));
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 1));
                }
              if (impr_mom) Flux_grad_moment.add_col(tab_flux_bords(2, n_bord, 2));
            }

          if (dimension == 3)
            {
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 0));
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 1));
              Flux_grad.add_col(tab_flux_bords(0, n_bord, 2));
              if (impr_sum)
                {
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 0));
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 1));
                  Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, 2));
                }
              if (impr_mom)
                {
                  Flux_grad_moment.add_col(tab_flux_bords(2, n_bord, 0));
                  Flux_grad_moment.add_col(tab_flux_bords(2, n_bord, 1));
                  Flux_grad_moment.add_col(tab_flux_bords(2, n_bord, 2));
                }
            }
        }
      Flux_grad << finl;
      if (impr_sum) Flux_grad_sum << finl;
      if (impr_mom) Flux_grad_moment << finl;
    }

  const LIST(Nom)& Liste_Bords_a_imprimer = zvdf.zone().Bords_a_imprimer();
  if (!Liste_Bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_grad_face;
      ouvrir_fichier_partage(Flux_grad_face,"",impr_bord);
      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          if (zvdf.zone().Bords_a_imprimer().contient(le_bord.le_nom()))
            {
              if (je_suis_maitre())
                {
                  Flux_grad_face << "# Force par face sur " << le_bord.le_nom() << " au temps ";
                  sch.imprimer_temps_courant(Flux_grad_face);
                  Flux_grad_face << " : " << finl;
                }
              for (face=ndeb; face<nfin; face++)
                {
                  Flux_grad_face << "# Face a x= " << zvdf.xv(face,0) << " y= " << zvdf.xv(face,1);
                  if (dimension==3) Flux_grad_face << " z= " << zvdf.xv(face,2);
                  Flux_grad_face << " : Fx= " << flux_bords_(face, 0) << " Fy= " << flux_bords_(face, 1);
                  if (dimension==3) Flux_grad_face << " Fz= " << flux_bords_(face, 2);
                  Flux_grad_face << finl;
                }
              Flux_grad_face.syncfile();
            }
        }
    }
  return 1;
}
