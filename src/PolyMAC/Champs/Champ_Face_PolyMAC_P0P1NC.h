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

#include <Domaine_PolyMAC_P0P1NC.h>
#include <Champ_Face_PolyMAC.h>
#include <SolveurSys.h>

/*! @brief : class Champ_Face_PolyMAC_P0P1NC
 *
 *  Champ correspondant a une inconnue decrite par ses flux aux faces (type vitesse)
 *  Degres de libertes : composante normale aux faces + composante tangentielle aux aretes de la vorticite
 *
 */
class Champ_Face_PolyMAC_P0P1NC : public Champ_Face_PolyMAC
{
  Declare_instanciable(Champ_Face_PolyMAC_P0P1NC) ;
public :
  inline const Domaine_PolyMAC_P0P1NC& domaine_PolyMAC_P0P1NC() const { return ref_cast(Domaine_PolyMAC_P0P1NC, le_dom_VF.valeur()); }

  DoubleTab& valeur_aux_faces(DoubleTab& result) const override;
  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&, double, int distant) const override;

  int nb_valeurs_nodales() const override;

  int fixer_nb_valeurs_nodales(int n) override;

  virtual void init_auxiliary_variables(); /* demande l'ajout des variables auxiliaires ( [lambda rot u] aux aretes )*/
  int reprendre(Entree& fich) override;

  //interpolations aux elements : vitesse val(e, i) = v_i, gradient vals(e, i, j) = dv_i / dx_j
  void interp_ve(const DoubleTab& inco, DoubleTab& val, bool is_vit = true) const override;
  void interp_ve(const DoubleTab& inco, const IntVect&, DoubleTab& val, bool is_vit = true) const override;
};

#endif /* Champ_Face_PolyMAC_P0P1NC_included */
