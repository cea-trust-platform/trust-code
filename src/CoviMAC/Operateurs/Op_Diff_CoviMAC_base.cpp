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
Implemente_ref(Op_Diff_CoviMAC_base);

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
  if (!nu_constant_) nu_a_jour_ = 0, xwh_a_jour_ = 0, phif_a_jour_ = 0;
  t_last_maj_ = t;
}


void Op_Diff_CoviMAC_base::completer()
{
  Operateur_base::completer();
  const Equation_base& eq = equation();
  int N = eq.inconnue().valeurs().line_size(), D = dimension, N_nu = max(N * dimension_min_nu(), diffusivite().valeurs().line_size());
  if (N_nu == N) nu_.resize(0, N); //isotrope
  else if (N_nu == N * D) nu_.resize(0, N, D); //diagonal
  else if (N_nu == N * D * D) nu_.resize(0, N, D, D); //complet
  else Process::exit(Nom("Op_Diff_CoviMAC_base : diffusivity component count ") + Nom(N_nu) + "not among (" + Nom(N) + ", " + Nom(N * D) + ", " + Nom(N * D * D)  + ")!");
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  zone.zone().creer_tableau_elements(nu_);
  invh_.resize(0, N), zone.creer_tableau_faces_bord(invh_);
  xh_.resize(zone.nb_faces_tot(), N, dimension); //position des points harmoniques, poids de l'amont
  wh_.resize(zone.nb_faces_tot(), N);

  int f_max = sub_type(Op_Diff_CoviMAC_Face, *this) ? zone.nb_faces_tot() : zone.nb_faces(); //en Op_.._Face, on doit aussi faire les faces virtuelles
  phif_w.resize(f_max, N); //tableaux phif_* statiques

  nu_constant_ = (sub_type(Champ_Uniforme, diffusivite()) || sub_type(Champ_Don_Fonc_xyz, diffusivite())) && !has_diffusivite_turbulente();
}

void Op_Diff_CoviMAC_base::init_op_ext() const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  int i, j, fb, M, N = equation().inconnue().valeurs().line_size(), n_mono, idx;

  op_ext = { this };//le premier op_ext est l'operateur local
  pe_ext.resize(0, 3), zone.creer_tableau_faces_bord(pe_ext);
  for (pe_ext = -1, i = 0, n_mono = 0, M = N; i < cls.size(); i++) if (sub_type(Echange_contact_CoviMAC, cls[i].valeur())) //on ajoute ceux des Echange_contact
      {
        const Echange_contact_CoviMAC& cl = ref_cast(Echange_contact_CoviMAC, cls[i].valeur());
        const Op_Diff_CoviMAC_base *o_diff = &cl.o_diff.valeur();
        if (std::find(op_ext.begin(), op_ext.end(), o_diff) == op_ext.end()) op_ext.push_back(o_diff);
        M = max(M, o_diff->equation().inconnue().valeurs().line_size()); //M -> nb de composantes maximal
        const Front_VF& fvf = ref_cast(Front_VF, cls[i].frontiere_dis());
        const IntTab& fe_dist = cl.fe_dist();
        for (j = 0, idx = std::find(op_ext.begin(), op_ext.end(), o_diff) - op_ext.begin(); j < fvf.nb_faces_tot(); j++, n_mono++)
          fb = zone.fbord(fvf.num_face(j)), pe_ext(fb, 0) = idx, pe_ext(fb, 1) = fe_dist(j, 1), pe_ext(fb, 2) = n_mono;
      }
  whm_.resize(n_mono, N, M, 2);
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

