/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Op_Grad_PolyMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Grad_PolyMAC_Face.h>
#include <Champ_P0_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>
#include <Neumann_sortie_libre.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Check_espace_virtuel.h>
#include <communications.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <DoubleTrav.h>

Implemente_instanciable(Op_Grad_PolyMAC_Face,"Op_Grad_PolyMAC_Face",Operateur_Grad_base);


//// printOn
//

Sortie& Op_Grad_PolyMAC_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Grad_PolyMAC_Face::readOn(Entree& s)
{
  return s ;
}



// Description:
void Op_Grad_PolyMAC_Face::associer(const Zone_dis& zone_dis,
                                    const Zone_Cl_dis& zone_Cl_dis,
                                    const Champ_Inc& )
{
  const Zone_PolyMAC& zpolymac = ref_cast(Zone_PolyMAC, zone_dis.valeur());
  const Zone_Cl_PolyMAC& zclpolymac = ref_cast(Zone_Cl_PolyMAC, zone_Cl_dis.valeur());
  la_zone_polymac = zpolymac;
  la_zcl_polymac = zclpolymac;

  porosite_surf.ref(zpolymac.porosite_face());
  volume_entrelaces.ref(zpolymac.volumes_entrelaces());
  face_voisins.ref(zpolymac.face_voisins());
  xp.ref(zpolymac.xp());
}

void Op_Grad_PolyMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  const Zone_PolyMAC& zpolymac = la_zone_polymac.valeur();
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (int f = 0; f < zpolymac.nb_faces(); f++) for (int i = 0, e; i < 2 && (e = zpolymac.face_voisins(f, i)) >= 0; i++)
      stencil.append_line(f, e);
  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(zpolymac.nb_faces_tot(), zpolymac.nb_elem_tot(), stencil, mat);
}

DoubleTab& Op_Grad_PolyMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  assert_espace_virtuel_vect(inco);
  const Zone_PolyMAC& zpolymac = la_zone_polymac.valeur();
  const Zone_Cl_PolyMAC& zclpolymac = la_zcl_polymac.valeur();
  const DoubleVect& face_surfaces = zpolymac.face_surfaces();

  double coef;
  int n0, n1;

  // Boucle sur les bords pour traiter les conditions aux limites
  int ndeb, nfin, num_face;
  for (int n_bord=0; n_bord<zpolymac.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = zclpolymac.les_conditions_limites(n_bord);
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
                  coef = face_surfaces(num_face)*porosite_surf(num_face);
                  resu(num_face) += (coef*(P_imp - inco(n0)));
                }
              else
                {
                  n1 = face_voisins(num_face,1);
                  coef = face_surfaces(num_face)*porosite_surf(num_face);
                  resu(num_face) += (coef*(inco(n1) - P_imp));
                }
            }
        }
      // Fin de la boucle for
    }

  // Boucle sur les faces internes
  for (num_face=zpolymac.premiere_face_int(); num_face<zpolymac.nb_faces(); num_face++)
    {
      n0 = face_voisins(num_face,0);
      n1 = face_voisins(num_face,1);
      coef = face_surfaces(num_face)*porosite_surf(num_face);
      resu(num_face) += coef*(inco(n1)-inco(n0));
    }
  resu.echange_espace_virtuel();
  return resu;
}

DoubleTab& Op_Grad_PolyMAC_Face::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}

void Op_Grad_PolyMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& la_matrice) const
{
  assert_espace_virtuel_vect(inco);
  const Zone_PolyMAC& zpolymac = la_zone_polymac.valeur();
  const Zone_Cl_PolyMAC& zclpolymac = la_zcl_polymac.valeur();
  const DoubleVect& face_surfaces = zpolymac.face_surfaces();

  double coef;
  int n0, n1;

  // Boucle sur les bords pour traiter les conditions aux limites
  int ndeb, nfin, num_face;
  for (int n_bord=0; n_bord<zpolymac.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = zclpolymac.les_conditions_limites(n_bord);
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
                  coef = face_surfaces(num_face)*porosite_surf(num_face);
                  la_matrice(num_face, n0) += coef;
                }
              else
                {
                  n1 = face_voisins(num_face,1);
                  coef = face_surfaces(num_face)*porosite_surf(num_face);
                  la_matrice(num_face, n1) -= coef;
                }
            }
        }
      // Fin de la boucle for
    }

  // Boucle sur les faces internes
  for (num_face=zpolymac.premiere_face_int(); num_face<zpolymac.nb_faces(); num_face++)
    {
      n0 = face_voisins(num_face,0);
      n1 = face_voisins(num_face,1);
      coef = face_surfaces(num_face)*porosite_surf(num_face);
      la_matrice(num_face, n0) += coef;
      la_matrice(num_face, n1) -= coef;
    }
}

