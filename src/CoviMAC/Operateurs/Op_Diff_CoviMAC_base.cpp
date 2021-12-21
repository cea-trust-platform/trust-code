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
// File:        Op_Diff_CoviMAC_base.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_CoviMAC_base.h>
#include <Motcle.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Champ_Don_Fonc_xyz.h>
#include <Vecteur3.h>
#include <Matrice33.h>
#include <Linear_algebra_tools_impl.h>
#include <DoubleTrav.h>
#include <Check_espace_virtuel.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Milieu_base.h>
#include <SFichier.h>
#include <communications.h>
#include <EcrFicPartage.h>
#include <Modele_turbulence_scal_base.h>
#include <cfloat>
#include <Champ_P0_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Op_Diff_CoviMAC_Face.h>
#include <Pb_Multiphase.h>
#include <Echange_contact_CoviMAC.h>
#include <Flux_parietal_base.h>

Implemente_base(Op_Diff_CoviMAC_base,"Op_Diff_CoviMAC_base",Operateur_Diff_base);

//// printOn
//

Sortie& Op_Diff_CoviMAC_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Op_Diff_CoviMAC_base::readOn(Entree& s )
{
  return s ;
}

void Op_Diff_CoviMAC_base::mettre_a_jour(double t)
{
  Operateur_base::mettre_a_jour(t);
  //si le champ est constant en temps, alors pas besoin de recalculer nu_ et les interpolations
  if (t <= t_last_maj_) return;
  if (!nu_constant_) nu_a_jour_ = 0, phif_a_jour_ = 0;
  t_last_maj_ = t;
}


void Op_Diff_CoviMAC_base::completer()
{
  Operateur_base::completer();
  const Equation_base& eq = equation();
  int N = eq.inconnue().valeurs().line_size(), D = dimension, N_nu = std::max(N * dimension_min_nu(), diffusivite().valeurs().line_size());
  if (N_nu == N) nu_.resize(0, N); //isotrope
  else if (N_nu == N * D) nu_.resize(0, N, D); //diagonal
  else if (N_nu == N * D * D) nu_.resize(0, N, D, D); //complet
  else Process::exit(Nom("Op_Diff_CoviMAC_base : diffusivity component count ") + Nom(N_nu) + "not among (" + Nom(N) + ", " + Nom(N * D) + ", " + Nom(N * D * D)  + ")!");
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  zone.zone().creer_tableau_elements(nu_);

  nu_constant_ = (sub_type(Champ_Uniforme, diffusivite()) || sub_type(Champ_Don_Fonc_xyz, diffusivite())) ;
}

