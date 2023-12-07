/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Operateur_Div_base_included
#define Operateur_Div_base_included


/*! @brief Classe Operateur_Div_base Cette classe est la base de la hierarchie des operateurs representant
 *
 *     le calcul de la divergence d'un champ dans une equation.
 *
 * @sa Operateur_base
 */
#include <Operateur_base.h>
#include <SFichier.h>

class Operateur_Div_base  : public Operateur_base
{
  Declare_base(Operateur_Div_base);
public :
  DoubleVect& multvect(const DoubleTab&, DoubleTab&) const;
  virtual void volumique(DoubleTab& ) const=0;

  /* version etendue de ajouter_blocs pour pouvoir donner en argument le champ dont on prend la divergence -> utile pour implementer ajouter() */
  virtual void ajouter_blocs_ext(const DoubleTab& vit, matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const
  {
    Process::exit(que_suis_je() + " : ajouter_blocs_ext() not coded!");
  }

  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override
  {
    return ajouter_blocs_ext(equation().inconnue().valeurs(), matrices, secmem, semi_impl);
  }
  DoubleTab& ajouter(const DoubleTab& vit, DoubleTab& div) const override
  {
    ajouter_blocs_ext(equation().inconnue().valeurs(), { }, div);
    return div;
  }

protected:
  mutable SFichier Flux_div;
};

#endif
