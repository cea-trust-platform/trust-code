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

#ifndef Champ_input_P0_included
#define Champ_input_P0_included

#include <Champ_Fonc_P0_base.h>
#include <Champ_Input_Proto.h>

/*! @brief class Champ_input_P0
 *
 *      Cette classe represente un champ accessible par setInputField
 *      defini sur le domane avec une valeur par element.
 *
 *
 * @sa Champ_Input_Proto
 */

class Champ_input_P0 : public Champ_Fonc_P0_base, public Champ_Input_Proto
{
  Declare_instanciable(Champ_input_P0);
public:
  friend class Champ_Input_P0_Composite;

  void mettre_a_jour(double) override { }
  void getTemplate(TrioField& afield) const override;
  void setValue(const TrioField& afield) override;

protected:
  // Factorisation function between several input field classes
  void set_nb_comp(int i) override; // calls fixer_nb_comp
  void set_name(const Nom&) override; // calls nommer
  virtual const Nom& get_name() const; // calls le_nom

private:
  int nb_elems_reels_sous_domaine_=0; //if sous-domaine : number of elements of the sous-domaine on this proc
  int nb_elems_reels_loc_=0;       //total number of local elements
};

#endif /* Champ_input_P0_included */