void Op_Diff_CoviMAC_base::update_nu_invh() const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const DoubleTab& nu_src = diffusivite().valeurs(), &nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();
  int e, i, j, k, f, fb, n, N = equation().inconnue().valeurs().line_size(), N_nu = nu_.line_size(), N_nu_src = nu_src.line_size(),
                            c_nu = nu_src.dimension_tot(0) == 1, d, db, D = dimension, nf_tot = zone.nb_faces_tot();
  assert(N_nu % N == 0);

  /* nu_ : si necessaire, on doit etendre la champ source */
  if (N_nu == N_nu_src) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N_nu; n++) nu_.addr()[N_nu * e +  n] = nu_src(!c_nu * e, n); //facile
  else if (N_nu == N * D && N_nu_src == N) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) for (d = 0; d < D; d++) //diagonal
          nu_(e, n, d) = nu_src(!c_nu * e, n);
  else if (N_nu == N * D * D && (N_nu_src == N || N_nu_src == N * D)) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) //complet
        for (d = 0; d < D; d++) for (db = 0; db < D; db++) nu_(e, n, d, db) = (d == db) * nu_src(!c_nu * e, N_nu_src == N ? n : D * n + d);
  else abort();

  if (has_diffusivite_turbulente()) /* prise en compte de la turbulence */
    {
      const DoubleTab& nut_src = diffusivite_turbulente().valeurs();
      int c_nut = (nut_src.dimension_tot(0) == 1), N_nut_src = nut_src.line_size();
      if (N_nu == N_nut_src) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N_nu; n++) nu_.addr()[N_nu * e + n] += nut_src(!c_nut * e, n); //facile
      else if (N_nu == N * D && N_nut_src == N) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) for (d = 0; d < D; d++) //diagonal
              nu_(e, n, d) += nut_src(!c_nut * e, n);
      else if (N_nu == N * D * D && (N_nut_src == N || N_nut_src == N * D)) for (e = 0; e < zone.nb_elem_tot(); e++) for (n = 0; n < N; n++) //complet
            for (d = 0; d < D; d++) for (db = 0; db < D; db++) nu_(e, n, d, db) += (d == db) * nut_src(!c_nut * e, N_nut_src == N ? n : D * n + d);
      else abort();
    }

  /* modification par une classe fille */
  modifier_nu(nu_);

  /* resistivite (1 / h) aux faces de bord */
  const RefObjU* modele_turbulence = has_diffusivite_turbulente() ? &equation().get_modele(TURBULENCE) : NULL;
  const Turbulence_paroi_scal_base *loi_par = modele_turbulence && modele_turbulence->non_nul() && sub_type(Modele_turbulence_scal_base,modele_turbulence->valeur()) ?
                                              &ref_cast(Modele_turbulence_scal_base,modele_turbulence->valeur()).loi_paroi().valeur() : NULL;

  /* si Pb_Multiphase et correlation de flux parietal -> invh par la correlation */
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  if (pbm && sub_type(Energie_Multiphase, equation()) && ref_cast(Pb_Multiphase, equation().probleme()).has_correlation("flux_parietal"))
    {
      const Flux_parietal_base& flux = ref_cast(Flux_parietal_base, ref_cast(Pb_Multiphase, equation().probleme()).get_correlation("flux_parietal").valeur());
      const Fluide_base& mil = ref_cast(Fluide_base, equation().milieu());
      const DoubleTab& temp = equation().inconnue().valeurs(), &press = pbm->eq_qdm.pression().valeurs(), &alpha = pbm->eq_masse.inconnue().valeurs(),
                       &vit = pbm->eq_qdm.inconnue().valeurs(), &lambda = mil.conductivite().valeurs(),
                        &mu = mil.viscosite_dynamique().valeurs(), &rho = mil.masse_volumique().valeurs(), &Cp = mil.capacite_calorifique().valeurs();
      int c_lambda = lambda.dimension(0) == 1, c_mu = mu.dimension(0) == 1, c_rho = rho.dimension(0) == 1, c_Cp = Cp.dimension(0) == 1;
      DoubleTrav nv(N); //normes des vitesses
      for (i = 0; i < cls.size(); i++)
        {
          const Front_VF& fvf = ref_cast(Front_VF, cls[i].frontiere_dis());
          for (j = 0; j < fvf.nb_faces_tot(); j++)
            {
              /* FIXME: ne marche que pour les flux lineaires */
              f = fvf.num_face(j), e = f_e(f, 0), fb = zone.fbord(f);
              double Dh = zone.diametre_hydraulique_elem().dimension(0) ? zone.diametre_hydraulique_elem()(e) : zone.dist_norm_bord(f);
              for (nv = 0, d = 0, k = nf_tot + D * e; d < D; d++, k++) for (n = 0; n < N; n++) nv(n) += std::pow(vit(k, n), 2);
              for (n = 0; n < N; n++) nv(n) = sqrt(nv(n));
              flux.flux_chaleur(N, Dh, Dh, &alpha(e, 0), &temp(e, 0), press(e, 0), nv.addr(), 0,
                                &lambda(!c_lambda * e, 0), &mu(!c_mu * e, 0), &rho(!c_rho * e, 0), &Cp(!c_Cp * e, 0),
                                NULL, NULL, NULL, NULL, NULL, &invh_(fb, 0)); //stocke le coefficient d'echange dans invh_
              for (n = 0; n < N; n++) invh_(fb, n) = invh_(fb, n) > 1. / max(invh_(fb, n), 1e-10); //pour eviter invh_ = +inf
            }
        }
    }
  else if (loi_par && loi_par->use_equivalent_distance()) for (i = 0; i < cls.size(); i++) /* si loi de paroi : invh par equivalent_distance */
      {
        const Front_VF& fvf = ref_cast(Front_VF, cls[i].frontiere_dis());
        for (j = 0; j < fvf.nb_faces_tot(); j++) for (f = fvf.num_face(j), fb = zone.fbord(f), n = 0; n < N; n++)
            invh_(fb, n) = (loi_par->equivalent_distance(i, j) - zone.dist_norm_bord(f)) * fs(f) * fs(f) / zone.nu_dot(&nu_, f_e(f, 0), n, &nf(f, 0), &nf(f, 0));
      }
  else invh_ = 0;

  nu_a_jour_ = 1;
}

void Op_Diff_CoviMAC_base::update_xwh() const
{
  if (!op_ext_init_) init_op_ext(), op_ext_init_ = 1;
  la_zone_poly_->harmonic_points(la_zcl_poly_->les_conditions_limites(), 0, 1, &nu(), &invh(), xh_, wh_, &whm_);
  xwh_a_jour_ = 1;
}

void Op_Diff_CoviMAC_base::update_phif(int full_stencil) const
{
  if (phif_a_jour_) return; //deja fait
  const Champ_Inc_base& ch = equation().inconnue().valeur();
  const IntTab& fcl = sub_type(Champ_Face_CoviMAC, ch) ? ref_cast(Champ_Face_CoviMAC, ch).fcl() : ref_cast(Champ_P0_CoviMAC, ch).fcl();
  la_zone_poly_->fgrad(la_zcl_poly_->les_conditions_limites(), fcl, &nu(), &invh(), xh(), wh(), &whm(),
                       &pe_ext, 1, full_stencil, phif_w, phif_d, phif_e, phif_c, &phif_pe, &phif_pc);
  phif_a_jour_ = 1;
}
