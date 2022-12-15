/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Champ_P1NC_included
#define Champ_P1NC_included

#include <Champ_P1NC_implementation.h>
#include <Zone_Cl_VEF.h>
#include <Zone_VEF.h>
#include <Champ_Inc.h>
#include <Ok_Perio.h>

/*! @brief class Champ_P1NC
 *
 *  Rq : cette classe est specifique au module VEF
 *
 * @sa Champ_Inc_base
 */

extern void calculer_gradientP1NC(const DoubleTab&, const Zone_VEF&, const Zone_Cl_VEF&, DoubleTab&);

class Champ_P1NC: public Champ_Inc_base, public Champ_P1NC_implementation
{
  Declare_instanciable(Champ_P1NC);
public:
  int fixer_nb_valeurs_nodales(int nb_noeuds) override;
  void mettre_a_jour(double temps) override;
  void abortTimeStep() override;
  void calcul_y_plus(const Zone_Cl_VEF&, DoubleVect&) const;
  void calcul_y_plus_diphasique(const Zone_Cl_VEF& , DoubleVect&) const;
  void calcul_grad_T(const Zone_Cl_VEF&, DoubleTab&) const;
  void calcul_grad_U(const Zone_Cl_VEF&, DoubleTab&) const;
  void calcul_h_conv(const Zone_Cl_VEF&, DoubleTab&, int temp_ref) const;
  int compo_normale_sortante(int) const;
  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double, int distant) const override;
  void cal_rot_ordre1(DoubleTab&) const;
  void gradient(DoubleTab&) const;
  int imprime(Sortie&, int) const override;
  void calcul_critere_Q(DoubleVect&) const;

  virtual double norme_L2(const Domaine&) const;
  Champ_base& affecter_(const Champ_base&) override;
  void verifie_valeurs_cl() override;

  static DoubleVect& calcul_S_barre(const DoubleTab&, DoubleVect&, const Zone_Cl_VEF&);
  static DoubleTab& calcul_gradient(const DoubleTab&, DoubleTab&, const Zone_Cl_VEF&);
  static DoubleTab& calcul_duidxj_paroi(DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&, const Zone_Cl_VEF&);

  virtual double norme_H1(const Domaine&) const;
  virtual double norme_L2_H1(const Domaine& dom) const;
  static double calculer_integrale_volumique(const Zone_VEF&, const DoubleVect&, Ok_Perio ok);

  // Methodes inlines
  inline const Zone_VEF& zone_vef() const override { return ref_cast(Zone_VEF, la_zone_VF.valeur()); }
  inline DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& val, int le_poly) const override
  {
    return Champ_P1NC_implementation::valeur_a_elem(position, val, le_poly);
  }

  inline double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const override
  {
    return Champ_P1NC_implementation::valeur_a_elem_compo(position, le_poly, ncomp);
  }

  inline double valeur_a_sommet_compo(int num_som, int le_poly, int ncomp) const override
  {
    return Champ_P1NC_implementation::valeur_a_sommet_compo(num_som, le_poly, ncomp);
  }

  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& tab_valeurs) const override
  {
    return Champ_P1NC_implementation::valeur_aux_elems(positions, les_polys, tab_valeurs);
  }

  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& tab_valeurs, int ncomp) const override
  {
    return Champ_P1NC_implementation::valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
  }

  inline DoubleTab& valeur_aux_elems_smooth(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& tab_valeurs) override
  {
    return Champ_P1NC_implementation::valeur_aux_elems_smooth(positions, les_polys, tab_valeurs);
  }

  inline DoubleVect& valeur_aux_elems_compo_smooth(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& tab_valeurs, int ncomp) override
  {
    return Champ_P1NC_implementation::valeur_aux_elems_compo_smooth(positions, les_polys, tab_valeurs, ncomp);
  }

  inline DoubleTab& valeur_aux_sommets(const Domaine& dom, DoubleTab& val) const override
  {
    return Champ_P1NC_implementation::valeur_aux_sommets(dom, val);
  }

  inline DoubleVect& valeur_aux_sommets_compo(const Domaine& dom, DoubleVect& val, int comp) const override
  {
    return Champ_P1NC_implementation::valeur_aux_sommets_compo(dom, val, comp);
  }

  inline DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override
  {
    return Champ_P1NC_implementation::remplir_coord_noeuds(positions);
  }

  inline int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const override
  {
    return Champ_P1NC_implementation::remplir_coord_noeuds_et_polys(positions, polys);
  }

  inline void filtrer_L2(DoubleTab& x) const
  {
    Champ_P1NC_implementation::filtrer_L2(x);
  }

  inline void filtrer_H1(DoubleTab& x) const
  {
    Champ_P1NC_implementation::filtrer_H1(x);
  }

  inline void filtrer_resu(DoubleTab& x) const
  {
    Champ_P1NC_implementation::filtrer_resu(x);
  }

private:
  inline const Champ_base& le_champ() const override { return *this; }
  inline Champ_base& le_champ() override { return *this; }
};

#endif /* Champ_P1NC_included */
