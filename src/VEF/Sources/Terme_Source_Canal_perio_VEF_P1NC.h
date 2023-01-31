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

#ifndef Terme_Source_Canal_perio_VEF_P1NC_included
#define Terme_Source_Canal_perio_VEF_P1NC_included

/*! @brief class Terme_Source_Canal_perio_VEF_P1NC Cette classe permet de conserver le debit dans une simulation
 *
 *   temporelle de Canal
 *
 *
 * @sa Terme_Source_Canal_perio
 */

#include <Terme_Source_Canal_perio.h>
#include <TRUSTTabs_forward.h>
#include <Ref_Domaine_Cl_VEF.h>
#include <Ref_Domaine_VEF.h>

class Navier_Stokes_std;
class Probleme_base;

// La classe derive de Source_base et peut etre d'un terme source
class Terme_Source_Canal_perio_VEF_P1NC : public Terme_Source_Canal_perio
{
  Declare_instanciable(Terme_Source_Canal_perio_VEF_P1NC);

public :
  DoubleTab& ajouter(DoubleTab& ) const override;

protected :

  REF(Domaine_VEF) le_dom_VEF;
  REF(Domaine_Cl_VEF) le_dom_Cl_VEF;
  void associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
  void calculer_debit(double&) const override;
  // les attributs ont ete mis dans la classe mere

};

class Terme_Source_Canal_perio_QC_VEF_P1NC : public Terme_Source_Canal_perio_VEF_P1NC
{
  Declare_instanciable(Terme_Source_Canal_perio_QC_VEF_P1NC);
};


#endif
