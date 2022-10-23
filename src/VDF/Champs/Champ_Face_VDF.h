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

#ifndef Champ_Face_VDF_included
#define Champ_Face_VDF_included

#include <Champ_Face_VDF_implementation.h>
#include <Champ_Face_base.h>
#include <Zone_VDF.h>

class Zone_Cl_VDF;

/*! @brief class Champ_Face_VDF Cette classe sert a representer un champ vectoriel dont on ne calcule
 *
 *   que les composantes normales aux faces en VDF.Il n'y a donc qu'un degre de
 *   liberte par face et l'attribut nb_comp_ d'un objet de type Champ_Face
 *   vaut 1. On peut neammoins imposer toutes les composantes du champ sur
 *   le bord. Si n est le nombre de faces total de la zone et nb_faces_bord
 *   le nombre de faces de bord le tableau de valeurs associe au champ
 *   est construit comme suit:
 *       - n valeurs pour representer les composantes normales aux faces
 *       - nb_faces_bord*dimension pour stocker les valeurs imposees
 *         sur les faces de bord
 *  Rq : cette classe est specifique au module VDF
 *
 * @sa Champ_Inc_base
 */
class Champ_Face_VDF : public Champ_Face_base, public Champ_Face_VDF_implementation
{
  Declare_instanciable(Champ_Face_VDF);
public:
  int fixer_nb_valeurs_nodales(int) override;

  inline const Zone_VDF& zone_vdf() const override { return ref_cast(Zone_VDF, la_zone_VF.valeur()); }
  Champ_base& affecter_(const Champ_base&) override;
  virtual const Champ_Proto& affecter(const double x1, const double x2);
  virtual const Champ_Proto& affecter(const double x1, const double x2, const double x3);
  virtual const Champ_Proto& affecter(const DoubleTab&);
  void verifie_valeurs_cl() override;
  int compo_normale_sortante(int) const;

  inline const DoubleTab& tau_diag() const { return tau_diag_; }
  inline const DoubleTab& tau_croises() const { return tau_croises_; }
  inline DoubleTab& tau_diag() { return tau_diag_; }
  inline DoubleTab& tau_croises() { return tau_croises_; }
  void dimensionner_tenseur_Grad();
  void calculer_rotationnel_ordre2_centre_element(DoubleTab&) const;
  int imprime(Sortie&, int) const override;
  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double, int distant) const override;
  void mettre_a_jour(double temps) override;

  void calculer_dscald_centre_element(DoubleTab&) const;

  void calcul_critere_Q(DoubleTab&, const Zone_Cl_VDF&);
  void calcul_grad_u(const DoubleTab&, DoubleTab&, const Zone_Cl_VDF&);
  void calcul_y_plus(DoubleTab&, const Zone_Cl_VDF&);

  DoubleTab& calcul_duidxj(const DoubleTab&, DoubleTab&) const;
  DoubleTab& calcul_duidxj(const DoubleTab&, DoubleTab&, const Zone_Cl_VDF&) const;
  DoubleVect& calcul_S_barre(const DoubleTab&, DoubleVect&, const Zone_Cl_VDF&) const;
  DoubleVect& calcul_S_barre_sans_contrib_paroi(const DoubleTab&, DoubleVect&, const Zone_Cl_VDF&) const;
  void calculer_dercov_axi(const Zone_Cl_VDF&);

  // methodes inlines
  inline DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& val, int le_poly) const override
  {
    return Champ_Face_VDF_implementation::valeur_a_elem(position, val, le_poly);
  }

  inline double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const override
  {
    return Champ_Face_VDF_implementation::valeur_a_elem_compo(position, le_poly, ncomp);
  }

  inline DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& tab_valeurs) const override
  {
    return Champ_Face_VDF_implementation::valeur_aux_elems(positions, les_polys, tab_valeurs);
  }

  inline DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& tab_valeurs, int ncomp) const override
  {
    return Champ_Face_VDF_implementation::valeur_aux_elems_compo(positions, les_polys, tab_valeurs, ncomp);
  }

  inline DoubleTab& valeur_aux_sommets(const Domaine& dom, DoubleTab& val) const override
  {
    return Champ_Face_VDF_implementation::valeur_aux_sommets(dom, val);
  }

  inline DoubleVect& valeur_aux_sommets_compo(const Domaine& dom, DoubleVect& val, int comp) const override
  {
    return Champ_Face_VDF_implementation::valeur_aux_sommets_compo(dom, val, comp);
  }

  inline DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override
  {
    return Champ_Face_VDF_implementation::remplir_coord_noeuds(positions);
  }

  inline int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const override
  {
    return Champ_Face_VDF_implementation::remplir_coord_noeuds_et_polys(positions, polys);
  }

private:
  double val_imp_face_bord_private(int face, int comp) const;
  double val_imp_face_bord_private(int face, int comp1, int comp2) const;
  inline const Champ_base& le_champ() const override { return *this; }
  inline Champ_base& le_champ() override { return *this; }

  DoubleTab tau_diag_;       // termes diagonaux du tenseur Grad
  DoubleTab tau_croises_;    // termes extradiagonaux du tenseur Grad
};

double Champ_Face_get_val_imp_face_bord_sym(const DoubleTab& tab_valeurs, const double temp,int face,int comp, const Zone_Cl_VDF& zclo);
double Champ_Face_get_val_imp_face_bord( const double temp,int face,int comp, const Zone_Cl_VDF& zclo) ;
double Champ_Face_get_val_imp_face_bord( const double temp,int face,int comp, int comp2, const Zone_Cl_VDF& zclo) ;

#endif /* Champ_Face_VDF_included */
