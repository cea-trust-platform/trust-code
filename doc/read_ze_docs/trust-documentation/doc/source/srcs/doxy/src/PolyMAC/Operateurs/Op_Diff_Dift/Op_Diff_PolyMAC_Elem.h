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

#ifndef Op_Diff_PolyMAC_Elem_included
#define Op_Diff_PolyMAC_Elem_included

#include <Op_Diff_PolyMAC_base.h>
#include <Matrice_Morse.h>

class Op_Diff_PolyMAC_Elem: public Op_Diff_PolyMAC_base
{
  Declare_instanciable( Op_Diff_PolyMAC_Elem );
public:
  DoubleTab& ajouter(const DoubleTab&, DoubleTab&) const override;
  virtual void calculer_flux_bord(const DoubleTab& inco) const = delete;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  void modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const override { }
  void completer() override;
  void mettre_a_jour(double t) override
  {
    Op_Diff_PolyMAC_base::mettre_a_jour(t);
    delta_int_a_jour_ = delta_a_jour_ = (stab_ ? 0 : 1);
  }

  void dimensionner(Matrice_Morse& mat) const override;

  void dimensionner_termes_croises(Matrice_Morse&, const Probleme_base& autre_pb, int nl, int nc) const override;
  void ajouter_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, DoubleTab& resu) const override;
  void contribuer_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, Matrice_Morse& matrice) const override;

  /* correction non lineaire de Le Potier / Mahamane : facteurs delta_e aux elements, delta_f aux faces */
  int stab_ = 0;                                    //1 si elle est activee
  void update_delta_int() const; //met a jour le tableau intermediaire delta_f_int et remplit delta_e
  mutable DoubleTab delta_f_int;         //tableaux intermediaires : delta_x_int(i, n, 0/1) : numerateur / denominateur de l'expression de delta_e/f (sans contrib externe)
  void update_delta() const;     //met a jour le tableau final delta_f
  mutable DoubleTab delta_e, delta_f;    //facteurs finaux (apres prise en compte des CL Echange_contact)

private:
  mutable int delta_int_a_jour_ = 0, delta_a_jour_ = 0; //si on doit calculer update_delta_{int,}_
};

/* comme des synonymes, mais avec l'info de ce qu'on est dans que_suis_je() */
class Op_Diff_Nonlinear_PolyMAC_Elem: public Op_Diff_PolyMAC_Elem
{
  Declare_instanciable( Op_Diff_Nonlinear_PolyMAC_Elem );
};

class Op_Dift_PolyMAC_Elem: public Op_Diff_PolyMAC_Elem
{
  Declare_instanciable( Op_Dift_PolyMAC_Elem );
};

class Op_Dift_Nonlinear_PolyMAC_Elem: public Op_Diff_PolyMAC_Elem
{
  Declare_instanciable( Op_Dift_Nonlinear_PolyMAC_Elem );
};

#endif /* Op_Diff_PolyMAC_Elem_included */
