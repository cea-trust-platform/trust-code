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

#ifndef Op_Conv_EF_included
#define Op_Conv_EF_included

#include <TRUSTTabs_forward.h>
#include <Op_Conv_EF_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Op_Diff_EF.h>
#include <Motcle.h>
#include <Entree.h>
#include <Debog.h>

enum class AJOUTE_COND { GEN , D3_81 , D3_82 , D2_41 , D2_42 };

// .DESCRIPTION class Op_Conv_EF
//  Cette classe represente l'operateur de convection associe a une equation de
//  transport d'un scalaire.
//  La discretisation est EF
//  Le champ convecte est scalaire ou vecteur de type Champ_P1NC
//  Le schema de convection est du type Decentre ou Centre
class Op_Conv_EF : public Op_Conv_EF_base
{
  Declare_instanciable(Op_Conv_EF);
public:

  DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const override;
  DoubleTab& ajouter_a_la_diffusion(const DoubleTab& , DoubleTab& ) const;

  double calculer_dt_stab() const override ;

  //virtual void remplir_fluent(DoubleVect& ) const;
  // Methodes pour l implicite.
  inline void dimensionner(Matrice_Morse& matrice) const override { Op_EF_base::dimensionner(la_zone_EF.valeur(),la_zcl_EF.valeur(), matrice); }
  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override { Op_EF_base::modifier_pour_Cl(la_zone_EF.valeur(),la_zcl_EF.valeur(), matrice, secmem); }
  inline void contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const override { ajouter_contribution(inco, matrice); }
  inline void contribuer_au_second_membre(DoubleTab& resu) const override { contribue_au_second_membre(resu); }
  void contribue_au_second_membre(DoubleTab& ) const;
  void ajouter_contribution_sous_cond(const DoubleTab& transporte, Matrice_Morse& matrice,int btd_impl,int hourglass_impl,int centre_impl ) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_a_la_diffusion(const DoubleTab&, Matrice_Morse& ) const;
  void contribue_au_second_membre_a_la_diffusion(DoubleTab& resu) const;

  DoubleTab& ajouter_sous_cond(const DoubleTab& transporte, DoubleTab& resu,int btd_impl,int hourglass_impl,int centre_impl) const;
  virtual double coefficient_btd() const;
  void completer() override;
  double get_btd() const { return btd_; }
  const Champ_base& get_champ(const Motcle& nom) const override;

protected:
  double hourglass;
  int hourglass_impl_,btd_impl_,centre_impl_; // flag pour savoir si les termes sont implicites
  int hourglass_hors_conv_,btd_hors_conv_;
  double f_lu_;
  enum type_operateur { amont, muscl, centre };
  int calcul_dt_stab_;
  type_operateur type_op;

  // pour supg
  mutable double max_ue_,min_dx_;
  mutable Champ_Uniforme coefficient_correcteur_supg_;

// optimisation
  DoubleTab& ajouter_sous_cond_gen(const DoubleTab& transporte, DoubleTab& resu,int btd_impl,int hourglass_impl,int centre_impl) const;
  DoubleTab& ajouter_sous_cond_dim3_nbn8_nbdim1(const DoubleTab& transporte, DoubleTab& resu,int btd_impl,int hourglass_impl,int centre_impl) const;
  DoubleTab& ajouter_sous_cond_dim3_nbn8_nbdim2(const DoubleTab& transporte, DoubleTab& resu,int btd_impl,int hourglass_impl,int centre_impl) const;
  DoubleTab& ajouter_sous_cond_dim2_nbn4_nbdim1(const DoubleTab& transporte, DoubleTab& resu,int btd_impl,int hourglass_impl,int centre_impl) const;
  DoubleTab& ajouter_sous_cond_dim2_nbn4_nbdim2(const DoubleTab& transporte, DoubleTab& resu,int btd_impl,int hourglass_impl,int centre_impl) const;
  double btd_;

  template <AJOUTE_COND _COND_>
  DoubleTab& ajouter_sous_cond_template(const DoubleTab& transporte, DoubleTab& resu,int btd_impl,int hourglass_impl,int centre_impl) const;
};

class Op_Conv_BTD_EF : public Op_Conv_EF
{
  Declare_instanciable(Op_Conv_BTD_EF);
  double coefficient_btd() const override;
  void completer() override;
protected:
  double facteur_;
};

