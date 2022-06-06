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

#ifndef Loi_Etat_Multi_GP_base_included
#define Loi_Etat_Multi_GP_base_included

#include <Loi_Etat_Melange_GP_base.h>
#include <Ref_Champ_Inc_base.h>
#include <List.h>

class Champ_Inc_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Loi_Etat_Multi_GP_base
//     Cette classe represente la loi d'etat pour un melange de gaz parfaits.
//
// .SECTION voir aussi
//     Fluide_Dilatable_base Loi_Etat_base Loi_Etat_Melange_GP_base
//////////////////////////////////////////////////////////////////////////////

Declare_liste(REF(Champ_Inc_base));

class Loi_Etat_Multi_GP_base : public Loi_Etat_Melange_GP_base
{
  Declare_base(Loi_Etat_Multi_GP_base);

public:
  void calculer_masse_molaire();
  void calculer_Cp() override;
  void calculer_lambda() override;
  void calculer_alpha() override;
  void calculer_mu() override;

  virtual void associer_inconnue(const Champ_Inc_base& inconnue);
  void initialiser_inco_ch() override;
  double calculer_masse_volumique(double,double) const override;
  void calculer_masse_volumique() override =0;
  virtual void calculer_masse_molaire(DoubleTab& M) const = 0;
  virtual void calculer_tab_Cp(DoubleTab& cp) const = 0;
  virtual void calculer_mu_wilke()=0;

  // Methodes inlines
  inline const DoubleTab& masse_molaire() const { return masse_mol_mel; }
  inline DoubleTab& masse_molaire() { return masse_mol_mel; }
  virtual double calculer_masse_volumique(double P,double T,double r) const;

protected :
  void calculer_tab_mu(const DoubleTab& mu, int size);
  LIST(REF(Champ_Inc_base)) liste_Y;
  DoubleTab masse_mol_mel;
};

#endif /* Loi_Etat_Multi_GP_base_included */
