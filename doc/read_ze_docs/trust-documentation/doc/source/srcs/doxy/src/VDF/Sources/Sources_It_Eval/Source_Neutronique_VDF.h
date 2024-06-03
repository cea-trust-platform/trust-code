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

#ifndef Source_Neutronique_VDF_included
#define Source_Neutronique_VDF_included

#include <Eval_Puiss_Neutr_VDF_Elem.h>
#include <Iterateur_Source_Elem.h>
#include <Source_Neutronique.h>
#include <Iterateur_Source.h>
#include <TRUST_Ref.h>

class Domaine_VDF;

/*! @brief class Source_Neutronique_VDF Cette classe constitue l'implementation en VDF du terme source
 *
 *   Source_Neutronique dans ThSol/.
 *
 */
class Source_Neutronique_VDF : public Source_Neutronique
{
  Declare_instanciable_sans_constructeur(Source_Neutronique_VDF);
public:
  Source_Neutronique_VDF() : iter(Iterateur_Source_Elem<Eval_Puiss_Neutr_VDF_Elem>()) { }
  Source_Neutronique_VDF(const Iterateur_Source_base& iter_base) : iter(iter_base) { }
  inline DoubleTab& calculer(DoubleTab& resu) const override { return iter->calculer(resu); }
  void completer() override;
  void associer_domaines(const Domaine_dis&, const Domaine_Cl_dis& ) override;
  void associer_pb(const Probleme_base& ) override;
  double calculer_Tmoyenne() override;
  inline void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override {}
  inline void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override { iter->ajouter(secmem); }
  inline int has_interface_blocs() const override { return 1; }

protected:
  Iterateur_Source iter;
  REF(Domaine_VDF) le_dom;
};

#endif /* Source_Neutronique_VDF_included */