template <AJOUTE_COND _COND_>
DoubleTab& Op_Conv_EF::ajouter_sous_cond_template(const DoubleTab& transporte, DoubleTab& resu,int btd_impl,int hourglass_impl,int centre_impl) const
{
  static constexpr bool IS_GEN = (_COND_ == AJOUTE_COND::GEN), IS_D3_81 = (_COND_ == AJOUTE_COND::D3_81), IS_D3_82 = (_COND_ == AJOUTE_COND::D3_82),
                        IS_D2_41 = (_COND_ == AJOUTE_COND::D2_41), IS_D2_42 = (_COND_ == AJOUTE_COND::D2_42);

  const Zone_EF& zone_ef = ref_cast(Zone_EF, la_zone_EF.valeur());
  const int nb_som_elem = (IS_D3_81 || IS_D3_82) ? 8 : ( (IS_D2_41 || IS_D2_42) ? 4 : zone_ef.zone().nb_som_elem() /* IS_GEN */);

  if ((btd_impl == 1) && (hourglass_impl == 1) && (centre_impl == 1))
    return resu;

  DoubleVect& fluent_ = fluent;

  const Champ_Inc_base& la_vitesse = vitesse_.valeur();
  const DoubleTab& G = la_vitesse.valeurs();

  int transport_rhou = 0;
  if (vitesse_.le_nom() == "rho_u") transport_rhou = 1;

  const DoubleTab& rho_elem = (transport_rhou == 1 ? equation().probleme().get_champ("masse_volumique_melange").valeurs() : equation().probleme().get_champ("masse_volumique").valeurs());
  int is_not_rho_unif = (rho_elem.size() == 1 ? 0 : 1);

  Debog::verifier("conv vitesse", G);
  Debog::verifier("conv rho", rho_elem);
  Debog::verifier("conv transporte", transporte);

  const int nb_comp0 = resu.line_size(), nb_elem_tot = zone_ef.zone().nb_elem_tot();
  const DoubleVect& volumes_thilde = zone_ef.volumes_thilde(), & volumes = zone_ef.volumes();
  const DoubleTab& IPhi_thilde = zone_ef.IPhi_thilde(), & bij = zone_ef.Bij();

  const IntTab& elems = zone_ef.zone().les_elems();
  double f = coefficient_btd();

  int mcoef3d[8] = { 1, -1, -1, 1, -1, 1, 1, -1 };
  int sommetoppose[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };

  DoubleTab transp_loc(nb_som_elem, nb_comp0);
  // A DEPLACER !!!!!
  const DoubleTab& lambda = ref_cast(Operateur_Diff_base,equation().operateur(0).l_op_base()).diffusivite().valeurs();
  int is_not_lambda_unif = 1;
  if (lambda.size() == 1)
    is_not_lambda_unif = 0;

  const int const_dimension = (IS_D3_81 || IS_D3_82) ? 3 : ( (IS_D2_41 || IS_D2_42) ? 2 : Objet_U::dimension /* IS_GEN */);
  const int nb_comp = IS_D3_82 ? 3 : ((IS_D3_81 || IS_D2_41) ? 1 : ( IS_D2_42 ? 2 : nb_comp0 /* IS_GEN */));
  const int dim_fois_nbn = (IS_D3_81 || IS_D3_82) ? 24 : ( (IS_D2_41 || IS_D2_42) ? 8 : -100 /* IS_GEN, inutile */);

  if (nb_comp0 != nb_comp)
    abort();

  ArrOfDouble G_e(const_dimension);
  ArrOfDouble pr(nb_comp),ge_bij(nb_som_elem);

  const double *bij_ptr = bij.addr();
  const double *transp_loc_ptr = transp_loc.addr();
  const double *transporte_ptr = transporte.addr();
  double *resu_ptr = resu.addr();

#define bij_(elem,i,j) (IS_GEN ? bij(elem,i,j) : bij_ptr[elem*dim_fois_nbn+i*const_dimension+j])
#define transp_loc_(som,a) (IS_GEN ? transp_loc(som,a) :transp_loc_ptr[som*nb_comp+a])
#define transporte_(som,a) (IS_GEN ? transporte(som,a) : transporte_ptr[som*nb_comp+a])
#define resu_(som,a) (IS_GEN ? resu(som,a) : resu_ptr[som*nb_comp+a])

  double inv_nb_som_elem = 1. / nb_som_elem;
  for (int elem = 0; elem < nb_elem_tot; elem++)
    if (elem_contribue(elem))
      {
        G_e = 0;
        for (int i1 = 0; i1 < nb_som_elem; i1++)
          {
            int glob = elems(elem, i1);
            for (int b = 0; b < const_dimension; b++)
              G_e[b] += G(glob, b);
            for (int a = 0; a < nb_comp; a++)
              transp_loc(i1, a) = transporte_(glob, a);
          }
        G_e *= inv_nb_som_elem;

        double vol_elem = volumes(elem);
        double inv_vol_elem = 1. / vol_elem;
        double pond2 = volumes_thilde(elem) * inv_vol_elem * inv_vol_elem;

        if (transport_rhou)
          pond2 /= (is_not_rho_unif ? rho_elem(elem) : rho_elem(0, 0));
        double fpond2 = f * pond2;

        if ((hourglass) && (nb_som_elem == 8) && (hourglass_impl == 0))
          {
            double pond3 = f * dotproduct_array(G_e, G_e);
            if (transport_rhou)
              pond3 /= (is_not_rho_unif ? rho_elem(elem) : rho_elem(0, 0));
            if (is_not_lambda_unif)
              pond3 += lambda(elem);
            else
              pond3 += lambda(0, 0);
            pond3 *= volumes_thilde(elem) * inv_vol_elem * pow(vol_elem, 0.3333333333333333);
            pond3 *= hourglass;

            for (int a = 0; a < nb_comp; a++)
              {
                double coef2d = 0.042 * pond3;
                double coef3d = coef2d * 0.5;

                double t0 = -coef2d * (transp_loc_(0,a)+transp_loc_(1,a)+transp_loc_(2,a)+transp_loc_(3,a)
                                       +transp_loc_(4,a)+transp_loc_(5,a)+transp_loc_(6,a)+transp_loc_(7,a));

                double t3d = 0;
                for (int i = 0; i < 8; i++)
                  t3d += mcoef3d[i] * transp_loc_(i, a);
                t3d *= coef3d;
                double t3db = coef3d * (transp_loc_(0,a)-transp_loc_(1,a)+transp_loc_(3,a)-transp_loc_(2,a)
                                        -transp_loc_(4,a)+transp_loc_(5,a)-transp_loc_(7,a)+transp_loc_(6,a));
                if (!est_egal(t3d, t3db, 1e-6))
                  assert(0);

                for (int i = 0; i < 8; i++)
                  resu_(elems(elem,i),a)-=mcoef3d[i]*t3d+t0+coef2d*4.*(transp_loc_(i,a)+transp_loc_(sommetoppose[i],a));
              }
          }

        {
          for (int yy = 0; yy < nb_som_elem; yy++)
            ge_bij[yy] = 0;
          if ((centre_impl == 0) || (btd_impl == 0))
            {
              for (int i1 = 0; i1 < nb_som_elem; i1++)
                {
                  double cb = 0;
                  for (int b = 0; b < const_dimension; b++)
                    cb += G_e[b] * bij_(elem, i1, b);
                  ge_bij[i1] = cb;
                }
              for (int i1 = 0; i1 < nb_som_elem; i1++)
                {
                  double pond = 0;
                  if (centre_impl == 0)
                    pond = IPhi_thilde(elem, i1) * inv_vol_elem;
                  int glob = elems(elem, i1);
                  //pr=0;
                  for (int yy = 0; yy < nb_comp; yy++)
                    pr[yy] = 0;

                  double cbbtd = pond;
                  if ((btd_impl == 0) && (type_op == amont))
                    cbbtd += ge_bij[i1] * (fpond2);
                  for (int i2 = 0; i2 < nb_som_elem; i2++)
                    {
                      const double cbi2 = ge_bij[i2];
                      double coef = cbbtd * cbi2;
                      for (int a = 0; a < nb_comp; a++)
                        pr[a] -= coef * transp_loc_(i2, a);
                    }
                  for (int a = 0; a < nb_comp; a++)
                    resu_(glob,a)+=pr[a];
                }
            }
        }
      }

  fluent_.echange_espace_virtuel();
  return resu;

#undef bij_
#undef transp_loc_
#undef transporte_
#undef resu_
}

#endif
