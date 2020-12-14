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
  if (!nu_constant_) nu_a_jour_ = 0;
}


void Op_Diff_CoviMAC_base::completer()
{
  Operateur_base::completer();
  const Equation_base& eq = equation();
  int N = eq.inconnue().valeurs().line_size(), N_nu = max(N * dimension_min_nu(), diffusivite().valeurs().line_size());
  nu_.resize(0, N_nu), nu_bord_.resize(0, N_nu);
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  zone.zone().creer_tableau_elements(nu_), zone.creer_tableau_faces_bord(nu_bord_);

  /* interpolations de nu.grad T : on prend les tailles maximales possibles */
  zone.init_feb();
  phif_d.resize(zone.nb_faces_tot() + 1), phif_j.resize(zone.feb_d(zone.nb_faces_tot()));
  phif_c.resize(zone.feb_d(zone.nb_faces_tot()), eq.inconnue().valeurs().line_size());
  phif_xb.resize(zone.nb_faces_tot(), eq.inconnue().valeurs().line_size(), dimension);

  nu_constant_ = (sub_type(Champ_Uniforme, diffusivite()) || sub_type(Champ_Don_Fonc_xyz, diffusivite())) && !has_diffusivite_turbulente();
  nu_a_jour_ = 0;
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
  if (nu_a_jour_) return; //deja fait
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const DoubleTab& nu_src = diffusivite().valeurs();
  int e, i, j, f, n, nb, N = equation().inconnue().valeurs().line_size(), N_nu = nu_.line_size(), N_nu_src = nu_src.line_size(),
                         c_nu = nu_src.dimension_tot(0) == 1, d, db, D = dimension, mult = N_nu / N;
  assert(N_nu % N == 0);

  /* nu_ : si necessaire, on doit etendre la champ source */
  if (N_nu == N_nu_src) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N_nu; n++) nu_(e, n) = nu_src(!c_nu * e, n); //facile
  else if (N_nu == N * D && N_nu_src == N) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) for (d = 0; d < D; d++) //diagonal
          nu_(e, D * n + d) = nu_src(!c_nu * e, n);
  else if (N_nu == N * D * D && (N_nu_src == N || N_nu_src == N * D)) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) //complet
        for (d = 0; d < D; d++) for (db = 0; db < D; db++) nu_(e, D * (D * n + d) + db) = (d == db) * nu_src(!c_nu * e, N_nu_src == N ? n : D * n + d);
  else abort();

  /* nu_bord_ : nu_ de l'element voisin */
  for (f = 0; f < zone.premiere_face_int(); f++) for (n = 0; n < N_nu; n++) nu_bord_(f, n) = nu_(f_e(f, 0), n);

  /* turbulence : ajout de la diffusivite turbulente a nu_, modification de nu_bord_ par les lois de paroi */
  if (has_diffusivite_turbulente())
    {
      const DoubleTab& nut_src = diffusivite_turbulente().valeurs();
      int c_nut = (nut_src.dimension_tot(0) == 1), N_nut_src = nut_src.line_size();
      if (N_nu == N_nut_src) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N_nu; n++) nu_(e, n) += nut_src(!c_nut * e, n); //facile
      else if (N_nu == N * D && N_nut_src == N) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) for (d = 0; d < D; d++) //diagonal
              nu_(e, D * n + d) += nut_src(!c_nut * e, N_nut_src < N_nu ? n : D * n + d);
      else if (N_nu == N * D * D && (N_nut_src == N || N_nut_src == N * D)) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) //complet
            for (d = 0; d < D; d++) for (db = 0; db < D; db++) nu_(e, D * (D * n + d) + db) += (d == db) * nut_src(!c_nut * e, N_nut_src == N ? n : D * n + d);
      else abort();

      const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
      int loi_par = modele_turbulence.non_nul() && sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()) &&
                    ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur()).loi_paroi().valeur().use_equivalent_distance();
      if (loi_par) for (i = 0; i < cls.size(); i++)
          {
            const Front_VF& fvf = ref_cast(Front_VF, cls[i].frontiere_dis());
            for (j = 0; j < fvf.nb_faces(); j++) for(f = fvf.num_face(j), n = 0; n < N_nu; n++)
                nu_bord_(f, n) *= zone.dist_norm_bord(f) / ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur()).loi_paroi().valeur().equivalent_distance(i, j);
          }
    }

  /* ponderation de nu par la porosite et par alpha (si pb_Multiphase) */
  const DoubleTab *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().valeurs() : NULL;
  for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0, i = 0; n < N; n++) for (nb = 0; nb < mult; nb++, i++)
        nu_(e, i) *= zone.porosite_elem()(e) * (alp ? max(alp->addr()[N * e + n], 1e-8) : 1);
  /* ponderation de nu_bord par alpha si pb_Multiphase */
  for (f = 0; f < zone.premiere_face_int(); f++) for (n = 0, i = 0; n < N; n++) for (nb = 0; nb < mult; nb++, i++)
        nu_bord_(f, i) *= (alp ? max(alp->addr()[N * f_e(f, 0) + n], 1e-8) : 1);

  /* modification de nu_ / nu_bord_ par une classe fille */
  modifier_nu(nu_, nu_bord_);

  /* partie virtuelle de nu_bord_ (faisable a la main, mais penible...) */
  nu_bord_.echange_espace_virtuel();

  /* heavy lifting */
  zone.fgrad(sub_type(Op_Diff_CoviMAC_Face, *this) ? zone.nb_faces_tot() : zone.nb_faces(), &nu_, &nu_bord_, 1, phif_d, phif_j, phif_c, &phif_w, &phif_xb);
  nu_a_jour_ = 1;
}
