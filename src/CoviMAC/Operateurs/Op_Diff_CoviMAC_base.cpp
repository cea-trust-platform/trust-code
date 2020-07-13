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
  if (equation().que_suis_je() == "Transport_K_Eps") nu_.resize(0, 2);
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  zone.zone().creer_tableau_elements(nu_);
  zone.creer_tableau_faces(nu_fac_);

  /* interpolations de nu.grad T : on prend les tailles maximales possibles */
  zone.init_feb();
  phif_d.resize(zone.nb_faces() + 1), phif_j.resize(zone.feb_d(zone.nb_faces()));
  phif_c.resize(zone.feb_d(zone.nb_faces()), equation().inconnue().valeurs().line_size());

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
      SFichier Flux;
      ouvrir_fichier(Flux,"",1);
      SFichier Flux_moment;
      ouvrir_fichier(Flux_moment,"moment",impr_mom);
      SFichier Flux_sum;
      ouvrir_fichier(Flux_sum,"sum",impr_sum);
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
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  int i, j, f;

  /* 1. nu_ */
  //dimensionnement
  const DoubleTab& diffu=diffusivite().valeurs();
  if (equation().que_suis_je() != "Transport_K_Eps")
    {
      if (!nu_.get_md_vector().non_nul())
        {
          if (diffu.nb_dim() > 1 && diffu.dimension(1) > 1)
            nu_.resize(0, diffu.dimension(1));
          zone.zone().creer_tableau_elements(nu_, Array_base::NOCOPY_NOINIT);
        }
      if (!diffu.get_md_vector().non_nul())
        {
          // diffusvite uniforme
          int n = nu_.dimension_tot(0), nb_comp = nu_.line_size();
          // Tableaux vus comme uni-dimenionnels:
          const DoubleVect& arr_diffu = diffu;
          DoubleVect& arr_nu = nu_;
          for (i = 0; i < n; i++)
            for (j = 0; j < nb_comp; j++)
              arr_nu[i*nb_comp + j] = arr_diffu[j];
        }
      else
        {
          assert(nu_.get_md_vector() == diffu.get_md_vector());
          assert_espace_virtuel_vect(diffu);
          nu_.inject_array(diffu);
        }
    }

  /* ajout de la diffusivite turbulente si elle existe */
  if (has_diffusivite_turbulente())
    {
      const DoubleTab& diffu_turb = diffusivite_turbulente().valeurs();
      if (equation().que_suis_je() == "Transport_K_Eps")
        {
          bool nu_uniforme = sub_type(Champ_Uniforme, diffusivite());
          double val_nu = diffu(0, 0);
          for (i = 0; i < diffu_turb.dimension(0); i++) for (j = 0; j < 2; j++)
              {
                if (!nu_uniforme) val_nu = diffu(i, 0);
                nu_(i, j) = val_nu + diffu_turb(i, j);
              }
        }
      else
        {
          if (!diffu_turb.get_md_vector().non_nul())
            {
              // diffusivite uniforme
              int n = nu_.dimension_tot(0), nb_comp = nu_.line_size();
              // Tableaux vus comme uni-dimenionnels:
              const DoubleVect& arr_diffu_turb = diffu_turb;
              DoubleVect& arr_nu = nu_;
              for (i = 0; i < n; i++)
                for (j = 0; j < nb_comp; j++)
                  arr_nu[i*nb_comp + j] += arr_diffu_turb[j];
            }
          else
            {
              assert(nu_.get_md_vector() == diffu_turb.get_md_vector());
              assert_espace_virtuel_vect(diffu_turb);
              nu_ += diffu_turb;
            }
        }
    }

  /* 2. nu_fac : prend en compte les lois de parois et le facteur utilisateur (nu_fac_mod) */
  // utilise-t-on des lois de paroi ?
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  int loi_par = modele_turbulence.non_nul() && sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()) &&
                ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur()).loi_paroi().valeur().use_equivalent_distance();

  for (i = 0; i <= cls.size(); i++) //boucle sur les bords, puis sur les faces internes
    {
      int deb = i < cls.size() ? ref_cast(Front_VF, cls[i].frontiere_dis()).num_premiere_face() : zone.premiere_face_int(),
          num = i < cls.size() ? ref_cast(Front_VF, cls[i].frontiere_dis()).nb_faces()          : zone.nb_faces() - zone.premiere_face_int();
      for (f = deb; f < deb + num; f++) //nu par composante a chaque face
        {
          if (i < cls.size() && loi_par) //facteur multiplicatif du a une loi de paroi
            nu_fac_(f) = zone.dist_norm_bord(f) / ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur()).loi_paroi().valeur().equivalent_distance(i, f - deb);
          else nu_fac_(f) = zone.porosite_face(f); //par defaut : facteur du a la porosite
          if (nu_fac_mod.size()) nu_fac_(f) *= nu_fac_mod(f); //prise en compte de nu_fac_mod
        }
    }
  nu_fac_.echange_espace_virtuel();
  /* heavy lifting */
  zone.flux(zone.nb_faces(), nu_, phif_d, phif_j, phif_c);
  nu_a_jour_ = 1;
}
