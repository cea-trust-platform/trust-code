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

#ifndef Domaine_EF_axi_included
#define Domaine_EF_axi_included

#include <Domaine_EF.h>

class DomaineAxi1d;
/*! @brief : class Domaine_EF_axi
 *
 *  <Description of class Domaine_EF_axi>
 *
 *
 *
 */

class Domaine_EF_axi : public Domaine_EF
{

  Declare_instanciable( Domaine_EF_axi ) ;

public :

  void calculer_IPhi(const Domaine_Cl_dis_base& zcl) override;
  void calculer_Bij_gen(DoubleTab& bij_) override ;
  void remplir_tableau_origine();
  void discretiser() override;

  const DomaineAxi1d& domaine_axi() const;
  DomaineAxi1d& domaine_axi();

  void verifie_compatibilite_domaine() override;

protected :

  DoubleTab origine_repere_;

};

#endif /* Domaine_EF_axi_included */
