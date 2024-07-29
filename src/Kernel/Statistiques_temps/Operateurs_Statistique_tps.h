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

#ifndef Operateurs_Statistique_tps_included
#define Operateurs_Statistique_tps_included

#include <Operateur_Statistique_tps_base.h>
#include <TRUST_List.h>
#include <TRUST_Ref.h>

class Postraitement;
class Motcle;

/*! @brief classe Operateurs_Statistique_tps Cette classe represente une liste d'operateurs statistiques en temps.
 *
 *      Les objets Postraitement portent un objet de ce type.
 *
 * @sa Postraitement
 */
class Operateurs_Statistique_tps : public LIST(DERIV(Operateur_Statistique_tps_base))
{
  Declare_instanciable(Operateurs_Statistique_tps);

public:

  inline int lserie() const;
  inline double dt_integr_serie() const;
  int sauvegarder(Sortie& os) const override;
  int reprendre(Entree& is) override;

protected:

  REF(Postraitement) mon_post;
  int lserie_=-123;
  double dt_integr_serie_=123.;

};

inline int Operateurs_Statistique_tps::lserie() const
{
  return lserie_;
}

inline double Operateurs_Statistique_tps::dt_integr_serie() const
{
  return dt_integr_serie_;
}

#endif
