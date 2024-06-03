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


#ifndef Ch_front_input_included
#define Ch_front_input_included


#include <Ch_front_var_instationnaire_dep.h>
#include <Champ_Input_Proto.h>
#include <TRUSTTabs_forward.h>

/*! @brief class Ch_front_input
 *
 *      Cette classe represente un champ accessible par setInputField
 *      defini sur une frontiere avec une valeur par face.
 *
 *
 * @sa Champ_Input_Proto
 */

class Ch_front_input : public Ch_front_var_instationnaire_dep, public Champ_Input_Proto
{
  Declare_instanciable(Ch_front_input);

public:

  Champ_front_base& affecter_(const Champ_front_base&) override
  {
    return *this;
  }
  void getTemplate(TrioField& afield) const override;
  void setValue(const TrioField& afield) override;
  void setDoubleValue(const double val) override;

  int initialiser(double temps, const Champ_Inc_base& inco) override;
  void buildSommetsFaces() const ; // const because used in Ch_Front_input_ALE::getTemplate() which is const - actually updates the 2 members below due to ALE mesh movement:

protected:

  // Factorisation function between several input field classes
  void set_nb_comp(int i) override; // calls fixer_nb_comp
  void set_name(const Nom& ) override; // calls nommer
  virtual const Nom& get_name() const; // calls le_nom
  mutable DoubleTab sommets_; // mutable because of buildSommetsFaces function used in ALE
  mutable IntTab faces_; // mutable because of buildSommetsFaces function used in ALE


};

#endif

