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

#ifndef Champ_Face_PolyMAC_P0P1NC_included
#define Champ_Face_PolyMAC_P0P1NC_included

#include <Champ_Face_base.h>
#include <Domaine_PolyMAC_P0P1NC.h>
#include <SolveurSys.h>

/*! @brief : class Champ_Face_PolyMAC_P0P1NC
 *
 *  Champ correspondant a une inconnue decrite par ses flux aux faces (type vitesse)
 *  Degres de libertes : composante normale aux faces + composante tangentielle aux aretes de la vorticite
 *
 */
class Champ_Face_PolyMAC_P0P1NC : public Champ_Face_base
{
  Declare_instanciable(Champ_Face_PolyMAC_P0P1NC) ;
public :
  inline const Domaine_PolyMAC_P0P1NC& domaine_PolyMAC_P0P1NC() const { return ref_cast(Domaine_PolyMAC_P0P1NC, le_dom_VF.valeur()); }

  DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const override;
  double valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const override;
  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const override;
  DoubleTab& valeur_aux_elems_passe(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const override;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const override;

  DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override;
  int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const override;

  DoubleTab& valeur_aux_faces(DoubleTab& result) const override;
  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double, int distant) const override;

  Champ_base& affecter_(const Champ_base&) override;
  int nb_valeurs_nodales() const override;

  int fixer_nb_valeurs_nodales(int n) override;

  virtual void init_auxiliary_variables(); /* demande l'ajout des variables auxiliaires ( [lambda rot u] aux aretes )*/
  int reprendre(Entree& fich) override;

  //interpolations aux elements : vitesse val(e, i) = v_i, gradient vals(e, i, j) = dv_i / dx_j
  void interp_ve(const DoubleTab& inco, DoubleTab& val, bool is_vit = true) const;
  void interp_ve(const DoubleTab& inco, const IntVect&, DoubleTab& val, bool is_vit = true) const;

protected:
  virtual Champ_base& le_champ() { return *this; }
  virtual const Champ_base& le_champ() const { return *this; }
  virtual DoubleTab& valeur_aux_elems_(const DoubleTab& val_face ,const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const;
};


#endif /* Champ_Face_PolyMAC_P0P1NC_included */
