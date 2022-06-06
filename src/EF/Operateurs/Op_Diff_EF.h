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


#ifndef Op_Diff_EF_included
#define Op_Diff_EF_included

#include <Op_Diff_EF_base.h>
#include <Ref_Zone_Cl_EF.h>
#include <Matrice_Morse.h>
#include <Ref_Zone_EF.h>
#include <Op_EF_base.h>
#include <Champ_Don.h>

// .DESCRIPTION class Op_Diff_EF
//  Cette classe represente l'operateur de diffusion
//  La discretisation est EF
//  Le champ diffuse est scalaire
//  Le champ de diffusivite est uniforme
class Op_Diff_EF : public Op_Diff_EF_base
{
  Declare_instanciable(Op_Diff_EF);
public:
  void associer_diffusivite(const Champ_base& ) override;
  void completer() override;
  const Champ_base& diffusivite() const override;

  DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const override;
  DoubleTab& ajouter_new(const DoubleTab& ,  DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  void verifier() const;
  void remplir_nu(DoubleTab&) const override;

  // Methodes pour l implicite.
  inline void dimensionner(Matrice_Morse& matrice) const override { Op_EF_base::dimensionner(la_zone_EF.valeur(), la_zcl_EF.valeur(), matrice); }
  inline void modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const override { Op_EF_base::modifier_pour_Cl(la_zone_EF.valeur(),la_zcl_EF.valeur(), matrice, secmem); }
  inline void contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const override { ajouter_contribution(inco, matrice); }

  void contribuer_au_second_membre(DoubleTab& ) const override;
  void ajouter_bords(const DoubleTab&, DoubleTab& ,int contrib_interne=1) const;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contributions_bords(Matrice_Morse& matrice ) const;
  void ajouter_contribution_new(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_cas_scalaire(const DoubleTab& inconnue, DoubleTab& resu, DoubleTab& flux_bords, DoubleTab& nu, const Zone_Cl_EF& zone_Cl_EF, const Zone_EF& zone_EF) const;
  void ajouter_cas_vectoriel(const DoubleTab& inconnue, DoubleTab& resu, DoubleTab& flux_bords, DoubleTab& nu, const Zone_Cl_EF& zone_Cl_EF, const Zone_EF& zone_EF, int nb_comp) const;

protected :
  int transpose_;   // vaurt zero si on ne veut pas calculer grad u transpose
  int transpose_partout_ ; // vaut 1 si on veut calculer grad_u_transpose meme au bord
  int nouvelle_expression_;
  REF(Champ_base) diffusivite_;

  DoubleTab& ajouter_scalaire_dim3_nbn_8(const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_scalaire_dim2_nbn_4(const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_scalaire_gen(const DoubleTab&, DoubleTab&) const;

  template<AJOUTE_SCAL _T_>
  DoubleTab& ajouter_scalaire_template(const DoubleTab&, DoubleTab&) const;

  DoubleTab& ajouter_vectoriel_dim3_nbn_8(const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_vectoriel_dim2_nbn_4(const DoubleTab&, DoubleTab&) const;
  DoubleTab& ajouter_vectoriel_gen(const DoubleTab&, DoubleTab&) const;

  template<AJOUTE_VECT _T_>
  DoubleTab& ajouter_vectoriel_template(const DoubleTab&, DoubleTab&) const;
};

class Op_Diff_option_EF : public Op_Diff_EF
{
  Declare_instanciable(Op_Diff_option_EF);
};

template<AJOUTE_SCAL _T_>
DoubleTab& Op_Diff_EF::ajouter_scalaire_template(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  static constexpr bool IS_GEN = (_T_ == AJOUTE_SCAL::GEN), IS_D3_8 = (_T_ == AJOUTE_SCAL::D3_8), IS_D2_4 = (_T_ == AJOUTE_SCAL::D2_4);

  const Zone_EF& zone_ef = ref_cast(Zone_EF, equation().zone_dis().valeur());

  const int N = IS_GEN ? resu.line_size() : 1;
  const int nb_som_elem = IS_D3_8 ? 8 : ( IS_D2_4 ? 4 : zone_ef.zone().nb_som_elem() /* IS_GEN */);
  const int const_dimension = IS_D3_8 ? 3 : ( IS_D2_4 ? 2 : Objet_U::dimension /* IS_GEN */);
  const int dim_fois_nbn = nb_som_elem * const_dimension;

  const DoubleVect& volumes_thilde = zone_ef.volumes_thilde(), &volumes = zone_ef.volumes();

  const DoubleTab& bij = zone_ef.Bij();
  int nb_elem_tot = zone_ef.zone().nb_elem_tot();
  const IntTab& elems = zone_ef.zone().les_elems();

  const double *bij_ptr = bij.addr();
  const double *inco_ptr = tab_inconnue.addr();

#define bij_(elem,i,j) bij_ptr[elem*dim_fois_nbn+i*const_dimension+j]
#define inconnue_(som,a) inco_ptr[som * N + a]
#define resu_(som,a) resu_ptr[som * N + a]

  ArrOfDouble pr(N);
  for (int elem = 0; elem < nb_elem_tot; elem++)
    if (elem_contribue(elem))
      {
        double pond = volumes_thilde(elem) / volumes(elem) / volumes(elem) * nu_(elem);

        for (int i1 = 0; i1 < nb_som_elem; i1++)
          {
            int glob = elems(elem, i1);
            for (int yy = 0; yy < N; yy++)
              pr[yy] = 0;
            for (int i2 = 0; i2 < nb_som_elem; i2++)
              {
                int glob2 = elems(elem, i2);

                {
                  double prod = 0;
                  for (int b = 0; b < const_dimension; b++)
                    {
                      prod += bij_(elem,i1,b)*bij_(elem,i2,b);
                    }
                  for (int n = 0; n < N; n++) pr[n] += prod * inconnue_(glob2, n);
                }
              }
            resu(glob) -= pr[0] * pond;
          }
      }

  // on ajoute la contribution des bords
  ajouter_bords(tab_inconnue, resu);
  return resu;

#undef bij_
#undef inconnue_
#undef resu_
}

template<AJOUTE_VECT _T_>
DoubleTab& Op_Diff_EF::ajouter_vectoriel_template(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  static constexpr bool IS_GEN = (_T_ == AJOUTE_VECT::GEN), IS_D3_8 = (_T_ == AJOUTE_VECT::D3_8), IS_D2_4 = (_T_ == AJOUTE_VECT::D2_4);

  const Zone_EF& zone_ef = ref_cast(Zone_EF, equation().zone_dis().valeur());

  const int N = IS_D3_8 ? 3 : (IS_D2_4 ? 2 : resu.line_size() /* IS_GEN */);
  const int const_dimension = IS_GEN ? Objet_U::dimension : N;
  const int nb_som_elem = IS_D3_8 ? 8 : (IS_D2_4 ? 4 : zone_ef.zone().nb_som_elem() /* IS_GEN */);
  const int dim_fois_nbn = nb_som_elem * const_dimension;

  ArrOfInt marqueur_neuman;
  remplir_marqueur_sommet_neumann(marqueur_neuman, zone_ef, la_zcl_EF.valeur(), transpose_partout_);

  const DoubleVect& volumes_thilde = zone_ef.volumes_thilde(), &volumes = zone_ef.volumes();

  const DoubleTab& bij = zone_ef.Bij();
  int nb_elem_tot = zone_ef.zone().nb_elem_tot();
  const IntTab& elems = zone_ef.zone().les_elems();

  const double *bij_ptr = bij.addr();
  const double *inco_ptr = tab_inconnue.addr();
  double *resu_ptr = resu.addr();

#define bij_(elem,i,j) bij_ptr[elem*dim_fois_nbn+i*const_dimension+j]
#define inconnue_(som,a) inco_ptr[som * N + a]
#define resu_(som,a) resu_ptr[som * N + a]

  ArrOfDouble pr(N);

  for (int elem = 0; elem < nb_elem_tot; elem++)
    if (elem_contribue(elem))
      {
        double pond = volumes_thilde(elem) / volumes(elem) / volumes(elem) * nu_(elem);

        for (int i1 = 0; i1 < nb_som_elem; i1++)
          {
            int glob = elems(elem, i1);
            for (int yy = 0; yy < N; yy++)
              pr[yy] = 0;
            int transpose = (marqueur_neuman[glob] == 1 || N == 1) ? 0 : transpose_;
            for (int i2 = 0; i2 < nb_som_elem; i2++)
              {
                int glob2 = elems(elem, i2);

                {
                  double prod = 0;
                  double prod2 = 0;
                  for (int b = 0; b < const_dimension; b++)
                    {
                      prod += bij_(elem,i1,b)*bij_(elem,i2,b);
                      if (transpose)
                        prod2+=bij_(elem,i1,b)*inconnue_(glob2,b);
                    }
                  for (int n = 0; n < N; n++) pr[n] += prod * inconnue_(glob2, n) + prod2 * bij_(elem, i2, n);
                }
              }
            for (int n = 0; n < N; n++) resu_(glob, n)-= pr[n] * pond;
          }
      }

  // on ajoute la contribution des bords
  ajouter_bords(tab_inconnue, resu);
  return resu;

#undef bij_
#undef inconnue_
#undef resu_
}

#endif