int Op_Diff_CoviMAC_base::impr(Sortie& os) const
{
  const Zone& ma_zone=la_zone_poly_->zone();
  const int& impr_mom=ma_zone.Moments_a_imprimer();
  const int impr_sum=(ma_zone.Bords_a_imprimer_sum().est_vide() ? 0:1);
  const int impr_bord=(ma_zone.Bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = la_zcl_poly_->equation().probleme().schema_temps();
  DoubleTab& tab_flux_bords= flux_bords();
  int nb_comp = tab_flux_bords.nb_dim() > 1 ? tab_flux_bords.dimension(1) : 0;
  DoubleVect bilan(nb_comp);
  const int nb_faces = la_zone_poly_->nb_faces_tot();
  DoubleTab xgr(nb_faces,dimension);
  xgr=0.;
  if (impr_mom)
    {
      const DoubleTab& xgrav = la_zone_poly_->xv();
      const ArrOfDouble& c_grav=ma_zone.cg_moments();
      for (int num_face=0; num_face <nb_faces; num_face++)
        for (int i=0; i<dimension; i++)
          xgr(num_face,i)=xgrav(num_face,i)-c_grav(i);
    }
  int k,face;
  int nb_front_Cl=la_zone_poly_->nb_front_Cl();
  DoubleTrav flux_bords2( 5, nb_front_Cl , nb_comp) ;
  flux_bords2=0;
  for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl_poly_->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      for (face=ndeb; face<nfin; face++)
        {
          for(k=0; k<nb_comp; k++)
            {
              flux_bords2(0,num_cl,k)+=tab_flux_bords(face, k);
              if (ma_zone.Bords_a_imprimer_sum().contient(frontiere_dis.le_nom()))
                flux_bords2(3,num_cl,k)+=tab_flux_bords(face, k);
            }  /* fin for k */
          if (impr_mom)
            {
              if (dimension==2)
                {
                  flux_bords2(4,num_cl,0)+=tab_flux_bords(face,1)*xgr(face,0)-tab_flux_bords(face,0)*xgr(face,1);
                }
              else
                {
                  flux_bords2(4,num_cl,0)+=tab_flux_bords(face,2)*xgr(face,1)-tab_flux_bords(face,1)*xgr(face,2);
                  flux_bords2(4,num_cl,1)+=tab_flux_bords(face,0)*xgr(face,2)-tab_flux_bords(face,2)*xgr(face,0);
                  flux_bords2(4,num_cl,2)+=tab_flux_bords(face,1)*xgr(face,0)-tab_flux_bords(face,0)*xgr(face,1);
                }
            }
        } /* fin for face */
    }
  mp_sum_for_each_item(flux_bords2);

  if (je_suis_maitre() && nb_comp > 0)
    {
      //SFichier Flux;
      if (!Flux.is_open()) ouvrir_fichier(Flux,"",1);
      //SFichier Flux_moment;
      if (!Flux_moment.is_open()) ouvrir_fichier(Flux_moment,"moment",impr_mom);
      //SFichier Flux_sum;
      if (!Flux_sum.is_open()) ouvrir_fichier(Flux_sum,"sum",impr_sum);
      Flux.add_col(sch.temps_courant());
      if (impr_mom) Flux_moment.add_col(sch.temps_courant());
      if (impr_sum) Flux_sum.add_col(sch.temps_courant());
      for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
        {
          for(k=0; k<nb_comp; k++)
            {
              Flux.add_col(flux_bords2(0,num_cl,k));
              if (impr_sum) Flux_sum.add_col(flux_bords2(3,num_cl,k));
              bilan(k)+=flux_bords2(0,num_cl,k);
            }
          if (dimension==3)
            {
              for (k=0; k<nb_comp; k++)
                if (impr_mom) Flux_moment.add_col(flux_bords2(4,num_cl,k));
            }
          else
            {
              if (impr_mom) Flux_moment.add_col(flux_bords2(4,num_cl,0));
            }
        } /* fin for num_cl */
      for(k=0; k<nb_comp; k++)
        Flux.add_col(bilan(k));
      Flux << finl;
      if (impr_sum) Flux_sum << finl;
      if (impr_mom) Flux_moment << finl;
    }
  const LIST(Nom)& Liste_Bords_a_imprimer = la_zone_poly_->zone().Bords_a_imprimer();
  if (!Liste_Bords_a_imprimer.est_vide() && nb_comp > 0)
    {
      EcrFicPartage Flux_face;
      ouvrir_fichier_partage(Flux_face,"",impr_bord);
      for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
        {
          const Frontiere_dis_base& la_fr = la_zcl_poly_->les_conditions_limites(num_cl).frontiere_dis();
          const Cond_lim& la_cl = la_zcl_poly_->les_conditions_limites(num_cl);
          const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = frontiere_dis.num_premiere_face();
          int nfin = ndeb + frontiere_dis.nb_faces();
          if (ma_zone.Bords_a_imprimer().contient(la_fr.le_nom()))
            {
              if(je_suis_maitre())
                {
                  Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps ";
                  sch.imprimer_temps_courant(Flux_face);
                  Flux_face << " : " << finl;
                }
              for (face=ndeb; face<nfin; face++)
                {
                  if (dimension == 2)
                    Flux_face << "# Face a x= " << la_zone_poly_->xv(face,0) << " y= " << la_zone_poly_->xv(face,1) << " : ";
                  else if (dimension == 3)
                    Flux_face << "# Face a x= " << la_zone_poly_->xv(face,0) << " y= " << la_zone_poly_->xv(face,1) << " z= " << la_zone_poly_->xv(face,2) << " : ";
                  for(k=0; k<nb_comp; k++)
                    Flux_face << tab_flux_bords(face, k) << " ";
                  Flux_face << finl;
                }
              Flux_face.syncfile();
            }
        }
    }
  return 1;
}
/*
void Op_Diff_CoviMAC_base::associer_zone_cl_dis(const Zone_Cl_dis_base& zcl)
{
  la_zcl_poly_ = ref_cast(Zone_Cl_CoviMAC,zcl);
}
*/
void Op_Diff_CoviMAC_base::associer(const Zone_dis& zone_dis, const Zone_Cl_dis& zcl,const Champ_Inc& )
{
  la_zone_poly_ = ref_cast(Zone_CoviMAC,zone_dis.valeur());
  la_zcl_poly_ = ref_cast(Zone_Cl_CoviMAC,zcl.valeur());
}