int Op_Grad_PolyMAC_Face::impr(Sortie& os) const
{
  return 0;
  const int& impr_mom=la_zone_polymac->zone().Moments_a_imprimer();
  const int impr_sum=(la_zone_polymac->zone().Bords_a_imprimer_sum().est_vide() ? 0:1);
  const int impr_bord=(la_zone_polymac->zone().Bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = equation().probleme().schema_temps();
  const Zone_PolyMAC& zpolymac = la_zone_polymac.valeur();
  const Zone_Cl_PolyMAC& zclpolymac = la_zcl_polymac.valeur();
  const DoubleVect& face_surfaces = zpolymac.face_surfaces();
  const Equation_base& eqn = equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,eqn);
  const Champ_P0_PolyMAC& la_pression_P0 = ref_cast(Champ_P0_PolyMAC,eqn_hydr.pression_pa().valeur());
  const DoubleTab& pression_P0 = la_pression_P0.valeurs();
  int elem0;
  int face;
  double n0;//, n1, n2 ;
  const int nb_faces =  zpolymac.nb_faces_tot();
  DoubleTab xgr(nb_faces,dimension);
  xgr=0.;
  if (impr_mom)
    {
      const DoubleTab& xgrav = zpolymac.xv();
      const ArrOfDouble& c_grav=zpolymac.zone().cg_moments();
      for (int num_face=0; num_face <nb_faces; num_face++)
        for (int i=0; i<dimension; i++)
          xgr(num_face,i)=xgrav(num_face,i)-c_grav(i);
    }

  flux_bords_.resize(zpolymac.nb_faces_bord(),dimension);
  flux_bords_ = 0.;
  // flux_bords contains the sum of flux on each boundary:
  DoubleTrav tab_flux_bords(3,zpolymac.nb_front_Cl(),3);
  tab_flux_bords=0.;
  /*  flux_bord(k)          ->   flux_bords2(0,num_cl,k)
      flux_bord_perio1(k)   ->   flux_bords2(1,num_cl,k)
      flux_bord_perio2(k)   ->   flux_bords2(2,num_cl,k)
      moment(k)             ->   flux_bords2(3,num_cl,k) */
  int nb_bord =  zpolymac.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zclpolymac.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int impr_boundary = (zpolymac.zone().Bords_a_imprimer_sum().contient(le_bord.le_nom()) ? 1 : 0);
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      for (face=ndeb; face<nfin; face++)
        {
          elem0 = face_voisins(face,0);
          n0 = face_surfaces(face)*porosite_surf(face);
          for (int r = 0; r < dimension; r++)
            {
              flux_bords_(face,r) = dabs(zpolymac.face_normales(face, r) / face_surfaces(face)) * (pression_P0(elem0))*n0;
              tab_flux_bords(0, n_bord, r) += flux_bords_(face,r);
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
      SFichier Flux_grad;
      ouvrir_fichier(Flux_grad,"",1);
      SFichier Flux_grad_moment;
      ouvrir_fichier(Flux_grad_moment,"moment",impr_mom);
      SFichier Flux_grad_sum;
      ouvrir_fichier(Flux_grad_sum,"sum",impr_sum);
      Flux_grad.add_col(sch.temps_courant());
      if (impr_mom) Flux_grad_moment.add_col(sch.temps_courant());
      if (impr_sum) Flux_grad_sum.add_col(sch.temps_courant());
      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          for (int r = 0; r < dimension; r++)
            {
              Flux_grad.add_col(tab_flux_bords(0, n_bord, r));
              if (impr_sum) Flux_grad_sum.add_col(tab_flux_bords(1, n_bord, r));
            }
          if (impr_mom) for (int r = (dimension < 3 ? 2 : 0); r < max(dimension, 3); r++)
              Flux_grad_moment.add_col(tab_flux_bords(2, n_bord, r));
        }
      Flux_grad << finl;
      if (impr_sum) Flux_grad_sum << finl;
      if (impr_mom) Flux_grad_moment << finl;
    }

  const LIST(Nom)& Liste_Bords_a_imprimer = zpolymac.zone().Bords_a_imprimer();
  if (!Liste_Bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_grad_face;
      ouvrir_fichier_partage(Flux_grad_face,"",impr_bord);
      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = zclpolymac.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          if (zpolymac.zone().Bords_a_imprimer().contient(le_bord.le_nom()))
            {
              if (je_suis_maitre())
                {
                  Flux_grad_face << "# Force par face sur " << le_bord.le_nom() << " au temps ";
                  sch.imprimer_temps_courant(Flux_grad_face);
                  Flux_grad_face << " : " << finl;
                }
              for (face=ndeb; face<nfin; face++)
                {
                  Flux_grad_face << "# Face a x= " << zpolymac.xv(face,0) << " y= " << zpolymac.xv(face,1);
                  if (dimension==3) Flux_grad_face << " z= " << zpolymac.xv(face,2);
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
