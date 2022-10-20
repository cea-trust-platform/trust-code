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

#include <Solveur_Masse_Elem_proto.h>
#include <Op_Diff_negligeable.h>
#include <Champ_Inc_P0_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Neumann_paroi.h>
#include <Matrix_tools.h>
#include <Milieu_base.h>
#include <Operateur.h>
#include <Zone_VF.h>

void Solveur_Masse_Elem_proto::associer_masse_proto(const Solveur_Masse_base& sm, const Zone_VF& zvf )
{
  solv_mass_ = sm;
  la_zone_ = zvf;
}

void Solveur_Masse_Elem_proto::preparer_calcul_proto()
{
  solv_mass_->equation().init_champ_conserve();
}

DoubleTab& Solveur_Masse_Elem_proto::appliquer_impl_proto(DoubleTab& sm) const
{
  const Zone_VF& zone = la_zone_.valeur();
  const DoubleVect& ve = zone.volumes(), &pe = solv_mass_->equation().milieu().porosite_elem();
  const DoubleTab& der = solv_mass_->equation().champ_conserve().derivees().at(solv_mass_->equation().inconnue().le_nom().getString());

  int e, ne_tot = zone.nb_elem_tot(), n, N = sm.line_size();
  assert(sm.dimension_tot(0) >= ne_tot && N == der.line_size());

  /* partie elem */
  for (e = 0; e < ne_tot; e++)
    for (n = 0; n < N; n++)
      if (der(e, n) > 1e-10) sm(e, n) /= pe(e) * ve(e) * der(e, n);
      else sm(e, n) = 0; //cas d'une evanescence

  return sm;
}

void Solveur_Masse_Elem_proto::dimensionner_blocs_proto(matrices_t matrices, const tabs_t& semi_impl) const
{
  solv_mass_->equation().init_champ_conserve();
  /* une diagonale par derivee de champ_conserve_ presente dans matrices */
  const Zone_VF& zone = la_zone_.valeur();
  const Champ_Inc_base& cc = solv_mass_->equation().champ_conserve();
  int e, ne = zone.nb_elem(), n, N = cc.valeurs().line_size();

  for (auto &&i_m : matrices)
    if (cc.derivees().count(i_m.first))
      {
        /* nombre de composantes de la variable : autant que le champ par defaut, mais peut etre different pour la pression */
        const DoubleTab& col = solv_mass_->equation().probleme().get_champ(i_m.first.c_str()).valeurs(); //tableau de l'inconnue par rapport a laquelle on derive
        int m, M = col.line_size();

        IntTrav stencil(0, 2);
        stencil.set_smart_resize(1);

        for (e = 0; e < ne; e++)
          for (n = 0, m = 0; n < N; n++, m += (M > 1)) stencil.append_line(N * e + n, M * e + m);
        Matrice_Morse mat;
        Matrix_tools::allocate_morse_matrix(N *  solv_mass_->equation().inconnue().valeurs().dimension_tot(0), M * col.dimension_tot(0), stencil, mat);
        i_m.second->nb_colonnes() ? *i_m.second += mat : *i_m.second = mat;
      }
}

void Solveur_Masse_Elem_proto::ajouter_blocs_proto(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  const Zone_VF& zone = la_zone_.valeur();
  const Champ_Inc_base& cc = solv_mass_->equation().champ_conserve();
  const Conds_lim& cls = solv_mass_->equation().zone_Cl_dis()->les_conditions_limites();
  const IntTab& fcl = ref_cast(Champ_Inc_P0_base, solv_mass_->equation().inconnue().valeur()).fcl(), &f_e = zone.face_voisins();
  const DoubleTab& present = cc.valeurs(), &passe = cc.passe();
  const DoubleVect& ve = zone.volumes(), &pe = solv_mass_->equation().milieu().porosite_elem(), &fs = zone.face_surfaces();
  int e, f, n, N = cc.valeurs().line_size(), ne = zone.nb_elem();

  /* second membre : avec ou sans resolution en increments*/
  for (e = 0; e < ne; e++)
    for (n = 0; n < N; n++)
      secmem(e, n) += pe(e) * ve(e) * (passe(e, n) - resoudre_en_increments * present(e, n)) / dt;

  /* si on n'a pas d'operateur de diffusion (operateur(0) negligeable ou operateur(0) convectif pour Masse_Multiphase), alors ajout des flux aux faces de Neumann */
  if ( (sub_type(Op_Diff_negligeable, solv_mass_->equation().operateur(0).l_op_base())) || (!sub_type(Operateur_Diff_base, solv_mass_->equation().operateur(0).l_op_base())) )
    for (f = 0; f < zone.premiere_face_int(); f++)
      if (fcl(f, 0) == 4)
        for (e = f_e(f, 0), n = 0; n < N; n++)
          secmem(e, n) += fs(f) * ref_cast(Neumann_paroi, cls[fcl(f, 1)].valeur()).flux_impose(fcl(f, 2), n);

  /* matrices */
  for (auto &&i_m : matrices)
    if (cc.derivees().count(i_m.first))
      {
        int m, M = solv_mass_->equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size();
        const DoubleTab& der = cc.derivees().at(i_m.first);
        for (e = 0; e < ne; e++)
          for (n = 0, m = 0; n < N; n++, m += (M > 1))
            (*i_m.second)(N * e + n, M * e + m) += pe(e) * ve(e) * der(e, n) / dt;
      }
}