// Description:
// calcule la contribution de la diffusion, la range dans resu
// renvoie resu
DoubleTab& Op_Diff_CoviMAC_base::calculer(const DoubleTab& inco,  DoubleTab& resu) const
{
  resu =0;
  return ajouter(inco, resu);
}


void Op_Diff_CoviMAC_base::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = diffu;
}

const Champ_base& Op_Diff_CoviMAC_base::diffusivite() const
{
  return diffusivite_.valeur();
}

void Op_Diff_CoviMAC_base::update_nu() const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const DoubleTab& nu_src = diffusivite().valeurs();
  int e, i, m, n, N = equation().inconnue().valeurs().line_size(), N_nu = nu_.line_size(), N_nu_src = nu_src.line_size(), mult = N_nu / N, c_nu = nu_src.dimension_tot(0) == 1, d, db, D = dimension;
  assert(N_nu % N == 0);

  /* nu_ : si necessaire, on doit etendre la champ source */
  if (N_nu == N_nu_src) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N_nu; n++) nu_.addr()[N_nu * e +  n] = nu_src(!c_nu * e, n); //facile
  else if (N_nu == N * D && N_nu_src == N) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) for (d = 0; d < D; d++) //diagonal
          nu_(e, n, d) = nu_src(!c_nu * e, n);
  else if (N_nu == N * D * D && (N_nu_src == N || N_nu_src == N * D)) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) //complet
        for (d = 0; d < D; d++) for (db = 0; db < D; db++) nu_(e, n, d, db) = (d == db) * nu_src(!c_nu * e, N_nu_src == N ? n : D * n + d);
  else abort();

  /* ponderation de nu par la porosite et par alpha (si pb_Multiphase) */
  const DoubleTab *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL;
  for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0, i = 0; n < N; n++) for (m = 0; m < mult; m++, i++)
        nu_.addr()[N_nu * e + i] *= zone.porosite_elem()(e) * (alp ? std::max((*alp)(e, n), 1e-8) : 1);

  /* modification par une classe fille */
  modifier_nu(nu_);

  nu_a_jour_ = 1;
}

void Op_Diff_CoviMAC_base::update_phif(int full_stencil) const
{
  if (!full_stencil && phif_a_jour_) return; //deja fait, sauf si on demande tout le stencil
  const Champ_Inc_base& ch = equation().inconnue().valeur();
  const IntTab& fcl = sub_type(Champ_Face_CoviMAC, ch) ? ref_cast(Champ_Face_CoviMAC, ch).fcl() : ref_cast(Champ_P0_CoviMAC, ch).fcl();
  la_zone_poly_->fgrad(ch.valeurs().line_size(), 0, la_zcl_poly_->les_conditions_limites(), fcl, &nu(), &som_ext, sub_type(Champ_Face_CoviMAC, ch), full_stencil, phif_d, phif_e, phif_c);
  phif_a_jour_ = 1;
}
