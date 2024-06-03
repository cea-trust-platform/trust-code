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

#ifndef Source_Dirac_VDF_Elem_included
#define Source_Dirac_VDF_Elem_included

#include <Terme_Puissance_Thermique_VDF_base.h>
#include <Iterateur_Source_Elem.h>
#include <Eval_Dirac_VDF_Elem.h>
#include <Modele_Permeabilite.h>

/*! @brief class Source_Dirac_VDF_Elem Cette classe represente un  terme source de Dirac.
 *
 */
class Source_Dirac_VDF_Elem : public Terme_Puissance_Thermique_VDF_base
{
  Declare_instanciable_sans_constructeur(Source_Dirac_VDF_Elem);
public:
  Source_Dirac_VDF_Elem() : Terme_Puissance_Thermique_VDF_base(Iterateur_Source_Elem<Eval_Dirac_VDF_Elem>()), nb_dirac(0) { }
  void associer_pb(const Probleme_base& ) override;
  void associer_domaines(const Domaine_dis&, const Domaine_Cl_dis& ) override;
  void mettre_a_jour(double temps) override { Terme_Puissance_Thermique::mettre_a_jour(temps); }

protected:
  DoubleVect point;
  int nb_dirac;
};

#endif /* Source_Dirac_VDF_Elem_included */
