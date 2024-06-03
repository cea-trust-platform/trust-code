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

#ifndef Terme_Source_Solide_SWIFT_VDF_included
#define Terme_Source_Solide_SWIFT_VDF_included

/*! @brief class Terme_Source_Solide_SWIFT_VDF Cette classe permet le forcage d'un champ moyen de temperature
 *
 *   d'un solide issu d'un calcul SWIFT vers le champ moyen du calcul
 *   LES grossier correspondant.
 *
 *
 */
#include <Source_base.h>
#include <TRUST_Ref.h>
#include <Champ_Don.h>

class Probleme_base;
class Conduction;

class Terme_Source_Solide_SWIFT_VDF : public Source_base
{
  Declare_instanciable(Terme_Source_Solide_SWIFT_VDF);

public :
  DoubleTab& calculer(DoubleTab& ) const override;
  void init_calcul_moyenne(const Conduction& my_eqn, DoubleVect& Y, IntVect& corresp, IntVect& compt);
  void calcul_moyenne(const Conduction& my_eqn, DoubleVect& T_moy, const IntVect& corresp, const IntVect& compt) const;
  void correspondance_SWIFT_coarse();
  void mettre_a_jour(double temps) override { }

  void associer_pb(const Probleme_base& ) override;
  void associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis& ) override;

  inline void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override {}
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override;
  inline int has_interface_blocs() const override
  {
    return 1;
  };

protected :
  REF(Probleme_base) pb_swift;
  REF(Probleme_base) pb_corse;
  REF(Conduction) eq_swift;
  REF(Conduction) eq_corse;

  double tau = -123.;                                // tau est la constante de temps du forcage.

  DoubleVect Y_swift;                         // Coordonnees des points ou est definie la temperature dans le solide
  DoubleVect Y_corse;                        // ou doit etre fait le forcage.

  IntVect compt_swift;                        // Nombre de points a meme Y.
  IntVect compt_corse;

  IntVect corresp_swift;                  // Correspondance numerotation globale -> numerotation locale dans Y.
  IntVect corresp_corse;
  IntVect corresp_SC;

};
#endif
