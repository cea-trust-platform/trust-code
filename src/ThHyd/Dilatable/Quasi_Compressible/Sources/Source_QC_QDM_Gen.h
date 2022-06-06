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

#ifndef Source_QC_QDM_Gen_included
#define Source_QC_QDM_Gen_included

#include <Source_base.h>
#include <Source.h>

//////////////////////////////////////////////////////////////////////////////
// .DESCRIPTION
//
//  Classe Source_QC_QDM_Gen
//  Ces classes filles permettent  de calculer une source pour le QC
//  en faisant rho * source_incompressible
//
//////////////////////////////////////////////////////////////////////////////

class Source_QC_QDM_Gen :  public Source_base
{
  Declare_instanciable(Source_QC_QDM_Gen);

public :

  Entree& readOn_spec(Entree& , Nom&);
  void mettre_a_jour(double temps) override;
  void completer() override;
  void creer_champ(const Motcle& motlu) override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  int impr(Sortie& os) const override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& calculer(DoubleTab& ) const override;

protected :
  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& ) override ;
  void associer_pb(const Probleme_base&  ) override ;
  Source source_incompressible;
};

#endif /* Source_QC_QDM_Gen_included */
